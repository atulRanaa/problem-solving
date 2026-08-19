#include <bits/stdc++.h>
using namespace std;

int main(int argc, char *a[])
{
    int t; cin >> t;
    int ar[26] = {0};
    ar['a'-'a'] = 1;
    ar['e'-'a'] = 1;
    ar['i'-'a'] = 1;
    ar['o'-'a'] = 1;
    ar['u'-'a'] = 1;
    while(t-- > 0) {
        string s; cin >> s;
        int vo = 0, con = 0;
        for(char ch: s) {
            if(ar[ch-'a']) vo++; else con++;
        }

        cout << vo << " " << con << " " << vo*con << "\n";
    }

}
