from fractions import Fraction
from decimal import Decimal, getcontext, ROUND_HALF_EVEN

def solve(n, r):
    getcontext().prec = 50
    # O(n log n) Fenwick aggregation, but with Fraction arithmetic
    vals = sorted(set(r))
    m = len(vals)
    cnt = [Fraction(0)] * (m + 1)
    sumRm1 = [Fraction(0)] * (m + 1)
    sumInv = [Fraction(0)] * (m + 1)

    def update(i, rv):
        while i <= m:
            cnt[i] += 1
            sumRm1[i] += rv - 1
            sumInv[i] += Fraction(1, rv)
            i += i & (-i)

    def query(i):
        c = Fraction(0); s1 = Fraction(0); s2 = Fraction(0)
        while i > 0:
            c += cnt[i]; s1 += sumRm1[i]; s2 += sumInv[i]
            i -= i & (-i)
        return c, s1, s2

    total_count = Fraction(0)
    total_inv = Fraction(0)
    ans = Fraction(0)

    import bisect
    for rj in r:
        t = rj + 1
        idx = bisect.bisect_right(vals, t)
        C1, S1, sumInv1 = query(idx)
        C2 = total_count - C1
        S2 = total_inv - sumInv1
        contrib = S1 / (2 * rj) + C2 - Fraction(t, 2) * S2
        ans += contrib

        pos = bisect.bisect_left(vals, rj) + 1
        update(pos, Fraction(rj))
        total_count += 1
        total_inv += Fraction(1, rj)

    d = Decimal(ans.numerator) / Decimal(ans.denominator)
    return d.quantize(Decimal('0.000001'), rounding=ROUND_HALF_EVEN)

n = int(input())
arr = [int(x) for x in input().split()]
print(solve(n, arr))