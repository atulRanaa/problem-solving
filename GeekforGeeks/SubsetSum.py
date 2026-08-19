def subset(xs,n):
	a, b = 0, 0
	for i in range(n):
	    while a != (n+1) and sum(xs[i:a]) < lo:
	        a += 1
	    while b != (n+1) and sum(xs[i:b]) <= hi:
	        b += 1
	    for j in range(a, b):
	        print(xs[i:j])
subset([1,2,3,4],4)