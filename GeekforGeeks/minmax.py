
def minmax(val , flag):
	l = len(val)
	if l <= 1:
		return val[0]
	
	new_val = []
	for i in xrange(0,l,2):
		if flag:
			new_val += [ max(val[i],val[i+1])]
		else:
			new_val += [ min(val[i],val[i+1])]


	print new_val
	return minmax(new_val, ~flag)

if __name__ == "__main__":
	val = [ int(x) for x in raw_input().split()]
	l = len(val)
	n_ply = 2**4
	if l <= n_ply:
		val += [ val[-1] ]*(n_ply-l)
		print val
		answer = minmax( val[:n_ply] , False)

		print "Solution is", answer
	else:
		print "Not 4-ply tree"
	