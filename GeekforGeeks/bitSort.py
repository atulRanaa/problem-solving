import operator
def bitset(a):
    count=0
    while(a > 0):
         a = a&(a-1)
         count+=1
    return count

arr = []
for _ in range(int(raw_input())):
     
    for _ in range(int(raw_input())):
       a = input()
       arr[a] = bitset(a)
    sorted_arr = sorted(arr.items() , key=operator.itemgetter(1))
    print sorted_arr
