// EE3980 HW07 Grouping Friends
// 105061110, 周柏宇
// 2020/04/24

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int n_names; // number of people
int n_links; // number of communication records
int n_SCCs; // number of friend groups
char **names; // array of names
int **adj_l; // adjacency list
int *adj_l_size; // size of list in adj_l
int *adj_l_ptr; // current position of list in adj_l
int **adj_lT; // adjacency list (transposed)
int *adj_lT_size; // size of list in adj_lT
int *adj_lT_ptr; // current position of list in adj_lT
int *f; // node's finish order
int *idx; // index mapping
int *visited; // record visited nodes
int time_DFS; // DFS clock
int *SCCs; // record nodes traversed by DFS
int SCCs_ptr; // current position of SCCs
int *C; // temporary array for CountingSort()

void readData(); // read data, construct the graph and its transposed version
int nameToIndex(char *name); // convert name to array index
double GetTime(void); // get local time in seconds
void SCC(); // find strongly connected components of a graph
void DFS_Call(int **G, int *len, int *idx);
							  // initialization and recursive DFS function call
void DFS_d(int **G, int *len, int v); // DFS from vertex v of the graph G
void CountingSort(int *idx, int *key, int n, int k);
								 // sort the idx by its key using counting sort
void freeAll(); // free all allocated memory

int main(void)
{
	int i; // loop index
	int component_idx = 0; // index of strongly connected component
	double start, end; // timestamp

	readData(); // read data, construct the graph and its transposed version
	start = GetTime(); // start time
	SCC(); // find strongly connected components of a graph
	end = GetTime(); // end time
	// print out input information, execution time
	printf("N = %d M = %d CPU time = %.5e\n", n_names, n_links, start - end);
	printf("Number of subgroups: %d\n", n_SCCs); // print out number of groups
	for (i = 0; SCCs[i] != -2; i++) { // print out member of groups
		if (i == 0) {
			printf("  Subgroup %d:", ++component_idx);
		}
		else if (SCCs[i] == -1) {
			printf("\n  Subgroup %d:", ++component_idx);
		}
		else {
			printf(" %s", names[SCCs[i]]);
		}
	}
	printf("\n");
	freeAll(); // free all allocated memory

	return 0;
}

void readData()
{
	int i, j, k, l; // indices
	char tmp[20], tmp2[20], tmp3[20]; // temporary variable for input
	int *newptr; // temporary pointer

	// input number of people and communication records
	scanf("%d %d", &n_names, &n_links);
	// allocate memory
	names = (char **)malloc(sizeof(char *) * n_names);
	adj_l = (int **)malloc(sizeof(int *) * n_names);
	adj_l_size = (int *)malloc(sizeof(int) * n_names);
	adj_l_ptr = (int *)calloc(n_names, sizeof(int));
	adj_lT = (int **)malloc(sizeof(int *) * n_names);
	adj_lT_size = (int *)malloc(sizeof(int) * n_names);
	adj_lT_ptr = (int *)calloc(n_names, sizeof(int));
	for (i = 0; i < n_names; i++) {
		scanf("%s", tmp); // input names
		names[i] = (char *)malloc(sizeof(char) * (3 * strlen(tmp) + 1));
		strcpy(names[i], tmp);
	}
	for (i = 0; i < n_names; i++) {
		// initialize the adjacency list for the graph
		adj_l_size[i] = n_links / n_names;
		adj_l[i] = (int *)malloc(sizeof(int) * adj_l_size[i]);
		// initialize the adjacency list for the transposed graph
		adj_lT_size[i] = n_links / n_names;
		adj_lT[i] = (int *)malloc(sizeof(int) * adj_lT_size[i]);
	}
	for (i = 0; i < n_links; i++) {
		// input communication records
		scanf("%s %s %s", tmp, tmp2, tmp3);
		// convert names to array indices
		j = nameToIndex(tmp);
		k = nameToIndex(tmp3);

		// dynamically allocate memory for the adjacency list
		if (adj_l_ptr[j] >= adj_l_size[j]) {
			adj_l_size[j] *= 2; // double the size
			newptr = (int *)malloc(sizeof(int) * adj_l_size[j]);
											   // allocate memory with new size
			for (l = 0; l < adj_l_ptr[j]; l++) newptr[l] = adj_l[j][l];
												 // copy the data to new memory
			free(adj_l[j]); // free old memory block
			adj_l[j] = newptr; // assign the new pointer
		}
		adj_l[j][adj_l_ptr[j]] = k; // store the edge <j, k>
		adj_l_ptr[j]++; // update current position in the list

		// dynamically allocate memory for the adjacency list
		if (adj_lT_ptr[k] >= adj_lT_size[k]) {
			adj_lT_size[k] *= 2; // double the size
			newptr = (int *)malloc(sizeof(int) * adj_lT_size[k]);
											   // allocate memory with new size
			for (l = 0; l < adj_lT_ptr[k]; l++) newptr[l] = adj_lT[k][l];
												 // copy the data to new memory
			free(adj_lT[k]); // free old memory block
			adj_lT[k] = newptr; // assign the new pointer
		}
		adj_lT[k][adj_lT_ptr[k]] = j; // store the edge <k, j>
		adj_lT_ptr[k]++; // update current position in the list
	}
}

