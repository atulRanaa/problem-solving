#include <iostream>
using namespace std;

#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;

template <typename T>
using indexed_set =
    tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;

int main() {
    int n, k;
    cin >> n >> k;

    indexed_set<int> s;
    for (int i = 1; i <= n; i++) {
        s.insert(i);
    }

    int pos = 0;
    for (int i = 1; i <= n; i++) {
        pos = (pos + k) % (n - i + 1);
        auto it = s.find_by_order(pos);
        cout << *it << " ";
        s.erase(it);
    }
    cout << "\n";
}