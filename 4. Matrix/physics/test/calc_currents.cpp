#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <iomanip>
#include "electronic_circuit.h"
#include "scan.h"

void usage_error(const char *msg = "")
{
	fprintf(stderr, "usage: \n"
		"\tn\t--\tprint number of nodes and components\n"
		"\tm\t--\tdump incidence matrix\n"
		"\tv\t--\tdump voltage sources matrix\n"
		"\ti\t--\tdump current sources matrix\n"
		"\tI\t--\tprint currents in components (default option, if nothing specified)\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
try {
	int opt;
	int opt_dump_nodes_and_components = 0;
	int opt_dump_incidence_matrix = 0;
	int opt_dump_voltage_srcs_matrix = 0;
	int opt_dump_current_srcs_matrix = 0;
	int opt_dump_currents = 0;

	while ((opt = getopt(argc, argv, "nmviI")) != -1)
		switch (opt) {
		case 'n': opt_dump_nodes_and_components = 1; break;
		case 'm': opt_dump_incidence_matrix = 1; break;
		case 'v': opt_dump_voltage_srcs_matrix = 1; break;
		case 'i': opt_dump_current_srcs_matrix = 1; break;
		case 'I': opt_dump_currents = 1; break;
		default: usage_error();
		}
	if (!opt_dump_nodes_and_components && !opt_dump_incidence_matrix
		&& !opt_dump_voltage_srcs_matrix && !opt_dump_current_srcs_matrix)
		opt_dump_currents = 1;

	Physics::ElectronicCircuit circuit;

	while (std::cin) {
		std::string line;
		int node1, node2;
		double R;
		double E = 0;

		std::getline(std::cin, line);
		scn::scanner scan(line);

		if (line.size() == 0)
			break;
		scan >> node1 >> scn::skip_spaces >> "--" >> scn::skip_spaces >> node2
			>> scn::skip_spaces >> "," >> scn::skip_spaces >> R;
		try {
			scan >> scn::skip_spaces >> scn::end_of_text;
			circuit.add_component(node1, node2, Physics::make_resistor(R));
			continue;
		} catch (scn::scan_error& e) {}
		scan >> ";" >> scn::skip_spaces >> E >> scn::skip_spaces
			>> "V" >> scn::skip_spaces >> scn::end_of_text;
		circuit.add_component(node1, node2, Physics::make_voltage_src(E, R));
	}

	if (opt_dump_nodes_and_components) {
		printf("nodes: %zu\n", circuit.nnodes());
		printf("components: %zu\n", circuit.ncomponents());
	}

	std::cout << std::left;
	if (opt_dump_incidence_matrix)
		circuit.get_incidence_matrix().dump(std::cout, 2);
	if (opt_dump_voltage_srcs_matrix)
		circuit.get_voltage_srcs_matrix().dump(std::cout, 2);
	if (opt_dump_current_srcs_matrix)
		circuit.get_current_srcs_matrix().dump(std::cout, 2);
	if (opt_dump_currents) {
		for (int comp_idx = 0; comp_idx != circuit.ncomponents(); ++comp_idx) {
			auto comp_data = circuit.get_component_data(comp_idx);
			std::cout << comp_data.node1 << " -- " <<comp_data.node2 << ": "
				<< circuit.get_current(comp_idx) << " A\n";
		}
	}

	return 0;
} catch (scn::scan_error& e) {
	std::cerr << "error while parsing input data: " << e.what() << std::endl;
}