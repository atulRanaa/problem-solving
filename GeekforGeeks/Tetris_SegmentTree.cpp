        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const int N = 1e5+5;
        #define all(x) x.begin(),x.end()
        #define MOD 1000000007
        #define pii pair<int,int>
        #define pis pair<int,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        #define test ll cse;cin>>cse;for(ll I=1;I<=cse;I++)
        #define fast ios_base::sync_with_stdio(0);cin.tie(NULL);cout.tie(NULL);
        #define PI 3.14159265
        #define MAXN 1300005
        #define pb      push_back
        #define mp      make_pair
        #define MAX_ITEM 110001

int segment[2 * MAX_ITEM]; //this is a max segment tree, where range update will update the tree values.
int lazy[MAX_ITEM];
int height;
int getmax(int a, int b)
{
	return a > b ? a : b;
}
 
 
int getheight(int n) //int h = sizeof(int) * 8 - __builtin_clz(n);
{
	int h = 0;
	while (n != 0)
	{
		h++; // increment the bit position
		n = n / 2;// or n = n << 1 shift the whole thing to the right once
	}
	return h;
}
 
void change(int index, int value)
{
	segment[index] = value;
	if (index < MAX_ITEM)
	{
		lazy[index] = value;
	}
}
 
void buildparents(int index)
{
	while (index > 1)
	{
		index = index / 2;
		int temp = getmax(segment[2 * index], segment[2 * index + 1]);
		segment[index] = getmax(lazy[index], temp);
	}
}
 
void buildleaves(int index)
{
	int idx;
	for (int i = height; i > 0; --i)
	{
		idx = index >> i;
		if (lazy[idx] != 0)
		{
			change(2 * idx, lazy[idx]);
			change(2 * idx + 1, lazy[idx]);
			lazy[idx] = 0;
		}
	}
}
 
void update(int l, int r, int value) // range update will update the tree values.
{
	l = l + MAX_ITEM;
	r = r + MAX_ITEM + 1;
	buildleaves(l);
	buildleaves(r - 1);
	int lsave = l;
	int rsave = r;
	while (l < r)
	{
		if (l & 1)
		{
			change(l, value);
			++l;
		}
		if (r & 1)
		{
			change(r - 1, value);
			--r;
		}
		l = l / 2;
		r = r / 2;
	}
	//build seg tree parents
	buildparents(lsave);
	buildparents(rsave - 1);
}
 
int query(int l, int r)
{
	l = l + MAX_ITEM;
	r = r + MAX_ITEM + 1;
	buildleaves(l);
	buildleaves(r - 1);
	int max = -1;
 
	while (l < r)
	{
		if (l & 1)
		{
			if (max < segment[l])
			{
				max = segment[l];
			}
			++l;
		}
		if (r & 1)
		{
			if (max < segment[r - 1])
			{
				max = segment[r - 1];
			}
			--r;
		}
		l = l / 2;
		r = r / 2;
	}
	return max;
}
void init(){
    memset(segment,0, sizeof(segment));
    memset(lazy,0, sizeof(lazy));
}
int main() {
	fast;
	height = getheight(MAX_ITEM);
    


    int n,q;
    test{


        init();
        cin >> n >> q;

        int type,p,k;
        while(q-- > 0){

            cin >> type >> p >> k;
            if(type){
                int mx = query(p, p + k);
                update(p, p + k, mx + k);
            }
            else{
                cout << query(p,k) << "\n";
            }
        }
    }
	return 0;
}