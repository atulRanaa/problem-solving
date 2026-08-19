MOD = 1000000000 + 7
mod = [0] * 100005
mod[0]=1


for i in xrange(1,100005):
    mod[i] = ( mod[i-1]*2 ) % MOD

t = input()


for _ in xrange(t):
    n = input()
    ar = [ int(x) for x in raw_input().split() ]
    #print ar
    
    
    ans = 0
    for j in xrange(30):
        cnt = 0
        val = (1 << j)
        
        for i in xrange(n):
            if (ar[i] & val):
                cnt += 1
                #print ar[i],
        
        #print 
        
        temp = (mod[ cnt ] - 1) % MOD
        ans += (temp * (val)%MOD)%MOD
    print ans
        