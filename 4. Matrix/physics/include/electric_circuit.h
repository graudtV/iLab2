#ifndef PHYSICS_ELECTRIC_CIRCUIT_H_
#define PHYSICS_ELECTRIC_CIRCUIT_H_

#include <vector>
#include "matrix.h"

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
};

AffineComponent make_resistor(double R) { return {1, R, 0}; }
AffineComponent make_voltage_src(double E, double R = 0) { return {1, R, E}; }
AffineComponent make_current_src(double I) { return {0, 1, -I}; }
AffineComponent make_jumper() { return {1, 0, 0}; }


class ElectronicCircuit final {
public:
	using node_idx_t = size_t; // unique number of node in particular circuit
	using comp_idx_t = size_t; // component index - unique number of component in particular circuit
	
	struct ComponentEntry {
		node_idx_t node1, node2;
		AffineComponent component;

		ComponentEntry(node_idx_t n1, node_idx_t n2, AffineComponent c) :
			node1(n1), node2(n2), component(c) {}
	};

	ElectronicCircuit() {}

	comp_idx_t add_component(node_idx_t node1, node_idx_t node2, AffineComponent c)
	{
		m_comps.push_back({node1, node2, c});
		return m_comps.size() - 1;
	}

	// void unlink_nodes(node_idx_t node1, node_idx_t node2); ???
	double get_current(comp_idx_t c) const { return 0; }
	double get_voltage(node_idx_t node1, node_idx_t node2) const; // phi1 - phi2
	
	ComponentEntry get_component_data(comp_idx_t c) const { return m_comps.at(c); }

	size_t ncomponents() const { return m_comps.size(); }

private:
	std::vector<ComponentEntry> m_comps;
};

} // Physics namespace end

#endif // PHYSICS_ELECTRIC_CIRCUIT_H_