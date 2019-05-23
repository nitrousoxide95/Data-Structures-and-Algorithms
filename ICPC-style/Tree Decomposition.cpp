#include "stdafx.h"
#pragma warning(disable:4996)

/*
Tree Decomposition
Test Problem: NOI 2015 Manager
Summarization of Problem:
Given a tree (by giving the parent of vertex 1...(n-1)) initially all verticies have weight 0
By using "install x", you need to output the number of 0's on the path between root (0) and vertex x;
and set the weight of all vertices on the path as 1
By using "uninstall x", you need to output the number of 1's in the subtree rooted at vertex x;
then set all in the subtree as 0.
*/

/*
Sample Input
7 #number of vertices, rooted at 0
0 0 0 1 1 5 #parents of vertex 1~6
5 #number of queries
install 5
install 6
uninstall 1
install 4
uninstall 0
*/

/*
Sample Output
3
1
3
2
3
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

const int maxn = 110000; 

//Storing the tree
struct edge {
	int v; //the head of an edge
	int next; //it is a (pointer) to the next edge in the linked list.
};

edge g[maxn];
int head[maxn];
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
int tag[maxn << 2], sum[maxn << 2];

//"pushdown" operation

void pushdown(int o, int l, int r) {
	int mid = (l + r) >> 1;
	if (tag[o] != -1) {
		tag[o * 2] = tag[o];
		tag[o * 2 + 1] = tag[o];
		sum[o * 2] = tag[o] * (mid - l + 1);
		sum[o * 2 + 1] = tag[o] * (r - mid);
		tag[o] = -1;
	}
}

//the interval to be updated (ul, ur) and the value to set
int ul, ur, val;

void update(int o, int l, int r) {
	if (ul <= l && r <= ur) {
		tag[o] = val;
		sum[o] = (r - l + 1) * val;
	}
	else {
		int mid = (l + r) >> 1;
		pushdown(o, l, r);
		if (ul <= mid) update(o * 2, l, mid);
		if (mid < ur) update(o * 2 + 1, mid + 1, r);
		//"merging" the left interval and the right interval
		sum[o] = sum[o * 2] + sum[o * 2 + 1];
	}
}

//the interval to query (ql, qr) and the result
int ql, qr, result;

void query(int o, int l, int r) {
	if (ql <= l && r <= qr) {
		result += sum[o];
	}
	else {
		int mid = (l + r) >> 1;
		pushdown(o, l, r);
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
top: the top of the chain belonging to that vertex
r: the right limit of the interval corresponding to the subtree rooted at a vertex in the DFS sequence
*/

bool vis[maxn];
int p[maxn], siz[maxn], son[maxn], dep[maxn], id[maxn], top[maxn], r[maxn];
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

//In this example, we will query the number of 0's on the path between u and v

int query_path(int u, int v) {
	int ret = 0;
	while (top[u] != top[v]) {
		if (dep[top[u]] < dep[top[v]]) swap(u, v);
		//Doing query the segment tree, assunming u is deeper 
		ql = id[top[u]];
		qr = id[u];
		result = 0;
		query(1, 1, n);
		ret += (qr - ql + 1) - result;
		//Moving u upwards
		u = p[top[u]];
	}
	//u and v has the same "top"
	if (dep[u] < dep[v]) swap(u, v);
	ql = id[v];
	qr = id[u];
	result = 0;
	query(1, 1, n);
	ret += (qr - ql + 1) - result;
	return ret;
}

//Setting all on the path between u and v as 1

void update_path(int u, int v) {
	while (top[u] != top[v]) {
		if (dep[top[u]] < dep[top[v]]) swap(u, v);
		//Doing query the segment tree, assunming u is deeper 
		ul = id[top[u]];
		ur = id[u];
		val = 1;
		update(1, 1, n);
		//Moving u upwards
		u = p[top[u]];
	}
	//u and v has the same "top"
	if (dep[u] < dep[v]) swap(u, v);
	ul = id[v];
	ur = id[u];
	val = 1;
	update(1, 1, n);
}

//Get number of 1's in the subtree
int query_subtree(int u) {
	ql = id[u];
	qr = r[u];
	result = 0;
	query(1, 1, n);
	return result;
}

//Set all in the subtree as 0
void update_subtree(int u) {
	ul = id[u];
	ur = r[u];
	val = 0;
	update(1, 1, n);
}

int main()
{
	scanf("%d", &n);
	for (int i = 2;i <= n;i++) {
		int parent;
		scanf("%d", &parent);
		insert(parent + 1, i);
	}
	get_size(1);
	build(1, 1);

	//Testing: "install" - setting all between root and v as 1 
	//"uninstall" - setting all in the subtree as 0
	memset(tag, -1, sizeof tag);
	int q,v;
	char command[15];
	scanf("%d", &q);
	while (q--) {
		scanf("%s%d", command, &v);
		v++;
		if (command[0] == 'i') {
			printf("%d\n", query_path(1, v));
			update_path(1, v);
		}
		else {
			printf("%d\n", query_subtree(v));
			update_subtree(v);
		}
	}
    return 0;
}