int nameToIndex(char *name)
{
	int i;

	for (i = 0; i < n_names; i++) {
		if (!strcmp(name, names[i])) return i;
	}
	return -1;
}

double GetTime(void)						// get local time in seconds
{
	struct timeval tv;						// variable to store time

	gettimeofday(&tv, NULL);				// get local time

	return tv.tv_sec + 1e-6 * tv.tv_usec;	// return local time in seconds
}

void SCC()
{
	int i;
	double t0, t1, t2, t3, t4, t5;

	t0 = GetTime();
	f = (int *)malloc(sizeof(int) * n_names);
	SCCs = (int *)calloc(n_names * 2 + 1, sizeof(int));
	idx = (int *)malloc(sizeof(int) * n_names);
	visited = (int *)malloc(sizeof(int) * n_names);
	C = (int *)malloc(sizeof(int) * n_names);

	for (i = 0; i < n_names; i++) idx[i] = i;
	t1 = GetTime();
	printf("Index init & malloc: %.5es\n", t1 - t0);

	DFS_Call(adj_l, adj_l_ptr, idx);
	t2 = GetTime();
	printf("First DFS: %.5es\n", t2 - t1);

	CountingSort(idx, f, n_names, n_names);
	t3 = GetTime();
	printf("Sort 1: %.5es\n", t3 - t2);

	for (i = 0; i < n_names; i++) CountingSort(adj_lT[i], f, 0, adj_lT_ptr[i] - 1);
	t4 = GetTime();
	printf("Sort 2: %.5es\n", t4 - t3);

	DFS_Call(adj_lT, adj_lT_ptr, idx);
	t5 = GetTime();
	printf("Second DFS: %.5es\n", t5 - t4);
}

void DFS_Call(int **G, int *len, int *idx)
{
	int i, j;

	for (i = 0; i < n_names; i++) visited[i] = 0;
	time_DFS = 0;
	SCCs_ptr = 0;
	n_SCCs = 0;
	for (i = 0; i < n_names; i++) {
		j = idx[i];
		if (visited[j] == 0) {
			SCCs[SCCs_ptr++] = -1;
			n_SCCs++;
			DFS_d(G, len, j);
		}
	}
	SCCs[SCCs_ptr] = -2;
}

void DFS_d(int **G, int *len, int v)
{
	int i, j;

	visited[v] = 1;
	SCCs[SCCs_ptr++] = v;
	for (i = 0; i < len[v]; i++) {
		j = G[v][i];
		if (visited[j] == 0) {
			DFS_d(G, len, j);
		}
	}
	f[v] = time_DFS++; // finish order
}

void CountingSort(int *idx, int *key, int n, int k)
{
	int i;

	for (i = 0; i < k; i++) C[i] = 0;
	for (i = 0; i < n; i++) C[key[idx[i]]]++;
	for (i = 1; i < k; i++) C[i] += C[i - 1];
	for (i = n - 1; i >= 0; i--) {
		idx[i] = n - (C[key[idx[i]]] - 1) - 1;
		C[key[idx[i]]]--;
	}
}

void freeAll()
{
	int i;

	for (i = 0; i < n_names; i++) {
		free(names[i]);
		free(adj_l[i]);
		free(adj_lT[i]);
	}
	free(names);
	free(adj_l);
	free(adj_l_size);
	free(adj_l_ptr);
	free(adj_lT);
	free(adj_lT_size);
	free(adj_lT_ptr);
	free(visited);
	free(f);
	free(idx);
	free(SCCs);
	free(C);
}
