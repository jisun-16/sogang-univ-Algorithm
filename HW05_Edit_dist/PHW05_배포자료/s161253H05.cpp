#include<stdio.h>
#include <stdlib.h>
#include <string.h>

int min(int a, int b, int c);

extern void Edit_Distance(
	// inputs
	char* SS,		// Source string array
	char* TS,		// Edited string array
	int ins_cost, int del_cost, int sub_cost,
	// insertion, deletion, substitution cost
	// outputs
	int** Table,    // Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
	char** SR,		// Source string with insertion to T marks '*'
	char** OP,		// Operation sequence('i', 'd', 's', or '.' (nop))
	char** TR,       // Edited string with deletion form S mark '*'
	// NOTE: ST, OP, TR must be allocated inside the function as needed(minimum as possible)
	int* Mem_Allocated
) {
	int SN, TN, N, i, j, k;

	SN = (int)strlen(SS);
	TN = (int)strlen(TS);

	//initialize Table
	//S[0:i-1] -> NULL, delete i times
	for (i = 0; i <= SN; i++)
		Table[i][0] = i * del_cost;
	//NULL -> T[0:i-1], insert i times
	for (i = 0; i <= TN; i++)
		Table[0][i] = i * ins_cost;

	//fill Table
	//S[0:i-1] -> T[0:j-1] = min(S[0:i-1] -> T[0:j-2] and insert T[j-1], S[0:i-2] -> T[0:j-1] and delete S[i-1], S[0:i-2] -> T[0:j-2] and substitute S[i-1]->T[j-1]
	for (i = 1; i <= SN; i++)
		for (j = 1; j <= TN; j++)
			Table[i][j] = min(Table[i][j - 1] + ins_cost, Table[i - 1][j] + del_cost, Table[i - 1][j - 1] + (SS[i - 1] == TS[j - 1] ? 0 : sub_cost));
	
	//SR, TR, OP의 메모리 할당값을 구한다. 메모리할당값은 SN + number of insertion
	N = SN, i = SN, j = TN;
	while (i && j) {
		//insert와 delete이 둘 다 가능한 경우 더 cost가 작은 것 선택
		if (ins_cost <= del_cost) {
			if (Table[i][j] == Table[i][j - 1] + ins_cost) {
				N++; j--;
			}
			else if (Table[i][j] == Table[i - 1][j] + del_cost)
				i--;
			else {
				i--; j--;
			}
		}
		else {
			if (Table[i][j] == Table[i - 1][j] + del_cost)
				i--;
			else if (Table[i][j] == Table[i][j - 1] + ins_cost) {
				N++; j--;
			}
			else {
				i--; j--;
			}
		}
	}
	while (j > 0) {
		j--; N++;
	}

	//back-tracing
	*SR = new char[N + 1];
	*TR = new char[N + 1];
	*OP = new char[N + 1];
	*Mem_Allocated = 3 * (N + 1);
	*(*SR + N) = '\0';
	*(*TR + N) = '\0';
	*(*OP + N) = '\0';

	i = SN, j = TN, k = N - 1;
	while (i && j) {
		if (ins_cost <= del_cost) {
			//parent의 선택이 둘 이상일 때 ins_cost <= del_cost일 경우 insert 선택
			if (Table[i][j] == Table[i][j - 1] + ins_cost) {
				*(*OP + k) = 'i';
				*(*SR + k) = '*';
				*(*TR + k) = TS[j - 1];
				k--; j--;
			}
			else if (Table[i][j] == Table[i - 1][j] + del_cost) {
				*(*OP + k) = 'd';
				*(*SR + k) = SS[i - 1];
				*(*TR + k) = '*';
				k--; i--;
			}
			else {
				*(*OP + k) = (SS[i - 1] == TS[j - 1]) ? '.' : 's';
				*(*SR + k) = SS[i - 1];
				*(*TR + k) = TS[j - 1];
				k--; i--; j--;
			}
		}
		else {
			//parent의 선택이 둘 이상일 때 ins_cost > del_cost일 경우 delete 선택
			if (Table[i][j] == Table[i - 1][j] + del_cost) {
				*(*OP + k) = 'd';
				*(*SR + k) = SS[i - 1];
				*(*TR + k) = '*';
				k--; i--;
			}
			else if (Table[i][j] == Table[i][j - 1] + ins_cost) {
				*(*OP + k) = 'i';
				*(*SR + k) = '*';
				*(*TR + k) = TS[j - 1];
				k--; j--;
			}
			else {
				*(*OP + k) = SS[i - 1] == TS[j - 1] ? '.' : 's';
				*(*SR + k) = SS[i - 1];
				*(*TR + k) = TS[j - 1];
				k--; i--; j--;
			}
		}
	}
	while (i) {
		//i>0, 즉 S가 남을 경우 나머지는 전부 delete
		*(*OP + k) = 'd';
		*(*SR + k) = SS[i - 1];
		*(*TR + k) = '*';
		k--; i--;
	}
	while (j) {
		//j>0, 즉 T가 남을 경우 나머지는 전부 insert
		*(*OP + k) = 'i';
		*(*SR + k) = '*';
		*(*TR + k) = TS[j - 1];
		k--; j--;
	}

	return;
}

int min(int a, int b, int c) {
	if (a <= b&&a <= c) return a;
	else if (b <= a&&b <= c) return b;
	else return c;
}