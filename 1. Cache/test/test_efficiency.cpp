#include <iostream>
#include "graph.h"

using std::cout;
using std::endl;

template <class T, class Function>
void walk_graph_randomly(const Graph<T>& graph, Function f)
{
	//auto it = graph.begin() + rand() % graph.nnodes();
	for (auto it = graph.begin(); it != graph.end(); ++it)
		f(*it);
}

void f(int val)
{
	printf("node %d\n", val);
}

int main()
{
	// std::vector<int> a, b;
	// a < b;

	Graph<int> graph;
	Graph<int> other_graph;



	auto fst = graph.add_node(228);
	auto snd = graph.add_node(337);
	auto trd = graph.add_node(455);
	Graph<int>::Iterator it = graph.end();

	graph.add_link(fst, snd);
	graph.add_bidir_link(fst, trd);

	// if (fst.linked_with(snd))
	// 	; // ...

	cout << it.end() - it.begin() << endl;

	
	auto node = graph.wbegin();
	for (auto adjacent_node = node.begin();
		adjacent_node != node.end(); ++adjacent_node)
		if (*adjacent_node == 337)
			cout << "node " << *node << " has adjacent_node with value 337\n";

	return 0;
}