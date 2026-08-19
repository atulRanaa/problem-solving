s = raw_input()
while '/' in s:
	i = s.index('/')
	if (i-1)>=0:
		s = s[0:(i-1)]+s[(i+1):]
	else: s = s[0:i]+s[(i+1):]
on = 0
op = ['@','<','>','?','^','#']
i,j = 0,0 
curi = 0
ans = [[]]
for char in s:
	if char == '@': on=(on+1)%2
	if char == '<':
		j -= 1
		if j<0 : j=0
	if char == '>':
		j += 1
		if j >= len(ans[curi]):
			j = len(ans[curi])
	if char == '?':
		curi += 1
		if curi >= len(ans):
			curi = len(ans)-1
		j = len(ans[curi])
	if char == '^':
		curi -= 1
		if curi < 0:
			curi = 0
		if len(ans[curi]) < j:
			j = len(ans[curi])
	if char == '#':
		ans += [[]]
		curi +=1
		if curi >= len(ans):
			curi = len(ans)-1
		j = 0
	if char not in op:
		if on%2 == 1:
			#ans[curi] += char.upper()
			ans[curi] = ans[curi][0:j]+[char.upper()]+ans[curi][j:]
		else:
			ans[curi] = ans[curi][0:j]+[char]+ans[curi][j:]
		j+=1
	#print char,curi,j
for a in ans:
	print ''.join(a)
	