#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
int factors(int n) {
    map<int, int> prime;

    while(n % 2 == 0) {
        prime[2]++;
        n /= 2;
    }
    for(ll i = 3; i * i <= n; i += 2) {
        while(n % i == 0) {
            prime[i]++;
            n /= i;
        }
    }

    if(n > 1)
        prime[n]++;

    int ans = 1;
    for(auto &[_, p]: prime) {
        ans *= (p + 1);
    }

    return ans;
}

int main() {
    int n;
    cin >> n;
    
    vector<int> arr(n);
    for(int &e: arr) {
        cin >> e;
    }
    
    unordered_map<int, int> dp;
    int answer = 0;
    for(int i = 0; i < n; i++) {
        int e = arr[i];
        dp[e] = max({dp[e], dp[e-1], dp[e/2], dp[e/3]}) + factors(e);
        
        if(dp[e] > answer) {
            answer = dp[e];
        }
    }

    cout << answer << "\n";
    
    return 0;
}
