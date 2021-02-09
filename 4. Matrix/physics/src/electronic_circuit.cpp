#include "electronic_circuit.h"
#include <cassert>
#include <stack>

namespace Physics {

CircuitGraph::comp_idx_t
CircuitGraph::add_edge(node_idx_t node1, node_idx_t node2, AffineComponent c)
{
	comp_idx_t comp_idx = m_comps.size();
	m_comps.push_back({node1, node2, c});
	if (std::max(node1, node2) >= m_adjacent_nodes.size())
		m_adjacent_nodes.resize(std::max(node1, node2) + 1);
	if (node1 != node2) {
		m_adjacent_nodes[node1].push_back(node2);
		m_adjacent_nodes[node2].push_back(node1);
	}
	return comp_idx;
}

/*  Row number is node real index (not key!), column number is component index(key)
 *  mrx[i][j] = -1, if component j comes out from node i,
 *  mrx[i][j] = 1, if component j comes in to node i,
 *  mrx[i][j] = 0, if component j is not connected to this node or
 * if component j is self-circuited on node i (circuit calculation algorithm treats
 * the latter correctly without any changes)
 */
Maths::Matrix<int>
CircuitGraph::get_incidence_matrix() const
{
	Maths::Matrix<int> mrx(nnodes(), ncomponents()); // +1 - because indices in matrix start from 0
	for (comp_idx_t j = 0; j < ncomponents(); ++j) {
		if (m_comps[j].node1 != m_comps[j].node2) {
			mrx[m_comps[j].node1][j] = -1;
			mrx[m_comps[j].node2][j] = 1;
		}
	}
	/* all other elems are automatically set to 0 by Matrix constructor */
	return mrx;
}

Maths::Matrix<double>
CircuitGraph::get_voltage_srcs_matrix() const
{
	Maths::Matrix<double> mrx(ncomponents(), 1);
	for (comp_idx_t i = 0; i < ncomponents(); ++i)
		if (!m_comps[i].component.is_ideal_current_src())
			mrx[i][0] = m_comps[i].component.get_EMF();
	return mrx;
}

Maths::Matrix<double>
CircuitGraph::get_current_srcs_matrix() const
{
	Maths::Matrix<double> mrx(ncomponents(), 1);
	for (comp_idx_t i = 0; i < ncomponents(); ++i)
		if (m_comps[i].component.is_ideal_current_src())
			mrx[i][0] = m_comps[i].component.get_short_circuit_current();
	return mrx;
}

/* diagonal elements are conductances, other are 0 */
Maths::Matrix<double>
CircuitGraph::get_square_conductance_matrix() const
{
	Maths::Matrix<double> mrx(ncomponents(), ncomponents());
	for (comp_idx_t i = 0; i < ncomponents(); ++i)
		if (!m_comps[i].component.is_ideal_current_src())
			mrx[i][i] = m_comps[i].component.get_diff_conductance();
	return mrx;
}

/*  Result contains nodes, which belong to different connected components.
 * Each connected component is taken into account (i.e. res.size() == number of 
 * connected components) */
std::vector<CircuitGraph::node_idx_t>
connected_components(const CircuitGraph& graph)
{
	using node_idx_t = CircuitGraph::node_idx_t;
	using pair = std::pair<node_idx_t, size_t>; // node idx + next way

	std::vector<node_idx_t> res;
	std::vector<bool> visited(graph.nnodes());

	// dfs-based algorithm
	for (node_idx_t node = 0; node < graph.nnodes(); ++node) {
		if (visited[node])
			continue;
		res.push_back(node);

		std::stack<pair> stk;
		stk.push({node, 0});
		while (!stk.empty()) {
			node_idx_t current_node = stk.top().first;
			size_t& current_way = stk.top().second;
			auto& adjacent_nodes = graph.adjacent_nodes(current_node);

			visited[current_node] = true;
			if (current_way == adjacent_nodes.size()) {
				stk.pop();
				continue;
			}
			node_idx_t next_node = adjacent_nodes[current_way];
			if (!visited[next_node])
				stk.push({next_node, 0});
			++current_way;
		}
	}
	return res;
}

ElectronicCircuit::comp_key_t
ElectronicCircuit::add_component(node_key_t node1, node_key_t node2, AffineComponent c)
{
	m_buf.is_up_to_date = false;
	return m_graph.add_edge(m_indexator.insert(node1), m_indexator.insert(node2), c);
}

double ElectronicCircuit::get_current(comp_key_t c) const
{
	fetch_buffer();
	auto comp = m_graph.get_edge_s(c); // comp_idx_t = comp_key_t
	return comp.component.calc_current(
		m_buf.potentials[comp.node1]
		- m_buf.potentials[comp.node2]);
}

double ElectronicCircuit::get_voltage(node_key_t node1, node_key_t node2) const
{
	fetch_buffer();
	return m_buf.potentials[m_indexator.value_to_index(node1)]
		- m_buf.potentials[m_indexator.value_to_index(node2)];
}

ElectronicCircuit::ComponentData
ElectronicCircuit::get_component_data(comp_key_t c) const
{
	auto comp = m_graph.get_edge(c);  // comp_idx_t = comp_key_t
	return ComponentData(
		m_indexator.index_to_value(comp.node1),
		m_indexator.index_to_value(comp.node2),
		comp.component);
}

void ElectronicCircuit::fetch_buffer() const
{
	if (m_buf.is_up_to_date)
		return;
	m_buf.potentials.clear();

	/* Matrix::cat() below will fail if nnodes() < 2, so treating these cases separetely */
	if (nnodes() == 1)
		m_buf.potentials.push_back(0);
	if (nnodes() == 0 || nnodes() == 1) {
		m_buf.is_up_to_date = true;
		return;
	}

	/*  Circuit must be connected graph to have a single potentials solution
	 * (Voltage between different connected components is unknown)
	 * So we can add virtual root and virtually connect it with all
	 * connected components. It is done on a new graph, in order to not
	 * modify the current on
	 */
	CircuitGraph vcircuit(m_graph); // virtual circuit
	CircuitGraph::node_idx_t vroot = vcircuit.nnodes();

	for (auto node : connected_components(m_graph))
		vcircuit.add_edge(vroot, node, make_resistor(1)); // resistance != 0 may be taken arbitrary

	/*  According to wikipedia solution can be found from matrix equation (some signs were changed):
	 * A*Y*transposed(A)*Phi = A*(J-Y*E)
	 * A - incidence_matrix for all nodes except one
	 * Y - square conductance matrix
	 * Phi - result with potential for all nodes except one. For that one potential=0
	 * J - current srcs matrix
	 * E - voltage srcs matrix
	 */
	Maths::Matrix<double> potentials;
	auto Y = vcircuit.get_square_conductance_matrix();
	auto A = vcircuit.get_incidence_matrix().convert_to<double>();
	A.cut(0, A.nrows() - 2, 0, A.ncolumns() - 1);

	try {
		potentials = (A * Y * A.get_transposed()).get_inverted()
			* A * (vcircuit.get_current_srcs_matrix() - Y * vcircuit.get_voltage_srcs_matrix());
	} catch (Maths::InvertionError&) {
		throw CalculationError();
	}

	assert(potentials.nrows() == vcircuit.nnodes() - 1);
	assert(potentials.ncolumns() == 1);
	for (size_t i = 0; i < potentials.nrows(); ++i)
		m_buf.potentials.push_back(potentials[i][0]);
	m_buf.potentials.push_back(0);
	m_buf.is_up_to_date = true;
}

} // Physics namespace end