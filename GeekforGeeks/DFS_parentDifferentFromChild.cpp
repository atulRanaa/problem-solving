// Coding with THE STARS - The Arista Hackathon
// Depth first traversal problem

/*
- Sample test case
2
6
cebbcb
4 5
6 3
2 3
3 4
5 1
7
aecbaea
4 2
7 5
3 5
3 1
3 6
2 1
*/
/*
// Sample code to perform I/O:

#include <iostream>

using namespace std;

int main() {
	int num;
	cin >> num;										// Reading input from STDIN
	cout << "Input number is " << num << endl;		// Writing output to STDOUT
}

// Warning: Printing unwanted or ill-formatted data to output will cause the test cases to fail
*/

// Write your code here
#include<bits/stdc++.h>
using namespace std;

const int N = 100005;
vector< int > adj[N];
int colorCnt[N][26];

string s;
void dfs(int node, int par) {
	for(int subNode: adj[node]){
		if(subNode != par){
			dfs(subNode, node);
			for(int i=0;i<26;i++) colorCnt[node][i] += colorCnt[subNode][i];
 		}
	}
	colorCnt[node][s[node-1] - 'a']++;
}
void init(int n) {
	memset(colorCnt, 0, sizeof colorCnt);
	for(int i=0;i<=n;i++) adj[i].clear();
}
int main(){
	int t; cin >> t;
	while(t-- > 0) {
		int n; cin >> n;
		cin >> s;

		int x, y; init(n);
		for(int i=0;i<(n-1);i++){ 
			cin >> x >> y;
			adj[x].push_back(y);
			adj[y].push_back(x);
		}
		dfs(1, 0);

		int ans = 0;
		for(int i=1;i<=n;i++) if(colorCnt[i][s[i-1] - 'a'] == 1) ans++;

		cout << ans << "\n";
	}
}
