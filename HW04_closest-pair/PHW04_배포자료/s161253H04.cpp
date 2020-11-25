#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

void merge(unsigned a, unsigned b, unsigned c, double* Z, unsigned* Zid, unsigned* TMP);
double combine(unsigned L, unsigned R, double d_LR, unsigned* pt1, unsigned* pt2, double* X, double* Y, unsigned* Xid, unsigned* Yid, unsigned* TMP);

extern void sortXid(double* X, unsigned* Xid, unsigned* TMP, unsigned N) {
    //using mergesort algorithm
    //X값을 기준으로 비오름차순으로 X좌표를 정렬한다
    if (N > 1) {
        unsigned h = N / 2;
        unsigned m = N - h;

        sortXid(X, Xid, TMP, h); //sort X[0:h-1]
        sortXid(X, Xid + h, TMP, m); //sort X[h:N-1]
        merge(0, h - 1, N - 1, X, Xid, TMP); //merge Xid[0:h-1], Xid[h:N-1]
    }
}

extern double closestPairDC(
    unsigned L, unsigned R,	  // current leftmost and rightmost indices
    unsigned* pt1, unsigned* pt2, // closest pair points indices
    double* X, double* Y,         // (x,y) positions array(input)
    unsigned* Xid,  // point index array(sorted by x coordinates)
    unsigned* Yid,  // point index array(gradually sorted by y coordinates)
    unsigned* TMP,  // temporal index array
    unsigned THR	// threshold value
) {
    unsigned i, j;

    //점들의 개수가 THR보다 작거나 같은 경우 O(n^2)알고리즘을 사용한다
    if (R - L + 1 <= THR) {
        double ret = DBL_MAX;
        //두 점 사이의 최소거리를 구한다
        for (i = L; i < R; i++) {
            j = i + 1;
            double temp = sqrt((X[Xid[i]] - X[Xid[j]]) * (X[Xid[i]] - X[Xid[j]]) + (Y[Xid[i]] - Y[Xid[j]]) * (Y[Xid[i]] - Y[Xid[j]]));
            if (temp < ret) {
                ret = temp;
                *pt1 = Xid[i];
                *pt2 = Xid[j];
            }
        }

        //Xid[L:R]을 Yid[L:R]으로 복사
        for (i = L; i <= R; i++)
            Yid[i] = Xid[i];

        //Yid[L:R]를 Y값 기준으로 비오름차순 정렬한다
        //selection sort
        for (i = L; i <= R; i++) {
            unsigned temp, mm = i;
            for (j = i + 1; j <= R; j++) {
                if (Y[Yid[mm]] < Y[Yid[j]])
                    mm = j;
            }
            temp = Yid[i];
            Yid[i] = Yid[mm];
            Yid[mm] = temp;
        }

        return ret;
    }

    //R-L+1>THR
    double d_L, d_R, d_LR, d_min;
    unsigned pt1_L, pt2_L, pt1_R, pt2_R, pt1_min, pt2_min;
    unsigned M = (L + R) / 2;

    d_L = closestPairDC(L, M, &pt1_L, &pt2_L, X, Y, Xid, Yid, TMP, THR);
    d_R = closestPairDC(M + 1, R, &pt1_R, &pt2_R, X, Y, Xid, Yid, TMP, THR);

    merge(L, M, R, Y, Yid, TMP); //merge Yid[L:M], Yid[M+1,R]

    //d_LR에 min(d_L, d_R)을 저장하고 pt1, pt2에 더 작은 dist의 각 점의 index를 저장한다
    if (d_L < d_R) {
        d_LR = d_L;
        *pt1 = pt1_L;
        *pt2 = pt2_L;
    }
    else {
        d_LR = d_R;
        *pt1 = pt1_R;
        *pt2 = pt2_R;
    }

    d_min = combine(L, R, d_LR, &pt1_min, &pt2_min, X, Y, Xid, Yid, TMP);
    if (d_LR <= d_min)
        return d_LR;
    else {
        *pt1 = pt1_min;
        *pt2 = pt2_min;
        return d_min;
    }
}


void merge(unsigned a, unsigned b, unsigned c, double* Z, unsigned* Zid, unsigned* TMP) {
    //Z를 기준으로 Zid[a:b], Zid[b+1:c]를 비오름차순으로 merge 
    unsigned i = a, j = b + 1, k = a;

    while (i <= b && j <= c) {
        if (Z[Zid[i]] >= Z[Zid[j]])
            TMP[k] = Zid[i++];
        else
            TMP[k] = Zid[j++];
        k++;
    }

    if (i > b) {
        for (; j <= c; j++)
            TMP[k++] = Zid[j];
    }
    else {
        for (; i <= b; i++)
            TMP[k++] = Zid[i];
    }

    for (i = a; i <= c; i++)
        Zid[i] = TMP[i];
}

double combine(unsigned L, unsigned R, double d_LR, unsigned* pt1, unsigned* pt2, double* X, double* Y, unsigned* Xid, unsigned* Yid, unsigned* TMP) {
    //d_LR보다 작은 pair 탐색, d_min return
    unsigned i, j, k = 0;
    double d_min = d_LR, mid, l, r;

    //중간영역의 x값 범위를 구한다
    mid = X[Xid[(L+R)/2]];
    l = mid - d_LR;
    r = mid + d_LR;

    //중간영역 구하기
    for (i = L; i <= R; i++) {
        if (X[Yid[i]] >= l && X[Yid[i]] <= r)
            TMP[k++] = Yid[i];
    }

    //중간영역을 탐색하여 최소거리를 구한다
    for (i = 0; i < k; i++) {
        for (j = i + 1; j < k; j++) {
            if (Y[TMP[i]] - Y[TMP[j]] >= d_min) break;
            double temp = sqrt((X[TMP[i]] - X[TMP[j]]) * (X[TMP[i]] - X[TMP[j]]) + (Y[TMP[i]] - Y[TMP[j]]) * (Y[TMP[i]] - Y[TMP[j]]));
            if (temp < d_min) {
                d_min = temp;
                *pt1 = TMP[i];
                *pt2 = TMP[j];
            }
        }
    }

    return d_min;
}