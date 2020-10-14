#include "Graph_adj_list.h"
#include <queue>

extern void errorExit(const char *s);

SLList2 pool;
unsigned long UsedMemoryForArray = 0;

void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	// V[ ].name과 E[ ].name은 설정할 필요가 없으나 편의상 각 배열에서의
	//   index를 저장한다.
	// V[].flag과 E[].flag은 반드시 false로 설정해야 한다.
	for (int i = 0; i < Vnum; i++) {
		V[i].flag = false;
		V[i].name = i;
	}
	for (int i = 0; i < Enum; i++) {
		E[i].flag = false;
		E[i].name = i;
	}
	// 파일에서 그래프 정보를 읽어 V와 E 배열을 구성한다.
	// V[ ].S는 adjacent list header로 스택 헤더로도 사용된다.
	// 따라서 edge (i,j)를 읽으면 V[i].S에 edge index k를 push하고
	//  마찬가지로 V[j].S에 k를 push한다.
	int v1, v2, cost;
	for (int i = 0; i < Enum; i++) {
		scanf_s("%d %d %d", &v1, &v2, &cost);
		//v1의 adj list에 edge를 넣는다
		SLL* temp = pool.allocSLL();
		temp->i = i;
		temp->p = NULL;
		V[v1].S.push(temp);
		//v2의 adj list에 edge를 넣는다
		SLL* temp2 = pool.allocSLL();
		temp2->i = i;
		temp2->p = NULL;
		V[v2].S.push(temp2);
		//E에 edge를 저장
		E[i].cost = cost;
		E[i].vf = v1;
		E[i].vr = v2;
	}
}

void Free_Graph_adj_list(int Vnum, vertex *V) {
	// V 배열의 adjacency list의 원소를 pop()을 사용하여
	// 모두  pool로 되돌려 준다
	int k;
	for (k = 0; k < Vnum; k++) {
		while (!V[k].S.empty()) {
			//k번째 vertex의 adj list가 빌 때까지 pop
			SLL* temp = V[k].S.pop();
			pool.freeSLL(temp);
		}
	}
}

int BFS_Tree_adj_list(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0)
) {
	// BFS를 사용하여 BFS tree에 속하는 에지의 flag을 true로 설정한다.
	// BFS 시작 vertex는 입력 파일에서 지정된다(src).
	// V[]의 adjacent list 탐색은 .top() 멤버 함수를 사용하여
	//  adj list 헤더를 얻고 이를 따라가며 인접 vertex를 탐색한다.
	// BFS tree에 속한 에지의 cost 함을 return 한다
	//   (not unique but 모든 각 edge cost가 1이면 unique)
	// -- 함수의 모든 parameter가 사용될 필요는 없다.
	// -- BFS를 위한 queue가 필요하면 STL의 queue를 사용해도 좋다

	int ret = 0;
	//탐색할 vertex의 name이 저장되는 queue
	std::queue<int> Q;
	Q.push(src);
	V[src].flag = true;
	while (!Q.empty()) {
		int now = Q.front();
		Q.pop();
		SLL* temp = V[now].S.top();
		while (temp != NULL) {
			int e = temp->i;
			int next = (E[e].vf != now) ? E[e].vf : E[e].vr;
			if (!V[next].flag) {
				V[next].flag = true;
				E[e].flag = true;
				ret += E[e].cost;
				Q.push(next);
			}
			temp = temp->p;
		}
	}
	return ret;
}

// SLL management functions
// 아래 함수들은 02_04_CSE3081_CPrg&DataStructure_추가자료.pdf를
//   참조하여 각 함수들을 채워 넣자.
// 스택만 필요하므로 큐의 멤버 함수는 아래 함수 리스트에 없다

// stack member fuctions
void sllStack2::push(SLL *p) {
	// insert p to the top of stack ST
	p->p = ST;
	ST = p;
}

SLL *sllStack2::pop(void) {
	// remove and return p at the top of ST
	SLL* p = ST;
	if (ST->p == NULL) {
		//스택에 값이 하나밖에 없는 경우
		ST = NULL;
	}
	else
		ST = ST->p;
	return p;
}

SLL *sllStack2::top(void) {
	// return p at the top of ST
	return ST;
}

bool sllStack2::empty(void) {
	// return true if empty
	if (ST == NULL)
		return true;
	else
		return false;
}

// SLList2 member functions		

// allocate an SLL element
SLL *SLList2::allocSLL(void) {
	SLL *p;
	if (SLL_pool == NULL) {
		//새로 allocate해야 하는 경우
		p = (SLL*)malloc(sizeof(SLL));
		if (p == NULL)
			errorExit("Memory allocation error(allocSLL)");
		UsedMemoryForSLLs += sizeof(SLL);
		p->i = NONE;
	}
	else {
		//재활용하는 경우
		p = SLL_pool;
		SLL_pool = p->p;
	}
	p->p = NULL;
	++SLL_cnt;
	return p;
}

// free *p (move it to SLL_pool
void SLList2::freeSLL(SLL *p) {		
	if (p->i == NONE) {
		errorExit("This is already freed(freeSLL).");
	}
	p->i = NONE;
	p->p = SLL_pool;
	SLL_pool = p;
	--SLL_cnt;
}

// clear SLL_pool
void SLList2::freeSLL_pool(void) {	
	SLL *p = SLL_pool;
	while (p != NULL) {
		SLL_pool = p->p;
		free(p);
		UsedMemoryForSLLs -= sizeof(SLL);
		p = SLL_pool;
	}
	if (SLL_cnt != 0) {
		errorExit("Non-zero SLL_cnt after cleanup.");
	}
}