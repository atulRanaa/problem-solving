#include <vector>
#include <set>
#include <iostream>
using namespace std;

class Solution {
public:
    // Returns length of LCS
    int longestCommonSubsequence(string text1, string text2) {
        int n = text1.size();
        int m = text2.size();

        vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));

        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                if(text1[i] == text2[j])
                    dp[i+1][j+1] = dp[i][j] + 1;
                else
                    dp[i+1][j+1] = max(dp[i+1][j], dp[i][j+1]);
            }
        }

        return dp[n][m];
    }
    
    // Returns the actual LCS string by backtracking
    string getLCS(string text1, string text2) {
        int n = text1.size();
        int m = text2.size();

        vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));

        // Fill DP table
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                if(text1[i] == text2[j])
                    dp[i+1][j+1] = dp[i][j] + 1;
                else
                    dp[i+1][j+1] = max(dp[i+1][j], dp[i][j+1]);
            }
        }

        // Backtrack to find the LCS string
        string lcs = "";
        int i = n, j = m;
        
        while(i > 0 && j > 0) {
            if(text1[i-1] == text2[j-1]) {
                // Characters match - part of LCS
                lcs = text1[i-1] + lcs;
                i--;
                j--;
            }
            else if(dp[i-1][j] > dp[i][j-1]) {
                // Move up
                i--;
            }
            else {
                // Move left
                j--;
            }
        }

        return lcs;
    }

    // Returns all possible LCS strings (if multiple exist)
    vector<string> getAllLCS(string text1, string text2) {
        int n = text1.size();
        int m = text2.size();

        vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));

        // Fill DP table
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                if(text1[i] == text2[j])
                    dp[i+1][j+1] = dp[i][j] + 1;
                else
                    dp[i+1][j+1] = max(dp[i+1][j], dp[i][j+1]);
            }
        }

        // Backtrack to find all LCS strings
        set<string> result;
        function<void(int, int, string)> backtrack = [&](int i, int j, string curr) {
            if(i == 0 || j == 0) {
                reverse(curr.begin(), curr.end());
                result.insert(curr);
                return;
            }

            if(text1[i-1] == text2[j-1]) {
                backtrack(i-1, j-1, curr + text1[i-1]);
            }
            else {
                if(dp[i-1][j] == dp[i][j]) {
                    backtrack(i-1, j, curr);
                }
                if(dp[i][j-1] == dp[i][j]) {
                    backtrack(i, j-1, curr);
                }
            }
        };

        backtrack(n, m, "");
        return vector<string>(result.begin(), result.end());
    }
};

int main() {
    Solution sol;
    
    // Test case 1
    string text1 = "abcde";
    string text2 = "ace";
    
    cout << "Text 1: " << text1 << endl;
    cout << "Text 2: " << text2 << endl;
    cout << "LCS Length: " << sol.longestCommonSubsequence(text1, text2) << endl;
    cout << "LCS String: " << sol.getLCS(text1, text2) << endl;
    cout << endl;
    
    // Test case 2
    text1 = "abc";
    text2 = "abc";
    
    cout << "Text 1: " << text1 << endl;
    cout << "Text 2: " << text2 << endl;
    cout << "LCS Length: " << sol.longestCommonSubsequence(text1, text2) << endl;
    cout << "LCS String: " << sol.getLCS(text1, text2) << endl;
    cout << endl;
    
    // Test case 3
    text1 = "abc";
    text2 = "def";
    
    cout << "Text 1: " << text1 << endl;
    cout << "Text 2: " << text2 << endl;
    cout << "LCS Length: " << sol.longestCommonSubsequence(text1, text2) << endl;
    cout << "LCS String: " << sol.getLCS(text1, text2) << endl;
    cout << endl;
    
    // Test case 4 - Multiple LCS
    text1 = "abcbdab";
    text2 = "bdcaba";
    
    cout << "Text 1: " << text1 << endl;
    cout << "Text 2: " << text2 << endl;
    cout << "LCS Length: " << sol.longestCommonSubsequence(text1, text2) << endl;
    cout << "One LCS: " << sol.getLCS(text1, text2) << endl;
    cout << "All LCS: ";
    vector<string> allLCS = sol.getAllLCS(text1, text2);
    for(const string& s : allLCS) {
        cout << s << " ";
    }
    cout << endl;
    
    return 0;
}