        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const int N = 1e6+5; 
        #define MOD 1000000007
        #define pii pair<int,int>
        #define pis pair<int,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        ll ar[N];
        ll lvl[50];
        
        struct node{
            ll val;
            node *L,*R;
            node(ll x){val = x; L=R=NULL;}
        };
        node* insert(node* root,ll data){
            if (root == NULL) return (new node(data));
            else{
                if (data <= root->val) root->L = insert(root->L, data);
                else                   root->R = insert(root->R, data);
                return root;
            }
        }
        node* make(ll n){
            node *root = new node(ar[1]);
            for(int i=2;i<=n;i++)
                root = insert(root, ar[i]);
            return root;
        }
        vector<ll> path;
        node* find(node *root,ll data){
            node *cur=root;
            while(cur != NULL){
                path.push_back( cur->val);
                if(cur->val == data)
                    break;
                cur = (data <= cur->val)?(cur->L):(cur->R);
            }
            return cur;
        }
        void preorder(node *root){
            if(root==NULL) return;
            cout<<root->val<<"\n";
            preorder(root->L); preorder(root->R); 
        }
        int height(node *root){
            if(root==NULL) return 0;
            return 1+max(height(root->L), height(root->R));
        }
        node* lca(node* root,ll n1,ll n2){
            node *cur = root;
            while (cur != NULL){
                ll data = cur->val;
                if(data>n1 && data>n2)      cur = cur->L;
                else if(data<n1 && data<n2) cur = cur->R;
                else
                    break;
            }
            return cur;
        }
        int main(){
            ios_base::sync_with_stdio(false);
            cin.tie(NULL);
            
            
            ll n; cin>>n;
            for(int i=1;i<=n;i++) cin>>ar[i];
            ll n1,n2; cin>>n1>>n2;
            node *BST = make(n);
            node *pos = lca(BST,n1,n2);

            node *loc = find(pos,n1); loc = find(pos,n2);
            cout<< *max_element(path.begin(),path.end()) <<"\n";
            return 0;
        }
