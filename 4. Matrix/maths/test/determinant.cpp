#include <iostream>
#include "matrix.h"

int main()
{
	std::ios::sync_with_stdio(true);

	int mrx_nrows = 0;
	std::cin >> mrx_nrows;
	if (mrx_nrows < 0) {
		fprintf(stderr, "determiant: matrix size must be >= 0\n");
		return EXIT_FAILURE;
	}

	Maths::Matrix<long int> mrx(mrx_nrows, mrx_nrows);
	for (int i = 0; i < mrx_nrows; ++i)
		for (int j = 0; j < mrx_nrows; ++j)
			std::cin >> mrx[i][j];
	if (!std::cin.good()) {
		fprintf(stderr, "determinant: failed to read enough values from stdin: %s",
			(std::cin.eof()) ? "eof reached" : "some error with stdin - try again");
		fprintf(stderr, "\n\t Input format: mrx_size mrx[0][0] mrx[0][1] ... mrx[mrx_size-1][mrx_size-1]\n");
		return EXIT_FAILURE;
	}
	std::cout << mrx.determinant() << std::endl;
	return 0;
}