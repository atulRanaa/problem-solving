n,m = map(int, raw_input().split())

mat = []
for _ in range(n):
    s = [c for c in raw_input()]
    mat += [s]

for r in mat:
    print r
