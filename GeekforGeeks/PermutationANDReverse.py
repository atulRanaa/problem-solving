n = input()
p = map(int ,raw_input().split())
a = map(int ,raw_input().split())
a = a*(100010/n)
t = p[:]
ans =[]
ans1=[]
j = n-1
for i in range(n,0,-1):
    ind = p.index(i)
    p[ind:(j+1)] = reversed(p[ind:(j+1)])
    if j!=ind: ans+=[[ind+1,j+1]]
    j-=1
 
j = 0
for i in range(n,0,-1):
    ind = t.index(i)
    t[j:(ind+1)] = reversed(t[j:(ind+1)])
    if j!=ind: ans1+=[[j+1,ind+1]]
    j+=1
ans1+=[[1,n]]

len1 =[ i[1]-i[0]+1 for i in ans ]
len2 =[ i[1]-i[0]+1 for i in ans1]
ind1 =0
ind2 =0

for num in len1:
    while a[ind1]!=num:
        ind1+=1
for num in len2:
    while a[ind2]!=num:
        ind2+=1

if ind1 <= ind2:
    print len(ans)
    for i in ans:
        print i[0],i[1]
else:
    print len(ans1)
    for i in ans1:
        print i[0],i[1]
