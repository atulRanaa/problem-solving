#include <bits/stdc++.h>

using namespace std;

int maxProfit(int price[], int n, int k){
    // table to store results of subproblems profit[t][i] stores maximum profit using
    // atmost t transactions up to day i (including day i)
    int profit[k+1][n+1];
 
    // For day 0, you can't earn money
    // irrespective of how many times you trade
    for (int i= 0; i <= k; i++) profit[i][0] = 0;
 
    // profit is 0 if we don't do any transation (i.e. k =0)
    for (int j= 0; j <= n; j++) profit[0][j] = 0;
 
    // fill the table in bottom-up fashion
    for (int i = 1; i <= k; i++){
        for (int j = 1; j < n; j++){
            int max_so_far = INT_MIN;
 
            for (int m = 0; m < j; m++)
                max_so_far = max(max_so_far, price[j] - price[m] + profit[i-1][m]);
 
            profit[i][j] = max(profit[i][j-1], max_so_far);
        }
    }
    return profit[k][n-1];
}
int main() {
    int q;  cin >> q;
    for(int a0 = 0; a0 < q; a0++){
        int k; cin >> k;
        int n; cin >> n;
        int arr[n];
        for(int arr_i = 0; arr_i < n; arr_i++) cin >> arr[arr_i];
        int result = maxProfit(arr,n,k);
        cout << result << endl;
    }
    return 0;
}
