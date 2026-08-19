
#include<bits/stdc++.h>
using namespace std;

#define ll long long int


struct TreeNode {
    int val;
    int sub;
    TreeNode *left,*right;
    TreeNode(int x) : val(x), sub(0),left(NULL), right(NULL) {}
};

void insert(TreeNode *root,int data){
    TreeNode *t = root, *t2;
    while(t){
        t2 = t;
        t = (t->val < data)?(t->right):(t->left);
    }
    if(t2->val < data) t2->right = new TreeNode(data);
    else t2->left = new TreeNode(data);
}

vector<int> pre,Sub;

void preOrder(TreeNode *root){
    if(root){
        pre.push_back(root->val), Sub.push_back(root->sub);
        preOrder(root->left);
        preOrder(root->right);
    }
}

int dfs(TreeNode *root){
    if(root){
        root->sub = 1+dfs(root->left)+dfs(root->right);
        return root->sub;
    }
    return 0;
}
void generateBST(int *ar,int n,int q){
    TreeNode *root = new TreeNode(ar[0]);
    for(int i=1;i<n;i++) insert(root, ar[i]);
    
    pre.clear(), Sub.clear();
    
    dfs(root);
    preOrder(root);
    
    //for(int i=0;i<n;i++) cout<<pre[i]<<" ";cout<<"\n";
    //for(int i=0;i<n;i++) cout<<Sub[i]<<" ";cout<<"\n";
    int k;
    while(q--){
        cin>>k;
        int i=0;
        if(k == pre[0]) cout<<"Empty";
        else{
            while(i < n){
                if(pre[i] != k) cout<<pre[i]<<" ";
                i += (pre[i] == k)?(Sub[i]):(1);
            }
        }
        cout<<"\n";
    }
}
int main(){
    int t; cin>>t;
    while(t--){
        
        
        int n,q; cin>>n>>q;
        int ar[n];
        for(int i=0;i<n;i++) cin>>ar[i];
        generateBST(ar,n,q);
    }
    return 0;
}