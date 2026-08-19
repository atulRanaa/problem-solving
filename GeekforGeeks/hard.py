import re

def solve(q,l):
  try:
    n = next(i for i in q if i.isalpha())
  except StopIteration:
    return q if eval(re.sub(r'(^|[^0-9])0+([1-9]+)', r'\1\2', q)) else False
  else: 
    for i in (str(i) for i in range(10) if str(i) not in q):
        print(n,i)
        r = solve(q.replace(n,str(i)),l)
        if r:
            return r
    return False


def solveAlphametic(q):
    q,p = q.strip().split('=')
    a,b = q.split('+')
    q = q+"=="+p
    l = [ a[-1],b[-1],p[-1]]
    print(a,b,p,l)
    ans = solve(q,l)
    if ans == False: return "impossible"
    else: return ans.replace("==","=")
    
if __name__ == "__main__":
  query = "AS+A==MOM "
  r = solveAlphametic(query)
  print(r if r else "No solution found.")