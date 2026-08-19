import re
def lcs(X, Y, m, n):
    L = [[0 for x in xrange(n+1)] for x in xrange(m+1)]

    # Following steps build L[m+1][n+1] in bottom up fashion. Note
    # that L[i][j] contains length of LCS of X[0..i-1] and Y[0..j-1] 
    for i in xrange(m+1):
        for j in xrange(n+1):
            if i == 0 or j == 0:
                L[i][j] = 0
            elif X[i-1] == Y[j-1]:
                L[i][j] = L[i-1][j-1] + 1
            else:
                L[i][j] = max(L[i-1][j], L[i][j-1])

    # Following code is used to print LCS(Backtracking)
    index = L[m][n]

    lcs = [""] * (index+1)
    lcs[index] = "\0"
    i = m
    j = n
    while i > 0 and j > 0:
    
        if X[i-1] == Y[j-1]:
            lcs[index-1] = X[i-1]
            i-=1
            j-=1
            index-=1
        elif L[i-1][j] > L[i][j-1]: i-=1
        else: j-=1
        
    return lcs

# Driver program
X = raw_input()
Y = raw_input()
v = raw_input()
m = len(X)
n = len(Y)
LCS = ''.join(lcs(X, Y, m, n)).replace(v,'')
if len(LCS) == 1:
	print "0"
else:
	print LCS[0:(len(LCS)-1)]
