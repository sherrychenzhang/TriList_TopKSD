#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "Utility.h"

using namespace std;

struct Edge {
	ui reverse; //cross link
	ui parent; //used for disjoint-set data structure
	unsigned char rank, count; //used for disjoint-set data structure
};

class Graph {
private:
	string dir; //input graph directory
	ui n, m; //number of nodes and edges of the graph

	ui *pstart; //offset of neighbors of nodes
	ui *edges; //adjacent ids of edges

public:
	Graph(const char *_dir) ;
	~Graph() ;

	void read_graph() ;

	void trilist_topksd(int k, int t, int output = 0) ;

private:
	ui binary_search(const ui *array, ui e, int val) ;
	void link_reverse_edge(Edge *DS) ;
	void build_degree_decreasing_order(ui *rank_id, ui *order) ;
	void my_union(ui *score, Edge *DS, int t, ui u, ui j, ui k) ;
	ui find_root(Edge *DS, int u) ;

	void bottom_up(ui *min_heap, ui i, ui *score) ;
	void top_down(ui *min_heap, ui i, ui heap_n, ui *score) ;
};

#endif
