#include<bits/stdc++.h>

using namespace std;

struct Node
{
	map< char, Node* > child;
	bool isEnd;
	int num,tcount;
	Node() { isEnd = false; num = 0; tcount = 0;}
};

void add(Node* node, string s){
	for(char ch: s){
		if(node->child.find(ch) == node->child.end())
			node->child[ ch ] = new Node;

		node->num++;
		node = node->child[ ch ];
	}
	node->isEnd = true; node->tcount++;
}
int find(Node* node, string s){
	bool ans = true; int pre = 0;
	for(char ch: s){
		if(node->child.find(ch) == node->child.end()){
			ans=false;
			break;
		}
		node = node->child[ ch];
	}
	return ans?(node->num+(node->isEnd?(node->tcount):0)):0;
}
int main(){
	int n,q; cin>>n>>q;

	struct Node *root = new Node;
	string a,b;
	while(n--){
		cin>>a; add(root, a);
	}
	while(q--){
	    cin>>b; cout<<find(root, b)<<"\n";
	}
	return 0;
}
