#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
 
using namespace std;
using ll = long long;
 
template <class T>
struct fenwick_tree {
    using U = typename std::make_unsigned<T>::type;
 
  public:
    fenwick_tree() : _n(0) {}
    explicit fenwick_tree(int n) : _n(n), data(n) {}
 
    void add(int p, T x) {
        p++;
        while (p <= _n) {
            data[p - 1] += U(x);
            p += p & -p;
        }
    }
 
    // sum of [0, r)
    T sum(int r) {
        U s = 0;
        while (r > 0) {
            s += data[r - 1];
            r -= r & -r;
        }
        return T(s);
    }
 
    // sum of [l, r)
    T sum(int l, int r) {
        return sum(r) - sum(l);
    }
    
  private:
    int _n;
    std::vector<U> data;
};
 
constexpr ll HOR_BEGIN = 0, HOR_END = 2, VER = 1;
 
struct event{
    ll type, x, y1, y2, time;
    bool operator<(const event &other) const{
        if(x != other.x) return x < other.x;
        return type < other.type;
    }
};
 
struct segment{
    ll x1, y1, x2, y2, len;
    bool hor() const{
        return y1 == y2;
    }
    ll intersect(const segment &other) const{
        if(max(y1, y2) < min(other.y1, other.y2) || max(other.y1, other.y2) < min(y1, y2)) return len;
        if(max(x1, x2) < min(other.x1, other.x2) || max(other.x1, other.x2) < min(x1, x2)) return len;
        if(hor()) return abs(other.x1 - x1);
        return abs(other.y1 - y1);
    }
};
 
bool intersections(vector<event> &events, int m, int last){
    fenwick_tree<ll> tree(m);
    for(event& e : events){
        int limit = (e.time < 0 || e.time == last ? 1 : 2);
        if(e.type == HOR_BEGIN){
            tree.add(e.y1, 1);
        }else if(e.type == HOR_END){
            tree.add(e.y1, -1);
        }else if(e.type == VER){
            if(tree.sum(e.y1, e.y2 + 1) > limit) return true;
        }
    }
    return false;
}
 
char reverse(char dir){
    if(dir == 'D') return 'U';
    if(dir == 'U') return 'D';
    if(dir == 'R') return 'L';
    if(dir == 'L') return 'R';
    return 'S';
}
 
int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
 
    int n;
    char dir, lastdir = 'S';
    ll x = 0, y = 0, dist = 0;
    
    cin >> n;
    map<ll, int> compress;
    compress[0] = 0;
    vector<event> events;
    vector<segment> segments;
    vector<ll> distance = {0};
    int j = 0;
 
    auto process_segment = [&]() {
        distance.push_back(distance.back() + dist);
        if(lastdir == 'U'){
            events.push_back({VER, x, y, y + dist, j});
            segments.push_back({x, y, x, y + dist, dist});
            y += dist;
            compress[y] = 0;
        }else if(lastdir == 'D'){
            events.push_back({VER, x, y - dist, y, j});
            segments.push_back({x, y, x, y - dist, dist});
            y -= dist;
            compress[y] = 0;
        }else if(lastdir == 'R'){
            events.push_back({HOR_BEGIN, x, y, y, j});
            events.push_back({HOR_END, x + dist, y, y, j});
            segments.push_back({x, y, x + dist, y, dist});
            x += dist;
        }else if(lastdir == 'L'){
            events.push_back({HOR_BEGIN, x - dist, y, y, j});
            events.push_back({HOR_END, x, y, y, j});
            segments.push_back({x, y, x - dist, y, dist});
            x -= dist;
        }
        dist = 0;
        j += 1;
    };
 
    for(int i = 0; i < n; i++){
        int curdist;
        cin >> dir >> curdist;
        if(lastdir == 'S'){
            if(dir == 'U' || dir == 'D'){
                events.push_back({HOR_BEGIN, 0, 0, 0, -1});
                events.push_back({HOR_END, 0, 0, 0, -1});
            }else{
                events.push_back({VER, 0, 0, 0, -1});
            }
        } else 
        if(dir != lastdir){
            process_segment();
        }
        if(reverse(dir) == lastdir){
            break;
        }
        dist += curdist;
        lastdir = dir;
    }
    if(dist > 0){
        process_segment();
    }
    n = j;
 
    int sz = 1;
    for(auto& p: compress) {
        p.second = sz++;
    }
 
    for(event& e : events){
        e.y1 = compress[e.y1];
        e.y2 = compress[e.y2];
    }
 
    sort(events.begin(), events.end());
    int a = 1, b = n + 1;
    while(b - a > 1){
        int mid = (a + b) / 2;
        vector<event> subevents;
        for(event& e : events){
            if(e.time < mid) subevents.push_back(e);
        }
        if(intersections(subevents, compress.size() + 5, mid - 1)) b = mid;
        else a = mid;
    }
    if(b == n + 1){
        cout << distance[n] << "\n";
    }else{
        ll ans = segments[a].len;
        for(int i = 0; i < a - 1; i++){
            ans = min(ans, segments[a].intersect(segments[i]));
        }
        ans += distance[a];
        cout << ans << "\n";
    }

    return 0;
}