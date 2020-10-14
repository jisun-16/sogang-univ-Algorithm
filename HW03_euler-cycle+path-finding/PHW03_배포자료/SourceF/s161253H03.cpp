#define _CRT_SECURE_NO_WARNINGS
using namespace std;
#include <time.h>
#include <stack>
#include "DBL.h"

//#define NO_PATH_OUT   // comment out this line for path output
void Error_Exit(const char *s);

typedef struct _Vertex {
	dblStack S;		// adj list contains edge index
	int degree;
} Vertex;

typedef struct _Edge {
	int v1, v2;
} Edge;

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN);
void adjListGenerate(Vertex *V, Edge *E, int VN, int EN);
void deallocGraph(Vertex *V, Edge *E, int VN);
int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN);

DBList pool;	// DBL storage pool

int main() {
	int T, VN, EN;
	Vertex *V;
	Edge   *E;
	int *path;	// Euler cycle or path
	int pathN;  // path length
	int  flag;	// 0: cycle, 1: path, 2: none
	clock_t start_time, finish_time;

	scanf("%d", &T);	// read # of tests
	for (int t = 1; t <= T; t++) {	// for each test
		graphGeneration(&V, &E, &VN, &EN);

		start_time = clock(); // set the start time

		path = Find_Euler(V, E, VN, EN, &flag, &pathN); // find an Euler path or cycle

		finish_time = clock(); // set finish time
		
		double cmpt = (((double)(finish_time - start_time)) / CLK_TCK)*1000; // compute the time passed
		printf("Test= %d flag= %d VnumInCycle/Path)= %d ", t, flag, pathN);

		if (flag == 0)
			printf("Euler_cycle(exec_time= %.2f msec)\n",cmpt);
		else if (flag == 1)
			printf("Euler_path(exec_time= %.2f msec)\n", cmpt);
		else
			printf("not_Eulerian(exec_time= %.2f msec)\n", cmpt);

#ifndef NO_PATH_OUT
		if (flag != 2) {
			for (int i = 0; i < EN + 1; i++) {
				printf("%d\n", path[i]);
			}
		}
#endif
		if (flag != 2)
			delete[] path;
		deallocGraph(V, E, VN);
	}
	pool.freeDBL_pool();	// clear all the DBL elements

	return 0;
}

int *Find_Euler(Vertex *V, Edge *E, int VN, int EN, int *flag, int *pathN) {
	// input V, VN, E, EN
	// output through paramters
	//   *flag = 0 (Euler cycle), 1 (Euler path), 2 (not Eulerian)
	//   *pathN = size of path[] array
	// output by return
	//   *path = list of vertex ids found(Euler cycle or path)

	stack<int> ST;		// use STL stack as explained in the class
	int *path = NULL;
	*pathN = 0;
	int odd_cnt = 0, start_index = 0; //degree가 홀수인 vertex의 개수를 센다
	for (int i = 0; i < VN; i++) {
		if (V[i].degree % 2 == 1) {
			odd_cnt++;
			start_index = i;
		}
	}

	//degree가 홀수인 vertex가 없을 경우 euler cycle이 존재
	if (odd_cnt == 0)
		*flag = 0;
	//degree가 홀수인 vertex가 2개일 경우 euler path 존재
	else if (odd_cnt == 2)
		*flag = 1;
	//위 두 경우가 아닐 경우 not eulerian
	else {
		*flag = 2;
		return path;
	}

	path = new int[EN + 1]; //euler path(cycle)의 크기는 EN+1이다.
	ST.push(start_index);
	while (!ST.empty()) {
		int v = ST.top();
		if (V[v].degree == 0) {
			path[(*pathN)++] = v;
			ST.pop();
		}
		else {
			//v의 adj list에서 가장 가까운 edge 선택, 제거
			DBL* p1 = V[v].S.pop();
			V[v].degree--;
			//v와 edge로 연결되어 있던 vertex w를 찾아 adj list에서 twin edge를 제거한다.
			int w = (E[p1->d].v1 != v) ? E[p1->d].v1 : E[p1->d].v2;
			DBL* p2 = p1->twin;
			V[w].S.remove(p2);
			V[w].degree--;
			ST.push(w);
			pool.freeDBL(p1);
			pool.freeDBL(p2);
		}
	}

	return path;
}

