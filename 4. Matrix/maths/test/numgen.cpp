#include <iostream>

void usage_error()
{
	fprintf(stderr, "usage: ./numgen nvalues minval maxval\n");
	exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
	srand(time(NULL));

	int nvalues = 0;
	int min = 0, max = 0;
	if (argc != 4
		|| sscanf(argv[1], "%d", &nvalues) != 1
		|| sscanf(argv[2], "%d", &min) != 1
		|| sscanf(argv[3], "%d", &max) != 1
		|| nvalues < 0
		|| min > max)
		usage_error();

	for (int i = 0; i < nvalues; ++i)
		printf("%d\n", rand() % (max-min+1) + min);

	return 0;
}