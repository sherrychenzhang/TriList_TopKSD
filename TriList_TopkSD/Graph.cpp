#include "Utility.h"
#include "Graph.h"

Graph::Graph(const char *_dir) {
	dir = string(_dir);

	n = m = 0;

	pstart = NULL;
	edges = NULL;
}

Graph::~Graph() {
	if(pstart != NULL) {
		delete[] pstart;
		pstart = NULL;
	}
	if(edges != NULL) {
		delete[] edges;
		edges = NULL;
	}
}

void Graph::read_graph() {
	FILE *f = open_file((dir + string("/b_degree.bin")).c_str(), "rb");

	int tt;
	fread(&tt, sizeof(int), 1, f);
	if(tt != (int)sizeof(int)) {
		printf("sizeof int is different: edge.bin(%d), machine(%d)\n", tt, (int)sizeof(int));
		return ;
	}
	fread(&n, sizeof(int), 1, f);
	fread(&m, sizeof(int), 1, f);

	// printf("\tn = %u; m = %u\n", n, m/2);

	ui *degree = new ui[n];
	fread(degree, sizeof(int), n, f);

#ifndef NDEBUG
	long long sum = 0;
	for(ui i = 0;i < n;i ++) sum += degree[i];
	if(sum != m) printf("WA input graph\n");
#endif

	fclose(f);

	f = open_file((dir + string("/b_adj.bin")).c_str(), "rb");

	if(pstart == NULL) pstart = new ui[n+1];
	if(edges == NULL) edges = new ui[m];

	pstart[0] = 0;
	for(ui i = 0;i < n;i ++) {
		//printf("%d %d\n", i, degree[i]);
		if(degree[i] > 0) fread(edges+pstart[i], sizeof(int), degree[i], f);

		pstart[i+1] = pstart[i] + degree[i];
	}

	fclose(f);

	int self_loop = 0, not_sorted = 0;

	for(ui i = 0;i < n;i ++) {
		for(ui j = pstart[i];j < pstart[i+1];j ++) {
			if(edges[j] == i) self_loop = 1;
			if(j > pstart[i]&&edges[j] <= edges[j-1]) not_sorted = 1;
		}
	}

	if(self_loop) printf("!!! Self_loop\n");
	if(not_sorted) {
		printf("!!! Not_sorted\n");
		for(ui i = 0;i < n;i ++) sort(edges+pstart[i], edges+pstart[i+1]);
	}

	delete[] degree;
}

ui Graph::binary_search(const ui *array, ui e, int val) {
	assert(e > 0);
	ui b = 0;
	-- e;
#ifndef NDEBUG
	if(array[e] < val||array[0] > val) {
		printf("Not found in binary_search!\n");
		return e+1;
	}
#endif
	while(b < e) {
		ui mid = b + (e-b)/2;
		if(array[mid] >= val) e = mid;
		else b = mid+1;
	}
	return e;
}

void Graph::link_reverse_edge(Edge *DS) {
	for(ui i = 0;i < n;i ++) for(ui j = pstart[i];j < pstart[i+1];j ++) {
		ui v = edges[j];
		ui d1 = pstart[i+1] - pstart[i], d2 = pstart[v+1] - pstart[v];

		if(d2 < d1||(d1 == d2&&i < v)) continue;

		ui r_id = binary_search(edges+pstart[v], pstart[v+1]-pstart[v], i) + pstart[v];
#ifndef NDEBUG
		if(r_id < pstart[v]||r_id >= pstart[v+1]||i != edges[r_id]) printf("??? WA in cross_link\n");
#endif

		DS[j].reverse = r_id;
		DS[r_id].reverse = j;
	}
}

