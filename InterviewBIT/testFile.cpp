#include <bits/stdc++.h>

using namespace std;

/*

int longestConsecutive(const vector<int> &ar) {
    // Do not write main() function.
    // Do not read input, instead use the arguments to the function.
    // Do not print the output, instead return values as specified
    // Still have a doubt. Checkout www.interviewbit.com/pages/sample_codes/ for more details

    set<int> s(ar.begin(),ar.end());
    vector<int> v(s.begin(),s.end());
    
    int ans=0,cur=1,l = v.size();
    for(int i=1;i<l;i++){
        //cout<<v[i]<<" ";
        if(v[i-1] == v[i]-1) cur++;
        else{
            if(cur > ans) ans = cur;
            cur = 1;
        }
        //cout<<cur<<" ";
    }
    return ans;
}
*/
/* 
bool checkDiagonal(int** mat, int i, int j,int N,int M)
{
    int res = mat[i][j];
    while (++i < N && ++j < M)
    {
        if (mat[i][j] != res)
            return false;
    }
    return true;
}
int toeplizMatrix(int N,int M,int** mat)
{
    for (int i = 0; i < M; i++)
    {
        if (!checkDiagonal(mat, 0, i,N,M))
            return -1;
    }
    for (int i = 1; i < N; i++)
    {
        if (!checkDiagonal(mat, i, 0,N,M))
            return -1;
    }
    return 1;
}
*/
int main()
{
    /*
    map< pair<int ,int> ,vector< int > > m;
    vector< int > v={1,2,3,4,2,3,3,2,4,3,23,4,3};
    m.insert({{1,2},v });
    if(m.find({1,2})!=m.end()) cout<<"sdfsdf";
    // cout<<m[{1,2}];
    m[{1,3}]= v;
    for (std::vector<int>::iterator i = m[{1,2}].begin(); i != m[{1,2}].end(); ++i)
    {
    	cout<<*i<<" ";
    }
    */
    /*int A;
    cin>>A;
    int s = 2*A-1;
    vector< vector<int> > v;
    for(int i=1;i<=s;i++){
        vector<int> temp;
        for(int j=1;j<=s;j++){
            temp.push_back(max(abs(i-A),abs(j-A))+1);
        }
        v.push_back(temp);
    }*/
    /*
    map< string , int > m;
    vector< pair<int,string> > v;
    string s;
    int n;    // number of words
    cin>>n;
    for(int i=0;i<n;i++){
        cin>>s;
        if(m.find(s) != m.end()) m[s] += 1;   // increasing count in map
        else m[s] = 1;                        // assigning inital count
    }
    // as map can not be sorted according.based on value, it already sorted by keys
    // copy element of map into vector
    for(auto it = m.begin(); it!=m.end() ;it++){
        v.push_back({it->second ,it->first});
    }
    sort(v.begin(),v.end());  // sorting vector by count of word
    //int l = v.size();
    for(auto it=v.begin();it!=v.end();it++)     // second most accuring but word is lexicographically greatest
        cout<<it->second<<" "<<it->first<<endl;
    */
    /*
    int t;
    cin>>t;
    while(t--){
        int n,k;
        cin>>n>>k;
        vector<int> ar;
        int temp;
        for(int i=0;i<n;i++){
        
            cin>>temp;
            ar.push_back(temp);
        }
        int i=0,j=n,result = -1;
        while(i < j){
            int mid = (i+j)/2;
            if(ar[mid]==k) result = mid,j = mid;
            else if( k < ar[mid]) j = mid-1;
            else i = mid+1;
        }
        cout<<result<<endl;
    }
    */
    //const std::vector<int> v={6,4,5,2,3};
    //cout<<longestConsecutive(v);
    
    /*int t;
    cin>>t;
     while(t--){
         int n,num;
         cin>>n>>num;
         
         vector<int> ar(n);
         int temp;
         for(int i=0;i<n;i++){
             cin>>temp;
             ar.push_back(temp);
         }
     int count=0;
     for(auto i=lower_bound(ar.begin(),ar.end(),num);i!=ar.end();i++){
         if(*i != num) break;
         count++;
     }
     cout<<count<<endl;
     
     }
     */   
    return 0;
}
/* bool comp(vector< pair<int,string> > &a,vector< pair<int,string> > &b){
 *       return a.first > b.first;   
 * }
 * use it before main()
 * modify sort()
 * sort(v.begin(),v.end(),comp);
 */