void deallocGraph(Vertex *V, Edge *E, int VN) {
	DBL *p;

	for (int i = 0; i < VN; i++) {
		while (!V[i].S.empty()) {
			p = V[i].S.pop();
			pool.freeDBL(p);
		}
	}

	delete[] V;
	delete[] E;
}

void graphGeneration(Vertex **V, Edge **E, int *VN, int *EN) {
	scanf("%d %d", VN, EN);	// read # of vertices and edges
	//allocVandEarray(V, E, *VN, *EN);	// vertex and edge array allocation

	*V = new Vertex[*VN];
	*E = new Edge[*EN];
	if (*V == NULL || *E == NULL) {
		Error_Exit("Memory Allocation Error!");
	}

	for (int v = 0; v < *VN; v++) {
		(*V)[v].degree = 0;
	}
	for (int e = 0; e < *EN; e++) {
		scanf("%d %d", &((*E)[e].v1), &((*E)[e].v2));	// read edge information
		++((*V)[(*E)[e].v1].degree);
		++((*V)[(*E)[e].v2].degree);
	}
	adjListGenerate(*V, *E, *VN, *EN);	// create adj lust in G=(V,E)
}

void adjListGenerate(Vertex *V, Edge *E, int VN, int EN) {
	// Construct adjacent list in V array
	int v1, v2;
	DBL *adjE1, *adjE2;
	for (int i = 0; i < EN; i++) {
		//v1,v2는 edge를 구성하는 두 vertex 
		v1 = E[i].v1;
		v2 = E[i].v2;
		//v1과 v2의 adj-list에 저장, twin에 서로의 pointer 저장
		adjE1 = pool.allocDBL();
		adjE2 = pool.allocDBL();
		adjE1->d = i;
		adjE1->twin = adjE2;
		adjE2->d = i;
		adjE2->twin = adjE1;
		//v1과 v2의 스택에 각각 push한다
		V[v1].S.push(adjE1);
		V[v2].S.push(adjE2);
	}
}

void Error_Exit(const char *s) {
	printf("%s", s);
	exit(-1);
}

// allocate a DBL element
DBL *DBList::allocDBL(void) {		
	DBL *p;

	if (DBL_pool == NULL) {
		p = new DBL;
		if (p == NULL)
			Error_Exit("memory alloc error(allocDBL)");
		UsedMemoryForDBLs += sizeof(DBL);
		p->d = NONE;
	}
	else {
		p = DBL_pool;
		DBL_pool = p->rp;
	}

	p->rp = p->lp = p->twin = NULL;
	++DBL_cnt;

	return(p);
}

// move p to pool
void DBList::freeDBL(DBL *p) {
	if (p->d == NONE) {
		Error_Exit("This element is already freed(Free_DBL).");
	}
	
	p->d = NONE;
	p->rp = DBL_pool;
	DBL_pool = p;
	--DBL_cnt;		// reduce # of active DBL elements by one 
}

void DBList::freeDBL_pool(void) {
	DBL *p = DBL_pool;

	while (p != NULL) {
		DBL_pool = p->rp;	// get next pointer
		delete p;
		p = DBL_pool;
		UsedMemoryForDBLs -= sizeof(DBL);
	}
	if (DBL_cnt != 0) {
		Error_Exit("Non-zero DBL_cnt after cleanup.");
	}
}

void dblStack::push(DBL *p) {
	if (ST != NULL) {
		ST->lp = p;
	}
	p->rp = ST;
	p->lp = NULL;
	ST = p;
}

// remove and return p in front of Q
DBL *dblStack::pop(void) {	
	DBL *p;
	p = ST;
	if (ST->rp == NULL) {
		//stack has only one element
		ST = NULL;
	}
	else {
		ST = ST->rp;
		ST->lp = NULL;
	}
	return p;
}

// delete p from ST
void dblStack::remove(DBL *p) {	
	if (ST == p) {
		//first element
		ST = p->rp;
		if (p->rp != NULL) {
			(p->rp)->lp = NULL;
		}
	}
	else {
		(p->lp)->rp = p->rp;
		if (p->rp != NULL) {
			(p->rp)->lp = p->lp;
		}
	}
}

DBL *dblStack::top(void) {
	return ST;
}

bool dblStack::empty(void) {
	if (ST == NULL)
		return true;
	else
		return false;
}
