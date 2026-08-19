#include <bits/stdc++.h>
using namespace std;
int ROW,COL;
int ans[1001][1001];

struct Point
{
    int x;
    int y;
};

struct queueNode
{
    Point pt;  
    int dist;
};
 
bool isValid(int row, int col)
{
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

int rowNum[] = {-1, 0, 0, 1};
int colNum[] = {0, -1, 1, 0};
void BFS(char mat[1001][1001], Point src) //, Point dest)
{
    //if (!mat[src.x][src.y] || !mat[dest.x][dest.y])
    //    return INT_MAX;
 
    bool visited[ROW][COL];
    memset(visited, false, sizeof visited);
 
    visited[src.x][src.y] = true;
 
    queue<queueNode> q;
    queueNode s = {src, 0};
    q.push(s);
 
    while (!q.empty())
    {
        queueNode curr = q.front();
        Point pt = curr.pt;
        //if (pt.x == dest.x && pt.y == dest.y) return curr.dist;
        q.pop();
        for (int i = 0; i < 4; i++)
        {
            int row = pt.x + rowNum[i];
            int col = pt.y + colNum[i];
             
            if (isValid(row, col) && mat[row][col]=='O' && !visited[row][col])
            {
           
                visited[row][col] = true;
                queueNode Adjcell = { {row, col},curr.dist + 1 };
                ans[ row][col] = curr.dist+1;
                q.push(Adjcell);
            }
        }
    }
    //return INT_MAX;
}
int main(){
	int q;					cin>>ROW>>COL>>q;
	char mat[1001][1001];
	for(int i=0;i<ROW;i++)	cin>>mat[i];
	int x,y,a,b;
	cin>>x>>y;
	x -= 1;
	y -= 1;
	Point source = {x,y};
	BFS( mat, source);
	q--;
	while(q-- > 0){
		cin>>a>>b;
		if(a-1 == x && b-1 == y) cout<<0<<"\n";
		else if( ans[a-1][b-1] == 0) cout<<-1<<"\n";
		else cout<<ans[a-1][b-1]<<"\n";
	}
	/*
	for(int i=0;i< ROW; i++){
		for(int j=0;j< COL;j++) cout<<ans[i][j]<<" ";
		cout<<"\n";
	}
	*/
	return 0;
}
