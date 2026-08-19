#include <bits/stdc++.h>


using namespace std;


int main(){
	int n,m; cin>>n>>m;


	int tail[n], orde[m], count =0 ;
	for(int i=0;i<n;i++) cin>>tail[i];
	for(int j=0;j<m;j++) cin>>orde[j];

	sort(tail,tail+n, greater<int>());
	sort(orde,orde+n, greater<int>());
	int f = m;
	
	while(f){

		int  i=0,j =0;
		while(i < n && j < m){
			if(orde[j] != -1 ){
				if(tail[i] >= orde[j]){
					orde[j] = -1;
					f--;
					i++,j++;
				}
				else j++;

			}
			else j++;
		}
		count++;
	}
	cout<< count;
	return 0;
}