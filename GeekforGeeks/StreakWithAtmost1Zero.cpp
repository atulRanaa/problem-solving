
// Coding with THE STARS - The Arista Hackathon
// two pointer problem

// Write your code here
# include<bits/stdc++.h>
using namespace std;

int main(){
	int t; cin >> t;
	while(t-- > 0) {
		int n; cin >> n;
		string s; cin >> s;

		int x = 0, y = 0;
		int count = 0, ans = 0;

		if(s[y] == '0') count++;
		while(x < n) {
			y++;
			while(y < n){
				if(s[y] == '0') count++;
				if(count > 1) break;
				y++;
			}

			// cout << x << " " << y << " " << " " << count << " " << ans << "\n";
			ans = max(ans, (y-x));
			if(s[x] == '0') count--;
			x++;
			// cout << x << " " << y << " " << " " << count << " " << ans << "\n";
		}
		
		cout << ans << "\n";
	}
}