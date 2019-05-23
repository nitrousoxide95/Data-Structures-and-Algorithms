#include "stdafx.h"
#pragma warning(disable:4996)

/*
An Additional Example (Source: ZJOI 2008 Count)
Given a tree with at most 30,000 vertices
Each vertex has a weight
Three types of queries (at most 200, 000 queries)
CHANGE x y: change the weight at vertex x to y
QMAX x y: get the maximum weight on the path between x and y
QSUM x y: get the sum of weight on the path between x and y
*/

/*
Sample Input:
4
1 2
2 3
4 1
4 2 1 3
12
QMAX 3 4
QMAX 3 3
QMAX 3 2
QMAX 2 3
QSUM 3 4
QSUM 2 1
CHANGE 1 5
QMAX 3 4
CHANGE 3 6
QMAX 3 4
QMAX 2 4
QSUM 3 4
*/

/*
Sample Output:
4
1
2
2
10
6
5
6
5
16
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

using namespace std;

/*
maxn indicates the size of input. I will usually set maxn to be 110% of the maximum possible input size.
However, some prefer to let maxn be the exact maximum possible input size.
*/

const int maxn = 31000; 

//Storing the tree
struct edge {
	int v; //the head of an edge
	int next; //it is a (pointer) to the next edge in the linked list.
};

edge g[maxn << 1];
int head[maxn], w[maxn];
int n; //number of vertices

//Insert directed edge u->v to the graph g
void insert(int u, int v) {
	static int id;
	g[++id].v = v;
	g[id].next = head[u];
	head[u] = id;
}

//The underlying data structure is often a segment tree.
//The variable tag denotes the value set for that interval. If it is -1, then it means that segment is not
//set by a single value.
//sum denotes the sum of all element in a interval
int maxi[maxn << 2], sum[maxn << 2];

//the interval to be updated (ul, ur) and the value to set
int pos, val;

void update(int o, int l, int r) {
	if (l == r) {
		sum[o] = maxi[o] = val;
	}
	else {
		int mid = (l + r) >> 1;
		if (pos <= mid) update(o * 2, l, mid);
		else update(o * 2 + 1, mid + 1, r);
		//"merging" the left interval and the right interval
		sum[o] = sum[o * 2] + sum[o * 2 + 1];
		maxi[o] = max(maxi[o * 2], maxi[o * 2 + 1]);
	}
}

//the interval to query (ql, qr) and the result
int ql, qr, sumi, maxim;

void query(int o, int l, int r) {
	if (ql <= l && r <= qr) {
		sumi += sum[o];
		maxim = max(maxim, maxi[o]);
	}
	else {
		int mid = (l + r) >> 1;
		if (ql <= mid) query(o * 2, l, mid);
		if (mid < qr) query(o * 2 + 1, mid + 1, r);
	}
}


//Tree Decomposition
//It will decompose the tree into multiple linear "chains". 

/*
vis: denoting whether a vertex has been visited in the DFS process
p: the parent of a node
siz: the size of the subtree rooted at vertex v
son: the son with maximum subtree size
dep: the depth of the vertex
id: the location of vertex v in the segment tree after tree decomposition
ori: the inverse of id - indicing what is the original vertex corresponding to id i
top: the top of the chain belonging to that vertex
r: the right limit of the interval corresponding to the subtree rooted at a vertex in the DFS sequence
*/

bool vis[maxn];
int p[maxn], siz[maxn], son[maxn], dep[maxn], id[maxn], ori[maxn], top[maxn], r[maxn];
void get_size(int u) {	
	vis[u] = true;
	siz[u] = 1;
	son[u] = 0;
	for (int i = head[u];i;i = g[i].next) {
		int v = g[i].v;
		if (!vis[v]) {
			p[v] = u;
			dep[v] = dep[u] + 1;
			get_size(v);
			if (siz[v] > siz[son[u]]) {
				son[u] = v;
			}
			siz[u] += siz[v];
		}
	}
}

/*
Doing the actual tree decomposition
*/
void build(int u,int topi) {
	static int stamp;
	id[u] = ++stamp;
	ori[stamp] = u;
	top[u] = topi;
	//If it has "heaviest son", go to that son first
	if (son[u] != 0) build(son[u], topi);
	for (int i = head[u];i;i = g[i].next) {
		int v = g[i].v;
		if (v != son[u] && v != p[u]) build(v, v);
	}
	//Getting the upper limit
	r[u] = stamp;
}

//Init the segment tree based on initial values, array w

void build_seg(int o, int l, int r) {
	if (l == r) {
		sum[o] = maxi[o] = w[ori[l]];
	}
	else {
		int mid = (l + r) >> 1;
		build_seg(o * 2, l, mid);
		build_seg(o * 2 + 1, mid + 1, r);
		sum[o] = sum[o * 2] + sum[o * 2 + 1];
		maxi[o] = max(maxi[o * 2], maxi[o * 2 + 1]);
	}
}


//In this example, we will query the number of 0's on the path between u and v

int query_max(int u, int v) {
	int ret = -123456;
	while (top[u] != top[v]) {
		if (dep[top[u]] < dep[top[v]]) swap(u, v);
		//Doing query the segment tree, assunming u is deeper 
		ql = id[top[u]];
		qr = id[u];
		maxim = -123456;
		query(1, 1, n);
		ret = max(ret, maxim);
		//Moving u upwards
		u = p[top[u]];
	}
	//u and v has the same "top"
	if (dep[u] < dep[v]) swap(u, v);
	ql = id[v];
	qr = id[u];
	maxim = -123456;
	query(1, 1, n);
	ret = max(ret, maxim);
	return ret;
}

int query_sum(int u, int v) {
	int ret = 0;
	while (top[u] != top[v]) {
		if (dep[top[u]] < dep[top[v]]) swap(u, v);
		//Doing query the segment tree, assunming u is deeper 
		ql = id[top[u]];
		qr = id[u];
		sumi = 0;
		query(1, 1, n);
		ret += sumi;
		//Moving u upwards
		u = p[top[u]];
	}
	//u and v has the same "top"
	if (dep[u] < dep[v]) swap(u, v);
	ql = id[v];
	qr = id[u];
	sumi = 0;
	query(1, 1, n);
	ret  += sumi;
	return ret;
}

void update(int u, int v){
	pos = id[u];
	val = v;
	update(1, 1, n);
}

int main()
{
	scanf("%d", &n);
	for (int i = 2;i <= n;i++) {
		int u, v;
		scanf("%d%d", &u, &v);
		insert(u,v);
		insert(v,u);
	}
	for (int i = 1;i <= n;i++){
		scanf("%d", &w[i]);
	}
	get_size(1);
	build(1, 1);
	build_seg(1, 1, n);
	
	int q, x, y;
	char command[15];
	scanf("%d", &q);
	while (q--) {
		scanf("%s%d%d", command, &x, &y);
		if (command[0] == 'C') {
			update(x, y);
		}
		else if (command[1] == 'M'){
			printf("%d\n", query_max(x, y));
		}
		else{
			printf("%d\n", query_sum(x, y));
		}
	}
    return 0;
}

