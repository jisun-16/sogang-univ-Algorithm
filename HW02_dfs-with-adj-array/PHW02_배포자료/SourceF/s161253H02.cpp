#include "Graph_adj_array.h"

void Read_Graph_adj_array (int Vnum, vertex V[], int Enum, edge E[]) {
	// V와 E의 name field는 각각 자신의 인덱스를 저장한다
	// V[].flag과 E[].flag은 반드시 false로 설정해야 한다.
	for (int i = 0; i < Vnum; i++) {
		V[i].flag = false;
		V[i].name = i;
		V[i].f_hd = NONE;
		V[i].r_hd = NONE;
	}
	for (int i = 0; i < Enum; i++) {
		E[i].flag = false;
		E[i].name = i;
		E[i].vf = NONE;
		E[i].vr = NONE;
		E[i].fp = NONE;
		E[i].rp = NONE;
		E[i].cost = 1;
	}
	// read graph information
	for (int i = 0; i < Enum; i++) {
		scanf_s("%d %d %d", &(E[i].vf), &(E[i].vr), &(E[i].cost));
		//forward
		if (V[E[i].vf].f_hd == NONE) {
			V[E[i].vf].f_hd = i;
		}
		else {
			E[i].fp = V[E[i].vf].f_hd;
			V[E[i].vf].f_hd = i;
		}
		//reverse
		if (V[E[i].vr].r_hd == NONE) {
			V[E[i].vr].r_hd = i;
		}
		else {
			E[i].rp = V[E[i].vr].r_hd;
			V[E[i].vr].r_hd = i;
		}
	}
}

int DFS_Tree_adj_array(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0) 
) {  
	// DFS를 사용하여 DFS tree에 속하는 에지의 flag을 true로 설정한다.
	// DFS 시작 vertex는 입력 파일에서 지정된다(src).
	// DFS tree에 속한 에지의 cost 함을 return 한다(모든 각 edge cost가 1이면 unique)
	// recursive 함수로 작성해도 무방하다.
	int ret = 0;
	V[src].flag = true;
	//forward
	for (int i = V[src].f_hd; i != NONE; i = E[i].fp) {
		int next = (E[i].vf != src) ? E[i].vf : E[i].vr;
		if (!V[next].flag) {
			E[i].flag = true;
			ret += E[i].cost + DFS_Tree_adj_array(next, Vnum, V, Enum, E);
		}
	}
	//reverse
	for (int i = V[src].r_hd; i != NONE; i = E[i].rp) {
		int next = (E[i].vf != src) ? E[i].vf : E[i].vr;
		if (!V[next].flag) {
			E[i].flag = true;
			ret += E[i].cost + DFS_Tree_adj_array(next, Vnum, V, Enum, E);
		}
	}
	return ret;
}
