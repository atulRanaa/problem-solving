#include<bits/stdc++.h>

using namespace std;
vector< string > S;

string ch = "abcdefghijklmnopqrstuvwxyz";
struct Node
{
	unordered_map< char, Node* > child;
	bool isEnd;
	int num;
	int tcount;
	Node() { isEnd = false; num = 0; tcount = 0;}
};
void dfs(Node* node,string s){
	if(node->isEnd)
		cout<<s<<"\n";

	for(char c: ch){
		if(node->child.find(c) != node->child.end()){
			dfs(node->child[c], s+c);
		}
	}
}
void add(Node* node, string s){
	for(char c: s){
		char ch = tolower(c);
		if(node->child.find(ch) == node->child.end())
			node->child[ ch ] = new Node;

		
		node = node->child[ ch ];
	}
	node->isEnd = true; node->tcount++;
}
int find(Node* node, string s){
	bool ans = true; Node* pre;
	for(char ch: s){
		if(node->child.find(ch) == node->child.end()){
			ans=false;
			break;
		}
		node = node->child[ ch];
	}
	
	if(ans){
		dfs(node ,s);
		return 1;
	}
	else{
		cout<<"No suggestions\n";
		return -1;
	}
}
int main(){
	int n,q; cin>>n;

	struct Node *root = new Node;
	string s;
	for(int i=0;i<n;i++){
	    cin>>s;
	    S.push_back(s);
	}
	//sort(S.begin(),S.end());

	for(int i=0;i<n;i++){
		add(root, S[i]);
	}
	cin>>q;
	for(int j=0;j<q;j++){
	    cin>>s;
	    int val =find(root,s);
	    if(val==-1){
	    	add(root, s);
	    }
	}
	return 0;
}
