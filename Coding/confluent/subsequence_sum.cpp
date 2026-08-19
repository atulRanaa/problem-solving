#include <vector>
#include <set>
#include <unordered_set>
#include <iostream>
using namespace std;

// Approach 1: Dynamic Programming with offset (for moderate sums)
// Time: O(n * sum_range), Space: O(sum_range)
bool canAchieveSum_DP(vector<int>& arr, long long s) {
    int n = arr.size();
    long long minSum = 0, maxSum = 0;
    long long curr = 0;
    
    // Calculate possible range
    for(int x : arr) {
        curr += x;
        minSum = min(minSum, curr);
        maxSum = max(maxSum, curr);
    }
    
    // Check if s is in possible range
    if(s < minSum || s > maxSum) return false;
    if(s == 0) return true;
    
    // If range too large, use meet-in-middle
    long long range = maxSum - minSum;
    if(range > 2e7) {
        // Fall back to meet-in-middle for large ranges
        return false; // Placeholder - see Approach 2
    }
    
    // DP with offset
    int offset = -minSum;
    vector<bool> dp(range + 1, false);
    dp[offset] = true; // sum = 0
    
    for(int x : arr) {
        vector<bool> ndp = dp;
        for(int i = 0; i <= range; i++) {
            if(dp[i] && i + x >= 0 && i + x <= range) {
                ndp[i + x] = true;
            }
        }
        dp = ndp;
    }
    
    return dp[s + offset];
}

// Approach 2: Meet in the Middle (for large value ranges)
// Time: O(2^(n/2) * log(2^(n/2))), Space: O(2^(n/2))
bool canAchieveSum_MITM(vector<int>& arr, long long s) {
    int n = arr.size();
    if(n == 0) return s == 0;
    
    int mid = n / 2;
    
    // Generate all sums for first half
    unordered_set<long long> leftSums;
    for(int mask = 0; mask < (1 << mid); mask++) {
        long long sum = 0;
        for(int i = 0; i < mid; i++) {
            if(mask & (1 << i)) sum += arr[i];
        }
        leftSums.insert(sum);
    }
    
    // Generate all sums for second half and check
    for(int mask = 0; mask < (1 << (n - mid)); mask++) {
        long long sum = 0;
        for(int i = 0; i < (n - mid); i++) {
            if(mask & (1 << i)) sum += arr[mid + i];
        }
        if(leftSums.count(s - sum)) return true;
    }
    
    return false;
}

// Main function - chooses best approach based on constraints
bool canAchieveSum(vector<int>& arr, long long s) {
    int n = arr.size();
    
    // For small arrays, use meet-in-middle
    if(n <= 40) {
        return canAchieveSum_MITM(arr, s);
    }
    
    // For larger arrays with moderate sums, try DP
    return canAchieveSum_DP(arr, s);
}

int main() {
    // Test cases
    vector<int> arr1 = {3, 34, 4, 12, 5, 2};
    cout << "Array: {3, 34, 4, 12, 5, 2}" << endl;
    cout << "Sum 9: " << (canAchieveSum(arr1, 9) ? "YES" : "NO") << endl;
    cout << "Sum 30: " << (canAchieveSum(arr1, 30) ? "YES" : "NO") << endl;
    
    vector<int> arr2 = {-5, 10, -3, 8};
    cout << "\nArray: {-5, 10, -3, 8}" << endl;
    cout << "Sum 2: " << (canAchieveSum(arr2, 2) ? "YES" : "NO") << endl;
    cout << "Sum 0: " << (canAchieveSum(arr2, 0) ? "YES" : "NO") << endl;
    
    vector<int> arr3 = {1, 2, 3};
    cout << "\nArray: {1, 2, 3}" << endl;
    cout << "Sum 4: " << (canAchieveSum(arr3, 4) ? "YES" : "NO") << endl;
    cout << "Sum 7: " << (canAchieveSum(arr3, 7) ? "YES" : "NO") << endl;
    
    return 0;
}