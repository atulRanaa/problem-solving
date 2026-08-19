#include <bits/stdc++.h>
#define ll long long
using namespace std;

struct Node{
    int value;
    Node *ar[2];
    Node(){value=0, ar[0]=ar[1]= NULL;}
};

ll pow2[33];
void powof2(){
	pow2[0]=1;
	for(int i=0;i<33;i++) pow2[i] = (pow2[i-1]+pow2[i-1]); 
}

void insert(Node *head, int n){
    
    Node *t = head;
    for(int i=20; i>=0; i--){
        bool val = n&pow2[i];
        if(t->ar[val] == NULL) t->ar[val]= new Node;

        t = t->ar[val];    
    }
    t->value = n;
}
int query(Node *head, int n){
    Node *t = head;
    for(int i=20; i>=0; i--){
        bool val = n&pow2[i];
        if(t->ar[1-val] != NULL) t= t->ar[1-val];
        else if(t->ar[val] != NULL) t=t->ar[val];
    }
    return n^(t->value);
}


long long dp[902];

int main(){
    powof2();
    int n, ans=0;
    cin>>n;
    int ar[n];
    for(int i=0; i<n; i++) cin>>ar[i];
    
    dp[0] = 0;
    for(int i=1; i<=n; i++) dp[i] = dp[i-1]+ar[i-1];
    
    Node* root = new Node();
    insert(root, 0);
    
    for(int i=1; i<n; i++){
        insert(root, dp[i]);
        for(int j=i+1; j<=n; j++){
            ans = max(ans, query(root, dp[j]-dp[i]));
        }
        for(int j=n; j>i; j--){
            ans = max(ans, query(root, dp[n]-dp[j-1]));
        }
    }
    cout<<ans<<endl;
    return 0;
}
