#include "Graph_adj_list.h"
#include <queue>

extern void errorExit(const char *s);

SLList2 pool;
unsigned long UsedMemoryForArray = 0;

void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	// V[ ].name�� E[ ].name�� ������ �ʿ䰡 ������ ���ǻ� �� �迭������
	//   index�� �����Ѵ�.
	// V[].flag�� E[].flag�� �ݵ�� false�� �����ؾ� �Ѵ�.
	for (int i = 0; i < Vnum; i++) {
		V[i].flag = false;
		V[i].name = i;
	}
	for (int i = 0; i < Enum; i++) {
		E[i].flag = false;
		E[i].name = i;
	}
	// ���Ͽ��� �׷��� ������ �о� V�� E �迭�� �����Ѵ�.
	// V[ ].S�� adjacent list header�� ���� ����ε� ���ȴ�.
	// ���� edge (i,j)�� ������ V[i].S�� edge index k�� push�ϰ�
	//  ���������� V[j].S�� k�� push�Ѵ�.
	int v1, v2, cost;
	for (int i = 0; i < Enum; i++) {
		scanf_s("%d %d %d", &v1, &v2, &cost);
		//v1�� adj list�� edge�� �ִ´�
		SLL* temp = pool.allocSLL();
		temp->i = i;
		temp->p = NULL;
		V[v1].S.push(temp);
		//v2�� adj list�� edge�� �ִ´�
		SLL* temp2 = pool.allocSLL();
		temp2->i = i;
		temp2->p = NULL;
		V[v2].S.push(temp2);
		//E�� edge�� ����
		E[i].cost = cost;
		E[i].vf = v1;
		E[i].vr = v2;
	}
}

void Free_Graph_adj_list(int Vnum, vertex *V) {
	// V �迭�� adjacency list�� ���Ҹ� pop()�� ����Ͽ�
	// ���  pool�� �ǵ��� �ش�
	int k;
	for (k = 0; k < Vnum; k++) {
		while (!V[k].S.empty()) {
			//k��° vertex�� adj list�� �� ������ pop
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
	// BFS�� ����Ͽ� BFS tree�� ���ϴ� ������ flag�� true�� �����Ѵ�.
	// BFS ���� vertex�� �Է� ���Ͽ��� �����ȴ�(src).
	// V[]�� adjacent list Ž���� .top() ��� �Լ��� ����Ͽ�
	//  adj list ����� ��� �̸� ���󰡸� ���� vertex�� Ž���Ѵ�.
	// BFS tree�� ���� ������ cost ���� return �Ѵ�
	//   (not unique but ��� �� edge cost�� 1�̸� unique)
	// -- �Լ��� ��� parameter�� ���� �ʿ�� ����.
	// -- BFS�� ���� queue�� �ʿ��ϸ� STL�� queue�� ����ص� ����

	int ret = 0;
	//Ž���� vertex�� name�� ����Ǵ� queue
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
// �Ʒ� �Լ����� 02_04_CSE3081_CPrg&DataStructure_�߰��ڷ�.pdf��
//   �����Ͽ� �� �Լ����� ä�� ����.
// ���ø� �ʿ��ϹǷ� ť�� ��� �Լ��� �Ʒ� �Լ� ����Ʈ�� ����

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
		//���ÿ� ���� �ϳ��ۿ� ���� ���
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
		//���� allocate�ؾ� �ϴ� ���
		p = (SLL*)malloc(sizeof(SLL));
		if (p == NULL)
			errorExit("Memory allocation error(allocSLL)");
		UsedMemoryForSLLs += sizeof(SLL);
		p->i = NONE;
	}
	else {
		//��Ȱ���ϴ� ���
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