#include <bits/stdc++.h>
using namespace std;

#define N 1000005
#define ll long long


struct node{
    int value;
    node *left, *right;
    node(int x){value=x; left=right=NULL;}
};

node *loc;
void find(int val,node *p) {
    if(p==NULL) return;
    if(p->value==val){
        loc=p;
        return;
    }
    find(val,p->left);
    find(val,p->right);
}
void make(int n, node *root){
    char ch; int a,b;
    int i = (n-1);
    while(i-- > 0){
        loc = NULL;
        scanf("%d %d %c",&a,&b,&ch);
        find(a, root);
        if(ch=='L')
            loc->left = new node(b);
        else
            loc->right = new node(b);
    }
}
vector<int> dia;
void dfs(node *root){
    if(root == NULL) return;
    
    cout<<root->value<<" ";
    dfs(root->left);
    dfs(root->right);
}
int mirror(node *r1,node *r2, int val){
    if(r1==NULL || r2==NULL) return -1;
    if(r1->value==val) return r2->value;
    else if(r2->value==val) return r1->value;
    return max( mirror(r1->left, r2->right,val),mirror(r1->right, r2->left, val) );
}
int main(){
    
    int n,q; scanf("%d %d",&n,&q);
    node *root = new node(1);
    
    make(n, root);
    //dfs(root);
    int num;
    while(q-- > 0){
        scanf("%d",&num);
        cout<< mirror(root,root, num) << "\n";
    }
    return 0;
}