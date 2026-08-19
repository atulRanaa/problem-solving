        #include <bits/stdc++.h>

        using namespace std;

        typedef long long ll;
        const ll MAXN = 3e5 +5;
        const ll MOD = 1000000007;
        #define all(x) x.begin(),x.end()
        #define pii pair<ll,ll>
        #define pis pair<ll,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        #define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
        #define PI 3.14159265
        #define fast ios_base::sync_with_stdio(false);cin.tie(NULL);
        const double EPS = 1E-9;
        typedef vector< vector<double> > matrix;
        typedef vector<int> vi;

const int N = 9;
const int M = 8;

int c[M] = {0};
struct loc {
    int x, y;
    loc() {}
    loc(int x, int y): x(x), y(y) {}
};

int mat[N][M] = {
  {1, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 1, 0},
  {1, 0, 1, 0, 0, 0, 0, 0},
  {1, 0, 1, 0, 0, 0, 1, 0},
  {1, 0, 0, 1, 1, 1, 1, 0},
  {1, 0, 0, 1, 1, 1, 1, 0},
  {1, 0, 0, 0, 0, 0, 1, 0},
  {1, 1, 1, 0, 0, 0, 1, 0},
  {1, 1, 1, 1, 1, 1, 1, 0}
};

void update_cache(int x) {
    for(int y = 0; y < M; y++) {
        if(mat[x][y] != 0)
            c[y] = c[y] + 1;
        else
            c[y] = 0;
    }


    for(int y = 0; y < M; y++) cout << c[y] << " "; cout << "\n";
}

int area(loc ll, loc ur) {
    if (ll.x > ur.x || ll.y > ur.y)
        return 0;
    else
        return (ur.x - ll.x + 1) * (ur.y - ll.y + 1);
}

stack< loc > Q;

int main() {

    fast;

    loc best_ll = loc(0, 0);
    loc best_ur = loc(-1, -1);

    for(int x = N - 1; x >= 0; x--) {
        update_cache(x);
        int width = 0;

        for(int y = 0; y < M; y++) {
            if(c[y] > width) {

                Q.push( loc(y, width) );
                width = c[y];

            }

            if(c[y] < width) {

                int y0, w0;
                do {
                    loc coordinates = Q.top();
                    Q.pop();

                    y0 = coordinates.x;
                    w0 = coordinates.y;
                    cout << "width: " << width << "\n";
                    cout << "Area New: " << width * (y - y0) << " " << "Old" <<  area(best_ll, best_ur) << "\n";
                    if( width * (y - y0) > area(best_ll, best_ur)) {
                        best_ll = loc(x, y0);
                        best_ur = loc(x + width - 1, y - 1);


                        cout << best_ll.x << " " << best_ll.y << "\n";
                        cout << best_ur.x << " " << best_ur.y << "\n";
                        cout << "\n";
                    }
                    width = w0;
                } while(c[y] >= width && !Q.empty());

                width = c[y];
                if(width != 0)
                    Q.push( loc(y0, width) );
            }
        }

    }



    return 0;
}