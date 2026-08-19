#include <vector>
#include <set>
#include <iostream>
using namespace std;

class Solution {
public:
    
    int longestPalindromeSubseq(string s) {
        int n = s.size();
        vector<vector<int>> dp(n, vector<int>(n, 0));
        
        // Every single character is a palindrome of length 1
        for(int i = 0; i < n; i++) {
            dp[i][i] = 1;
        }
        
        // Fill for substrings of length 2 to n
        for(int len = 2; len <= n; len++) {
            for(int i = 0; i <= n - len; i++) {
                int j = i + len - 1;
                
                if(s[i] == s[j]) {
                    dp[i][j] = dp[i+1][j-1] + 2;
                } else {
                    dp[i][j] = max(dp[i+1][j], dp[i][j-1]);
                }
            }
        }
        
        return dp[0][n-1];
    }
    
    // Print the actual LPS string using direct DP method
    string getLPS(string s) {
        int n = s.size();
        vector<vector<int>> dp(n, vector<int>(n, 0));
        
        // Build DP table
        for(int i = 0; i < n; i++) {
            dp[i][i] = 1;
        }
        
        for(int len = 2; len <= n; len++) {
            for(int i = 0; i <= n - len; i++) {
                int j = i + len - 1;
                
                if(s[i] == s[j]) {
                    dp[i][j] = dp[i+1][j-1] + 2;
                } else {
                    dp[i][j] = max(dp[i+1][j], dp[i][j-1]);
                }
            }
        }
        
        // Backtrack to construct the LPS
        function<string(int, int)> backtrack = [&](int i, int j) -> string {
            if(i > j) return "";
            if(i == j) return string(1, s[i]);
            
            if(s[i] == s[j]) {
                return s[i] + backtrack(i + 1, j - 1) + s[j];
            } else {
                if(dp[i+1][j] > dp[i][j-1]) {
                    return backtrack(i + 1, j);
                } else {
                    return backtrack(i, j - 1);
                }
            }
        };
        
        return backtrack(0, n - 1);
    }
    
    // Get all possible LPS strings
    vector<string> getAllLPS(string s) {
        int n = s.size();
        vector<vector<int>> dp(n, vector<int>(n, 0));
        
        // Build DP table
        for(int i = 0; i < n; i++) {
            dp[i][i] = 1;
        }
        
        for(int len = 2; len <= n; len++) {
            for(int i = 0; i <= n - len; i++) {
                int j = i + len - 1;
                
                if(s[i] == s[j]) {
                    dp[i][j] = dp[i+1][j-1] + 2;
                } else {
                    dp[i][j] = max(dp[i+1][j], dp[i][j-1]);
                }
            }
        }
        
        // Backtrack to find all LPS
        set<string> result;
        
        function<void(int, int, string)> backtrack = [&](int i, int j, string curr) {
            if(i > j) {
                result.insert(curr);
                return;
            }
            if(i == j) {
                result.insert(curr + s[i]);
                return;
            }
            
            if(s[i] == s[j]) {
                backtrack(i + 1, j - 1, curr + s[i] + s[j]);
            } else {
                if(dp[i+1][j] == dp[i][j]) {
                    backtrack(i + 1, j, curr);
                }
                if(dp[i][j-1] == dp[i][j]) {
                    backtrack(i, j - 1, curr);
                }
            }
        };
        
        backtrack(0, n - 1, "");
        
        // Reconstruct palindromes properly
        vector<string> palindromes;
        for(const string& half : result) {
            string pal = half;
            int len = half.length();
            for(int i = len - 1; i >= 0; i--) {
                pal += half[i];
            }
            // Adjust for middle character if length is odd
            if(dp[0][n-1] % 2 == 1) {
                pal = pal.substr(0, pal.length() / 2 + 1) + 
                      pal.substr(pal.length() / 2 + 1);
            }
            palindromes.push_back(pal);
        }
        
        return vector<string>(result.begin(), result.end());
    }
};

int main() {
    Solution sol;
    
    // Test case 1
    string s1 = "bbbab";
    cout << "String: " << s1 << endl;
    cout << "LPS Length: " << sol.longestPalindromeSubseq(s1) << endl;
    cout << "LPS String: " << sol.getLPS(s1) << endl;
    cout << endl;
    
    // Test case 2
    string s2 = "cbbd";
    cout << "String: " << s2 << endl;
    cout << "LPS Length: " << sol.longestPalindromeSubseq(s2) << endl;
    cout << "LPS String: " << sol.getLPS(s2) << endl;
    cout << endl;
    
    // Test case 3
    string s3 = "abcde";
    cout << "String: " << s3 << endl;
    cout << "LPS Length: " << sol.longestPalindromeSubseq(s3) << endl;
    cout << "LPS String: " << sol.getLPS(s3) << endl;
    cout << endl;
    
    // Test case 4
    string s4 = "aabaa";
    cout << "String: " << s4 << endl;
    cout << "LPS Length: " << sol.longestPalindromeSubseq(s4) << endl;
    cout << "LPS String: " << sol.getLPS(s4) << endl;
    cout << endl;
    
    // Test case 5
    string s5 = "agbdba";
    cout << "String: " << s5 << endl;
    cout << "LPS Length: " << sol.longestPalindromeSubseq(s5) << endl;
    cout << "LPS String: " << sol.getLPS(s5) << endl;
    cout << endl;
    
    // Test case 6 - Palindrome itself
    string s6 = "racecar";
    cout << "String: " << s6 << endl;
    cout << "LPS Length: " << sol.longestPalindromeSubseq(s6) << endl;
    cout << "LPS String: " << sol.getLPS(s6) << endl;
    cout << endl;
    
    return 0;
}