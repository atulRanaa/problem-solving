n = int(input())


x_axis = []
y_axis = []
for _ in range(n):
    x, y = [int(x) for x in input().split()]

    x_axis.append(x)
    y_axis.append(y)


x_axis.sort()
y_axis.sort()

# print(x_axis)
# print(y_axis)
ans = 0
for i in range(1, n):
    ans += (x_axis[i] - x_axis[i-1]) * (i * (n - i))
    ans += (y_axis[i] - y_axis[i-1]) * (i * (n - i))

print(ans)
