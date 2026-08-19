#include <bits/stdc++.h>
#include <stdio.h>
#include <string.h>

using namespace std;
int findMinInsertionsDP(char str[], int n)
{
    int t[n][n], l, h, gap;
    memset(t, 0, sizeof(t));
    for (gap = 1; gap < n; ++gap){
        for (l = 0, h = gap; h < n; ++l, ++h){
            if(str[l] == str[h]) t[l][h]=t[l+1][h-1];
            else t[l][h] = min(t[l][h-1], t[l+1][h]) + 1;
        }
    }
 
    return t[0][n-1];
}
int main()
{
    char str[5050];
    scanf("%s", str);
    int l = strlen(str);
    printf("%d", findMinInsertionsDP(str,l) );
    return 0;
}
