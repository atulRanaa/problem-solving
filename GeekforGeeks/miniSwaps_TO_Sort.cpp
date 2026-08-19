#include <bits/stdc++.h>

using namespace std;

int minimumSwaps(vector<int> arr) {
    
    int count = 0;
    vector<int> sorted_arr(arr.begin(),arr.end());
    sort(sorted_arr.begin(),sorted_arr.end());
    
    unordered_map<int,int> m; 
    for(int i=0;i<sorted_arr.size();i++) m[ sorted_arr[i] ] = i;

    for(int i=0;i<arr.size();i++){
        if( arr[i] !=  sorted_arr[i]){
            
            int index_at_sorted = m[ arr[i]];
            swap(arr[i] ,arr[ index_at_sorted ]);
            count++;
            i--;
        }
        for(auto ele:arr) cout<<ele<<" ";
        cout<<"\n";
    }
    return count;
}

int main()
{
    
    int n;
    cin >> n;
  

    vector<int> arr(n);

    for (int i = 0; i < n; i++) {
        int arr_item; cin>>arr_item;
        arr[i] = arr_item;
    }

    cout<<minimumSwaps(arr);

    return 0;
}