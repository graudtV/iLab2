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


class ElectronicCircuit final {
public:
	using node_key_t = size_t; // unique number, choosen by user, to work with node in particular circuit
	using comp_key_t = size_t; // unique number of component in particular circuit. Returned by ElectronicCircuit::add_component
	
	using NodeIndexator = other::Indexator<node_key_t, size_t>;
	using real_node_idx_t = NodeIndexator::index_type; // successive index of node for implementation purposes
	
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
	double get_voltage(node_key_t node1, node_key_t node2) const; // phi1 - phi2
	
	ComponentData get_component_data(comp_key_t c) const;
	const NodeIndexator& get_indexator() const { return m_nodes; }

	size_t nnodes() const { return m_nodes.size(); }
	size_t ncomponents() const { return m_comps.size(); }

	Maths::Matrix<int> get_incidence_matrix() const;
	Maths::Matrix<double> get_voltage_srcs_matrix() const;
	Maths::Matrix<double> get_current_srcs_matrix() const;
	Maths::Matrix<double> get_square_conductance_matrix() const;

private:
	struct HiddenComponentEntry;

	std::vector<HiddenComponentEntry> m_comps;
	NodeIndexator m_nodes;
	mutable struct {
		std::vector<double> potentials;
		bool is_up_to_date = false;
	} m_buf;

	void fetch_buffer() const;
};

/* similar to ComponentData, but holds indices of nodes instead of keys */
struct ElectronicCircuit::HiddenComponentEntry {
	real_node_idx_t node1, node2;
	AffineComponent component;

	HiddenComponentEntry(real_node_idx_t n1, real_node_idx_t n2, AffineComponent c) :
		node1(n1), node2(n2), component(c) {}
};

} // Physics namespace end

#endif // PHYSICS_ELECTRONIC_CIRCUIT_H_