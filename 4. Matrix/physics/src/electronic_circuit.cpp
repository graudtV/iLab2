#include "electronic_circuit.h"

namespace Physics {

Maths::Matrix<int>
ElectronicCircuit::get_incidence_matrix() const
{
	Maths::Matrix<int> mrx(max_node_idx() + 1, ncomponents()); // +1 - because indices in matrix start from 0
	for (size_t j = 0; j < ncomponents(); ++j) {
		mrx[m_comps[j].node1][j] = -1;
		mrx[m_comps[j].node2][j] = 1;
	}
	/* all other elems are automatically set to 0 by Matrix constr */
	return mrx;
}


} // Physics namespace end