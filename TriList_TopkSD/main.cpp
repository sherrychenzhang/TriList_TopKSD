/*****************************************
 *
 * Triangle Listing based Top-k Structural Diversity Search
 * Author: Lijun Chang
 * Date: 21/01/2016
 * Email: ljchang@outlook.com
 *
 *****************************************/


#include "Utility.h"
#include "Graph.h"

void print_usage() {
	printf("Usage: [1]exe [2]graph-dir [3]k [4]t [5]output\n");
}

int main(int argc, char *argv[]) {
#ifdef _DEBUG_
	printf("**** TriList_TopkSD (Debug): %s *** ", argv[1]);
#else
	printf("**** TriList_TopkSD (Release): %s *** ", argv[1]);
#endif
	printf("\n");

	if(argc < 4) {
		print_usage();
		return 0;
	}

#ifdef _LINUX_
	struct timeval start, end1, end;
	gettimeofday(&start, NULL);
#else
	int start, end1, end;
	start = clock();
#endif

	Graph *graph = new Graph(argv[1]);
	graph->read_graph();
	//printf("\t*** Finished reading graph\n");

#ifdef _LINUX_
	gettimeofday(&end1, NULL);

	long long mtime1, seconds1, useconds1;
	seconds1 = end1.tv_sec - start.tv_sec;
	useconds1 = end1.tv_usec - start.tv_usec;
	mtime1 = seconds1*1000000 + useconds1;
#else
	end1 = clock();
#endif

	int k = atoi(argv[2]);
	int t = atoi(argv[3]);
	if(argc == 5&&strcmp(argv[4], "output") == 0) graph->trilist_topksd(k, t, 1);
	else graph->trilist_topksd(k, t);

#ifdef _LINUX_
	gettimeofday(&end, NULL);

	long long mtime, seconds, useconds;
	seconds = end.tv_sec - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = seconds*1000000 + useconds;

	printf("Total time excluding IO is: %lld\n", mtime-mtime1);
	//printf("IO time: %lld\nHash time: %lld\nTriangle time: %lld\nTotal time(w/o IO): %lld\n", mtime1, mtime2, mtime-mtime1-mtime2, mtime-mtime1);
#else
	end = clock();

	printf("IO time: %d, Hash time: %d, Triangle time: %d, Total time(w/o IO): %d\n", end1-start, end2-end1, end-end2, end-end1);
#endif

	return 0;
}
