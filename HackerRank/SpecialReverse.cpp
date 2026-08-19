#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <algorithm>

using namespace std;
int main()
{   
    freopen("input.txt","r",stdin);
    freopen("output.txt","w",stdout);
    int t,x;
    scanf("%d",&t);
    for(x=0;x<t;x++){
        char str[100000];
        scanf("%s",&str);
        int i,first = 0, last = strlen(str)-1;
        while(first < last ){
            
            if(isalpha(str[first]) && isalpha(str[last])){
                swap(str[first],str[last]);
                first += 1;
                last -= 1;
            }
            else if(isalpha(str[first])) last -= 1;
            else first += 1;

        }
        printf("%s\n",str);
    }
    return 0;
}