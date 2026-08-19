    #include <bits/stdc++.h>
    using namespace std;
    class WaveletTree
    {
    public:
        WaveletTree(std::vector<int> &v, int size) : s(size), nodes(2 * size)
        {
            build(v.begin(), v.end(), 0, s - 1, 1);
        }
     
        // l, r is 0 based
        // k is 1 based
        // range [l, r)
        int kTh(int l, int r, int k)
        {
            int idx = 1;
            int L = 0, U = s - 1;
            while (L != U) {
                int M = (L + U) / 2;
                int ll = nodes[idx][l];
                int lr = nodes[idx][r];
                if (k <= lr - ll) {
                    l = ll, r = lr, U = M, idx = 2 * idx;
                } else {
                    k -= lr - ll, l -= ll, r -= lr, L = M + 1, idx = 2 * idx + 1;
                }
            }
            return L;
        }
     
    private:
        int s;
        std::vector<std::vector<int>> nodes;
     
        using iter = std::vector<int>::iterator;
     
        void build(iter b, iter e, int L, int U, int idx)
        {
            if (L == U)
                return;
            int M = (L + U) / 2;
     
            nodes[idx].resize(distance(b, e) + 1);
            nodes[idx][0] = 0;
            int i = 0;
            for (iter it = b; it != e; ++it) {
                nodes[idx][i + 1] = nodes[idx][i] + (*it <= M);
                i++;
            }
     
            iter p = std::stable_partition(b, e, [M](int i) { return i <= M; });
     
            build(b, p, L, M, idx * 2);
            build(p, e, M + 1, U, idx * 2 + 1);
        }
    };
     
    int main(){
        int n,m; scanf("%d %d",&n,&m);
     
        vector<int> ar(n,0);
        for(int i=0;i<n;i++)    scanf("%d",&ar[i]);
        WaveletTree T(ar, n);
        
        int l,r,k;
        while(m-- > 0){
            scanf("%d %d %d",&l,&r,&k);
            l--;
     
            printf("%d\n", T.kTh(l,r,k));
        }
    } 