#include "electronic_circuit.h"
#include <cassert>

namespace Physics {

ElectronicCircuit::comp_key_t
ElectronicCircuit::add_component(node_key_t node1, node_key_t node2, AffineComponent c)
{
	m_comps.push_back({m_nodes.insert(node1), m_nodes.insert(node2), c});
	m_buf.is_up_to_date = false;
	return m_comps.size() - 1;
}

double ElectronicCircuit::get_current(comp_key_t c) const
{
	fetch_buffer();
	return m_comps.at(c).component.calc_current(
		m_buf.potentials.at(m_comps[c].node1)
		- m_buf.potentials.at(m_comps[c].node2));
}

double ElectronicCircuit::get_voltage(node_key_t node1, node_key_t node2) const
{
	fetch_buffer();
	return m_buf.potentials.at(m_nodes.value_to_index(node1))
		- m_buf.potentials.at(m_nodes.value_to_index(node2));
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

Maths::Matrix<double>
ElectronicCircuit::get_voltage_srcs_matrix() const
{
	Maths::Matrix<double> mrx(ncomponents(), 1);
	for (size_t i = 0; i < ncomponents(); ++i)
		if (!m_comps[i].component.is_ideal_current_src())
			mrx[i][0] = m_comps[i].component.get_EMF();
	return mrx;
}

Maths::Matrix<double>
ElectronicCircuit::get_current_srcs_matrix() const
{
	Maths::Matrix<double> mrx(ncomponents(), 1);
	for (size_t i = 0; i < ncomponents(); ++i)
		if (m_comps[i].component.is_ideal_current_src())
			mrx[i][0] = m_comps[i].component.get_short_circuit_current();
	return mrx;
}

/* diagonal elements are conductances, other are 0 */
Maths::Matrix<double>
ElectronicCircuit::get_square_conductance_matrix() const
{
	Maths::Matrix<double> mrx(ncomponents(), ncomponents());
	for (size_t i = 0; i < ncomponents(); ++i)
		if (!m_comps[i].component.is_ideal_current_src())
			mrx[i][i] = m_comps[i].component.get_diff_resistance();
	return mrx;
}

void ElectronicCircuit::fetch_buffer() const
{
	if (m_buf.is_up_to_date)
		return;
	m_buf.potentials.clear();
	/*  According to wikipedia solution can be found from matrix equation:
	 * A*Y*transposed(A)*Phi = A*(J+Y*E)
	 * A - incidence_matrix for all nodes except one
	 * Y - square conductance matrix
	 * Phi - result with potential for all nodes except one. For that one potential=0
	 * J - current srcs matrix
	 * E - voltage srcs matrix
	 */
	Maths::Matrix<double> potentials;
	auto Y = get_square_conductance_matrix();
	auto A = get_incidence_matrix().convert_to<double>();
	try {
		potentials = (A * Y * A.get_transposed()).get_inverted()
			* A * (get_current_srcs_matrix() + Y * get_voltage_srcs_matrix());
	} catch (Maths::InvertionError&) {
		throw std::runtime_error("too hard circuit, sorry:(");
	}
	assert(potentials.nrows() == nnodes() - 1);
	assert(potentials.ncolumns() == 1);
	for (size_t i = 0; i < potentials.nrows(); ++i)
		m_buf.potentials.push_back(potentials[i][0]);
	m_buf.potentials.push_back(0);
	m_buf.is_up_to_date = true;
}

} // Physics namespace end