// Coding with THE STARS - The Arista Hackathon
// 2D array maximum K * K matrix sum

#include <bits/stdc++.h>

using namespace std;

int main() {
	int n, m, k; cin >> n >> m >> k;

	int arr[n+5][m+5];
	for(int i=0;i<n;i++) for(int j=0;j<m;j++) arr[i][j]=0;
	
	char ch;
	for(int i=1;i<=n;i++) for(int j=1;j<=m;j++) {
		cin >> ch;
		if(ch == '*') arr[i][j] = 1; else arr[i][j] = 0;
	}

	for(int i=1;i<=n;i++) for(int j=1;j<=m;j++) {
		arr[i][j] += (arr[i-1][j] + arr[i][j-1] - arr[i-1][j-1]);
	}

	int ans = 0;
	for(int i=k;i<=n;i++) for(int j=k;j<=m;j++) {
		ans = max(ans, (arr[i][j] - arr[i-k][j] - arr[i][j-k] + arr[i-k][j-k]));
	}
	cout << ans << "\n";
}
