n,q = map(int ,raw_input().split())
rail =[]
bus = []
for _ in range(q):
    ty,start,end = map(int ,raw_input().split())
    if ty == 1:
        rail += [[start,end]]
        e =[]
        i =1
        if (start in rail[0] or end in rail[0]):
            while i < len(rail):
                if(start in rail[i] or end in rail[i]):
                    rail[0]+=rail[i]
                    e+=[i]
                i+=1
        e.sort(reverse = True)
        for ele in e:
            del(rail[ele])
    else:
        bus += [[start,end]]
        e =[]
        i =1
        if (start in bus[0] or end in bus[0]):
            while i < len(bus):
                if(start in bus[i] or end in bus[i]):
                    bus[0]+=bus[i]
                    e+=[i]
                i+=1
        e.sort(reverse = True)
        for ele in e:
            del(bus[ele])
    if len(rail)==len(bus) and len(set(rail[0]))==len(set(bus[0])):
        print "YES"
    else:
        print "NO"