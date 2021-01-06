#include "electronic_circuit.h"

namespace Physics {

ElectronicCircuit::comp_key_t
ElectronicCircuit::add_component(node_key_t node1, node_key_t node2, AffineComponent c)
{
	m_comps.push_back({m_nodes.insert(node1), m_nodes.insert(node2), c});
	return m_comps.size() - 1;
}

ElectronicCircuit::ComponentData
ElectronicCircuit::get_component_data(comp_key_t c) const
{
	return ComponentData(
		m_nodes.index_to_value(m_comps[c].node1),
		m_nodes.index_to_value(m_comps[c].node2),
		m_comps[c].component);
}

/* row number is node real index (not key!), column number is component index(key)
 * mrx[i][j] = -1, if component j comes out from node i,
 * mrx[i][j] = 1, if component j comes in to node i,
 * mrx[i][j] = 0, if component j is not connected to this node
 */
Maths::Matrix<int>
ElectronicCircuit::get_incidence_matrix() const
{
	Maths::Matrix<int> mrx(nnodes(), ncomponents()); // +1 - because indices in matrix start from 0
	for (size_t j = 0; j < ncomponents(); ++j) {
		mrx[m_comps[j].node1][j] = -1;
		mrx[m_comps[j].node2][j] = 1;
	}
	/* all other elems are automatically set to 0 by Matrix constr */
	return mrx;
}

// Maths::Matrix<double>
// ElectronicCircuit::get_voltage_srcs_matrix() const
// {
// 	Maths::Matrix<double> mrx(ncomponents(), 1);
// 	for (size_t i = 0; i < ncomponents(); ++i)
// 		if (!m_comps[i].component.is_ideal_current_src())
// 			mrx[i][0] = m_comps[i].component.get_EMF();
// 	return mrx;
// }

// Maths::Matrix<double>
// ElectronicCircuit::get_current_srcs_matrix() const
// {
// 	Maths::Matrix<double> mrx(ncomponents(), 1);
// 	for (size_t i = 0; i < ncomponents(); ++i)
// 		if (m_comps[i].component.is_ideal_current_src())
// 			mrx[i][0] = m_comps[i].component.get_short_circuit_current();
// 	return mrx;
// }

} // Physics namespace end