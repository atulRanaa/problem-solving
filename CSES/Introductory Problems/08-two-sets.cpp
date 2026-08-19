#include <vector>
#include <string>
#include <iostream>

using namespace std;
typedef long long ll;
const ll N = 2e5 + 5;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e15
#define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
#define PI 3.14159265
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
const double EPS = 1E-9;
typedef vector< vector<double> > matrix;
typedef vector<int> vi;

template <typename T>
void inspect(const char* name, const T& container) {
    std::cerr << "[INSPECT] " << name << " = [ ";
    bool first = true;
    for (const auto& item : container) {
        if (!first) std::cerr << ", ";
        std::cerr << item;
        first = false;
    }
    std::cerr << " ]\n";
}
#define inspect(x) inspect(#x, x)


int ar[N];
int main() {
    fast;

    ll n; cin >> n;
    ll sum = n * (n + 1) / 2;
    if(sum & 1) {
        cout << "NO\n";
        return 0;
    }
    cout << "YES\n";


    vector<ll> a, b;
    ll sum_a = 0, sum_b = 0;
    for(int i = n; i >= 1; i--) {
        if(sum_a <= sum_b) {
            a.push_back(i);
            sum_a += i;
        } else {
            b.push_back(i);
            sum_b += i;
        }
    }

    cout << a.size() << "\n";
    for(auto x : a) {
        cout << x << " ";
    }
    cout << "\n";
    cout << b.size() << "\n";
    for(auto x : b) {
        cout << x << " ";
    }
    cout << "\n";
    return 0;
}