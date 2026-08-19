from sys import stdin
MOD = 10**9 + 7
 
def fib(n):
	if n == 1:
		return 1
	if n == 2:
		return 1
	q, r = n//2, n%2
	q0 = fib(q)
	q1 = fib(q+1)
	if r == 0:
		return (q0*(2*q1-q0))%MOD
	else:
		return (q1*q1 + q0*q0)%MOD
 
def main():
	readl = stdin.readline
	for i in xrange(int(readl())):
		n = int(readl())
		print("Case %d: %d" % (i+1, fib(n)*fib(n+1)%MOD))
 
if __name__ == "__main__":
	main() 