void Graph::trilist_topksd(int k, int t, int output) {
	Edge *DS = new Edge[m];
	ui *score = new ui[n];
	memset(score, 0, sizeof(ui)*n);

	if(t == 1) for(ui i = 0;i < n;i ++) score[i] = pstart[i+1]-pstart[i];

	link_reverse_edge(DS);
	for(ui i = 0;i < m;i ++) {
		DS[i].parent = i; DS[i].rank = 0; DS[i].count = 1;
	}

	ui *rank_id = new ui[n];
	ui *order = new ui[n];

	build_degree_decreasing_order(rank_id, order);

	ui *hash = new ui[n];
	memset(hash, 0, sizeof(ui)*n);

	ui *min_heap = new ui[k];
	ui heap_n = 0;

	for(ui i = 0;i < n;i ++) {
		ui u = order[i];
		//printf("Degree of %d: %d\n", u, pstart[u+1]-pstart[u]);
#ifndef NDEBUG
		if(rank_id[u] != i) printf("WA in ordering!\n");
#endif

		if(heap_n == k&&score[min_heap[0]] >= (pstart[u+1]-pstart[u])/t) {
			printf("Vertices explored: %d\n", i);
			break;
		}

		for(ui j = pstart[u];j < pstart[u+1];j ++) if(rank_id[edges[j]] > i) hash[edges[j]] = j+1;

		for(ui j = pstart[u];j < pstart[u+1];j ++) if(hash[edges[j]]) {
			ui v = edges[j];

			for(ui k = pstart[v];k < pstart[v+1]&&edges[k] < v;k ++) if(hash[edges[k]]) {
				ui w = edges[k];
				my_union(score, DS, t, u, j, hash[w]-1);
				my_union(score, DS, t, v, DS[j].reverse, k);
				my_union(score, DS, t, w, DS[hash[w]-1].reverse, DS[k].reverse);
			}
		}

		for(ui j = pstart[u];j < pstart[u+1];j ++) hash[edges[j]] = 0;

		if(heap_n < k) {
			min_heap[heap_n] = u;
			bottom_up(min_heap, heap_n, score);
			++ heap_n;
		}
		else if(score[u] > score[min_heap[0]]){
			min_heap[0] = u;
			top_down(min_heap, 0, k, score);
		}
	}

	if(output) {
		vector<pair<int,int> > vp;
		vp.reserve(heap_n);
		for(ui i = 0;i < heap_n;i ++) vp.pb(mp(-score[min_heap[i]], min_heap[i]));
		sort(vp.begin(), vp.end());
		printf("Top-%d structural diverify nodes:\n", k);
		for(ui i = 0;i < heap_n;i ++) printf("%d: %d (score)\n", vp[i].second, -vp[i].first);
	}

	delete[] min_heap;
	delete[] hash;
	delete[] rank_id;
	delete[] order;
	delete[] DS;
	delete[] score;
}

void Graph::bottom_up(ui *min_heap, ui i, ui *score) {
	ui u = min_heap[i];
	while(i > 0&&score[u] < score[min_heap[(i-1)/2]]) {
		min_heap[i] = min_heap[(i-1)/2];
		i = (i-1)/2;
	}
	min_heap[i] = u;
}

void Graph::top_down(ui *min_heap, ui i, ui heap_n, ui *score) {
	ui u = min_heap[i];
	while(i*2+1 < heap_n) {
		ui j = i*2+1;
		if(j+1 < heap_n&&score[min_heap[j+1]] < score[min_heap[j]]) ++ j;

		if(score[min_heap[j]] >= score[u]) break;

		min_heap[i] = min_heap[j];
		i = j;
	}
	min_heap[i] = u;
}

ui Graph::find_root(Edge *DS, int u) {
	ui x = u;
	while(DS[x].parent != x) x = DS[x].parent;

	while(DS[u].parent != x) {
		ui tmp = DS[u].parent;
		DS[u].parent = x;
		u = tmp;
	}

	return x;
}

void Graph::my_union(ui *score, Edge *DS, int t, ui u, ui j, ui k) {
	ui rj = find_root(DS, j);
	ui rk = find_root(DS, k);

	if(rj == rk) return ;

	Edge &e1 = DS[rj];
	Edge &e2 = DS[rk];

	if(e1.count >= t&&e2.count >= t) -- score[u];
	if(e1.count < t&&e2.count < t&&e1.count + e2.count >= t) ++ score[u];

	int cnt = ((int)e1.count) + e2.count;
	if(cnt > t) cnt = t;

	if(e1.rank < e2.rank) {
		e1.parent = rk;
		e2.count = cnt;
	}
	else if(e1.rank > e2.rank) {
		e2.parent = rj;
		e1.count = cnt;
	}
	else {
		e1.parent = rk;
		e2.count = cnt;
		++ e2.rank;
	}
}

void Graph::build_degree_decreasing_order(ui *rank_id, ui *order) {
	memset(order, 0, sizeof(ui)*n);

	for(ui i = 0;i < n;i ++) {
		ui d = pstart[i+1] - pstart[i];
		++ order[d];
	}
	for(ui i = 1;i < n;i ++) order[i] += order[i-1];

	for(ui i = 0;i < n;i ++) {
		ui d = pstart[i+1] - pstart[i];
		rank_id[i] = -- order[d];
	}

	for(ui i = 0;i < n;i ++) {
		rank_id[i] = n-1-rank_id[i];
		order[rank_id[i]] = i;
	}
}
