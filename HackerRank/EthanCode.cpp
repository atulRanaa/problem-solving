#include "bits/stdc++.h"
using namespace std;


typedef long long int ll;
bool contains(string a,string b){
	int i=0,j=0;
	int A = a.length(), B = b.length();
	cout<<A<<" "<<B<<endl;
	while(i <=A && j <=B ){
		
		//cout<<i<<" "<<j<<endl;

		if(i >= A) return true;
		else if(j >= B) return false;
		else if(a[i] == b[j]) i++,j++;
		else if(i==0) j++;
		else i = 0;
	}
}
vector<int> prefixtable(string Z) {

    int n = (int) Z.length();
    vector<int> F (n);
    F[0]=0;
    for (int i=1; i<n; ++i) {
        int j = F[i-1];
        while (j > 0 && Z[i] != Z[j])
            j = F[j-1];

        if (Z[i] == Z[j])  ++j;
        F[i] = j;
    }
    return F;
}
int main(){

	int n; cin>>n;
	while(n--){
		string a; cin>>a;
		string b; cin>>b;

		vector<int> F = prefixtable(a);
		for(int ele: F) cout<<ele<<" ";
		cout<<":"<<contains(a,b)<<endl;
	}
}