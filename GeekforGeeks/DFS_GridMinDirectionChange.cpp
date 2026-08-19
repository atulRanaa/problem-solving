#include<bits/stdc++.h>
using namespace std;

#define MEM(p, v) memset(p, v, sizeof(p))
template< class T > bool inside(T a, T b, T c) { return a<=b && b<=c; }
const int MAX = 1001;

char grid[MAX][MAX];
int d[MAX][MAX];
int R, C, sr, sc, er, ec;
int dr[] = {0,1,0,-1};
int dc[] = {1,0,-1,0};

bool valid(int r, int c) {
	return (inside(0, r, R-1) && inside(0, c, C-1));
}

int bfs() {
	int r1, c1, r2, c2, i, dir;
	queue< int > Q;
	
	MEM(d, 0x3f);
	d[sr][sc] = -1;
	Q.push(sr);
	Q.push(sc);
	while(!Q.empty()) {
	
		r1 = Q.front(); Q.pop();
		c1 = Q.front(); Q.pop();
	
		for(i = 0; i < 4; i++) {
			r2 = r1 + dr[i];
			c2 = c1 + dc[i];
			while(valid(r2, c2) && grid[r2][c2] != '*') {
	
				if(d[r2][c2] > d[r1][c1] + 1) {   // non -visited site
					d[r2][c2] = d[r1][c1] + 1;
					
					if(r2==er && c2==ec) return d[r2][c2];
					
					Q.push(r2);
					Q.push(c2);
				}
				r2 += dr[i];
				c2 += dc[i];
			}
		}
	}
	return d[er][ec];
}

int main() {

	int i, j, cnt0 = 0,cnt1 = 0;
	scanf("%d %d", &R, &C);
	for(i = 0; i < R; i++) {
		scanf("%s", grid[i]);

		for(j = 0; j < C && (cnt0 < 1 || cnt1 < 1); j++) {
			if(grid[i][j]=='V') {sr = i; sc = j; cnt0++;}
			if(grid[i][j]=='H') {er = i; ec = j; cnt1++;}
		}
	}
    int ans = bfs();
    if(ans > max(R,C)) ans =-1;
	printf("%d", ans);
	return 0;
}