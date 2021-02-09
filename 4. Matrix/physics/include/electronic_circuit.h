#ifndef PHYSICS_ELECTRONIC_CIRCUIT_H_
#define PHYSICS_ELECTRONIC_CIRCUIT_H_

#include <vector>
#include <set>
#include <unordered_map>
#include "matrix.h"
#include "indexator.h"

namespace Physics {

/* Volt-ampere characteristic: AU = BI + C
 * If U = phi1 - phi2, then I is current from 1 to 2
 * (So for resistor U = IR, for voltage source: U = E + IR (note signs)) */
struct AffineComponent {
public:
	double A, B, C;

	AffineComponent(double AA, double BB, double CC) : A(AA), B(BB), C(CC) {}
	
	bool is_resistor() const { return C == 0; }
	bool is_jumper() const { return C == 0 && B == 0; }
	bool is_ideal_voltage_src() const { return B == 0; }
	bool is_ideal_current_src() const { return A == 0; }

	double get_EMF() const { return C / A; } // electromotive force (i.e. open circuit voltage). inf for ideal current source
	double get_short_circuit_current() const { return -C / B; } // inf for ideal voltage source
	double get_diff_resistance() const { return B / A; } // inf for ideal current source
	double get_diff_conductance() const { return 1 / get_diff_resistance(); } // inf for ideal voltage source

	double calc_current(double V) const { return (A * V - C) / B; } // undefined for ideal voltage source
	double calc_voltage(double I) const { return (B * I + C) / A; } // undefined for ideal current source
};

inline AffineComponent make_resistor(double R) { return {1, R, 0}; }
inline AffineComponent make_voltage_src(double E, double R = 0) { return {1, R, E}; } // E is voltage in open circuit
inline AffineComponent make_current_src(double I) { return {0, 1, -I}; }
inline AffineComponent make_jumper() { return {1, 0, 0}; }

struct CalculationError : public std::runtime_error {
	CalculationError() : std::runtime_error("calculation failure") {}
};

/* Representation of circuit as a graph */
class CircuitGraph {
public:
	using node_idx_t = size_t; /* indices must be successive  */
	using comp_idx_t = size_t;

	struct GraphEdge {
		node_idx_t node1, node2;
		AffineComponent component;

		GraphEdge(node_idx_t n1, node_idx_t n2, AffineComponent c) :
			node1(n1), node2(n2), component(c) {}
	};

	comp_idx_t add_edge(node_idx_t node1, node_idx_t node2, AffineComponent c);
	GraphEdge get_edge(comp_idx_t c) const { return m_comps[c]; }
	GraphEdge get_edge_s(comp_idx_t c) const { return m_comps.at(c); }

	size_t nnodes() const { return m_adjacent_nodes.size(); }
	size_t ncomponents() const { return m_comps.size(); }
	const std::vector<node_idx_t> adjacent_nodes(node_idx_t node) const { return m_adjacent_nodes.at(node); }

	/* building different describing data structures */
	Maths::Matrix<int> get_incidence_matrix() const;
	Maths::Matrix<double> get_voltage_srcs_matrix() const;
	Maths::Matrix<double> get_current_srcs_matrix() const;
	Maths::Matrix<double> get_square_conductance_matrix() const;

protected:
	std::vector<GraphEdge> m_comps;
	std::vector<std::vector<node_idx_t>> m_adjacent_nodes;
};

std::vector<CircuitGraph::node_idx_t> connected_components(const CircuitGraph& graph);

/* Implements algorithms + provide more convenient key access to nodes */
class ElectronicCircuit final {
public:
	using node_key_t = size_t; // unique number, choosen by user, to work with node in particular circuit
	using comp_key_t = size_t; // unique number of component in particular circuit. Returned by ElectronicCircuit::add_component
	
	using NodeIndexator = other::Indexator<node_key_t, size_t>;
	
	struct ComponentData {
		node_key_t node1, node2;
		AffineComponent component;

		ComponentData(node_key_t n1, node_key_t n2, AffineComponent c) :
			node1(n1), node2(n2), component(c) {}
	};

public:
	ElectronicCircuit() {}

	comp_key_t add_component(node_key_t node1, node_key_t node2, AffineComponent c);

	double get_current(comp_key_t c) const;
	double get_voltage(node_key_t node1, node_key_t node2) const; // phi1 - phi2. Voltage between different connected components is undefined
	
	ComponentData get_component_data(comp_key_t c) const;
	const NodeIndexator& get_indexator() const { return m_indexator; }
	const CircuitGraph& get_circuit_graph() const { return m_graph; }

	size_t nnodes() const { return m_graph.nnodes(); }
	size_t ncomponents() const { return m_graph.ncomponents(); }

private:
	CircuitGraph m_graph;
	NodeIndexator m_indexator;
	mutable struct {
		std::vector<double> potentials;
		bool is_up_to_date = false;
	} m_buf;

	void fetch_buffer() const;
};

} // Physics namespace end

#endif // PHYSICS_ELECTRONIC_CIRCUIT_H_