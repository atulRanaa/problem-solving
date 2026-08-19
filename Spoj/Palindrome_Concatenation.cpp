        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const int N = 1e5+5;
        #define all(x) x.begin(),x.end()
        #define MOD 1000000007
        #define pii pair<int,int>
        #define pis pair<int,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        #define test ll cse;cin>>cse;for(ll I=1;I<=cse;I++)
        #define fast ios_base::sync_with_stdio(0);cin.tie(NULL);cout.tie(NULL);
        #define PI 3.14159265
        #define MAXN 1300005
        #define pb      push_back
        #define mp      make_pair


int max_pal(string str) 
{ 
    int maxLength = 1;
  
    int start = 0; 
    int len = str.length();
  
    int low, high; 
  
    for (int i = 1; i < len; ++i) 
    {
        low = i - 1; 
        high = i; 
        while (low >= 0 && high < len && str[low] == str[high]) 
        { 
            if (high - low + 1 > maxLength) 
            { 
                start = low; 
                maxLength = high - low + 1; 
            } 
            --low; 
            ++high; 
        } 
        low = i - 1; 
        high = i + 1; 
        while (low >= 0 && high < len && str[low] == str[high]) 
        { 
            if (high - low + 1 > maxLength) 
            { 
                start = low; 
                maxLength = high - low + 1; 
            } 
            --low; 
            ++high; 
        } 
    }  
    return maxLength; 
}

vector<int> rabin_karp(string const& s, string const& t) {
    const int p = 31; 
    const int m = 1e9 + 9;
    int S = s.size(), T = t.size();

    vector<long long> p_pow(max(S, T)); 
    p_pow[0] = 1; 
    for (int i = 1; i < (int)p_pow.size(); i++) 
        p_pow[i] = (p_pow[i-1] * p) % m;

    vector<long long> h(T + 1, 0); 
    for (int i = 0; i < T; i++)
        h[i+1] = (h[i] + (t[i] - 'a' + 1) * p_pow[i]) % m; 
    long long h_s = 0; 
    for (int i = 0; i < S; i++) 
        h_s = (h_s + (s[i] - 'a' + 1) * p_pow[i]) % m; 

    vector<int> occurences;
    for (int i = 0; i + S - 1 < T; i++) { 
        long long cur_h = (h[i+S] + m - h[i]) % m; 
        if (cur_h == h_s * p_pow[i] % m)
            occurences.push_back(i);
    }
    return occurences;
}
int main(){
        fast;

        string a, b;
        cin>>a;
        cin>>b;


        cout<< max(max_pal(a),max_pal(b)) <<"\n";
        return 0;
} 