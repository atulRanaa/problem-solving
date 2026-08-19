import math
nm = 0
a = 0
b = 1
for i in range (16):
    next = b + 1
    print("Next Target: " + str(next))
    diff = abs(math.ceil((nm-next)/2))
    a = next - diff
    print("Next Move: (%d -> %d)"%(a, next))
    b = next