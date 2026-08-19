#include <bits/stdc++.h>

using namespace std;
//long long ar[2*1000000 +10];
#define F first
#define S second

bool comp1(pair<int,string> a, pair<int,string> b){
	return a.F > b.F;
}
bool comp2(pair<int,string> a, pair<int,string> b){
	return (a.F < b.F)||(a.F == b.F && a.S < b.S);
}
int main() {
    
    
	/*long long num,k; scanf("%lld %lld", &num,&k);
	
	for(int i=0;i<num;i++) scanf("%lld",&ar[i]);
	
	long long l = min(num, k+1), ans;
	
	if(num == 1 && k&1) ans = -1;
	else if(num == 1 && !(k&1)) ans = ar[0];
	else if(k == 1) ans = ar[1];
	else if(k == 2 && num == 2) ans = ar[0];
	else ans = *max_element(ar,ar+l);
	cout<< ans <<"\n";*/

	int n; cin>>n;
	map<string, pair<int,int> > m;
	string s;
	int t;
	for(int i=0;i<n;i++){
		cin>>s>>t;
		if(m.find(s) != m.end()){
			m[ s].F += 1;
			m[ s].S += t;
		}
		else m[s] = {1,t};
	}

	pair<int , string> byScore[n];
	int j = 0;
	for(auto ele: m){
		byScore[j++] = { ele.S.F, ele.F};
	}
	cout<<"part1\n";
	sort(byScore,byScore+j,comp1);
	
	int  i =0,counter = 1; 
	while( i < j){
		vector< pair<int , string> > byTime;
		int check = byScore[i].F;
		while(byScore[i].F == check){
			byTime.push_back({ m[ byScore[i].S ].S , byScore[i].S});
			i++;
		}
		sort(byTime.begin(),byTime.end(), comp2);

		for(auto ele: byTime) cout<<counter++<<" "<<ele.S<<"\n";
	}
}

