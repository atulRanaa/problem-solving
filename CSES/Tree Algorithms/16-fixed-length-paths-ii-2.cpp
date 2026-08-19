#include <deque>
#include <iostream>
#include <vector>
using namespace std;
using ll = long long;

const int N = 200001;
int n, k1, k2;
vector<int> g[N];
ll answer;

void merge(deque<int> &a, deque<int> &b) {
    if (b.size() > a.size()) swap(a, b);
    auto get = [&](int i) {
        if (i < 0) return a.front();
        if (i >= (int)a.size()) return 0;
        return a[i];
    };
    int bs = b.size();
    b.push_back(0);
    for (int i = 0; i < bs; ++i) {
        ll cur_b = b[i] - b[i + 1];
        answer += cur_b * (get(k1 - i) - get(k2 - i + 1));
    }
    for (int i = 0; i < bs; ++i) {
        a[i] += b[i];
    }
}

deque<int> dfs(int node, int parent) {
    deque<int> ret{1};
    for (int child : g[node]) {
        if (child == parent) continue;
        auto child_depths = dfs(child, node);
        child_depths.push_front(child_depths.front());
        merge(ret, child_depths);
    }
    return ret;
}

int main() {
    cin >> n >> k1 >> k2;

    for (int i = 1; i <= n - 1; ++i) {
        int a, b;
        cin >> a >> b;
        g[a].push_back(b);
        g[b].push_back(a);
    }

    dfs(1, -1);
    cout << answer << '\n';
}