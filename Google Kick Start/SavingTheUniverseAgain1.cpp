#include<stdio.h>
#include<algorithm>
#include<iostream>
#include<string.h>
#include<set>
#include<map>

using namespace std;
#define X first
#define Y second
typedef long long ll;
const ll MAX = 1e5+5;
const ll MOD = 1e9+7;

bool ifPossible(ll d,char str[50]){
	ll count = 0;
	int l = strlen(str);
	for(int i=0;i < l;i++){
		if(str[i] == 'S') count++;
	}
	if(count > d) return false;
	else return true;
}
ll damage(char str[50],int l){
	ll sum = 0, add = 1;
	for(int i=0; i < l;i++){
		if(str[i] == 'C') add *= 2;
		else sum += add;
	}
	return sum;
}
int main(){

	int t; scanf("%d",&t);

	for(int c=1; c <= t; c++){
		ll d;
		char str[50];
		scanf("%lld %s",&d,str);

		if(ifPossible(d,str)){
				int l = strlen(str);
				int moves = 0;
				ll curr = damage(str,l);

				if(curr <= d) printf("Case #%d: %d\n",c,moves);
				else{
					for(int i = l-2; i >= 0;i--){
						int j = i;
						while(str[j] == 'C' && str[j+1] == 'S' && curr > d){
							str[j] = 'S';
							str[j+1] = 'C';
							moves += 1;
							curr = damage(str, l);
							//printf("%s\n",str);
							j++;
							if(j == l-1) break;
						}
						if(curr <= d) break;
					}
					printf("Case #%d: %d\n",c,moves);
				}
		}
		else printf("Case #%d: IMPOSSIBLE\n",c);
	}
	return 0;
}