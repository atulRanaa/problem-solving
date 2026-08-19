import re
s = "#$This$#is% Matrix#  %!"

list_s = [c for c in s]
l = len(list_s)

i = 0
temp1 = ""
while i < l and not list_s[i].isalnum():
	temp1 += list_s[i]
	i += 1
temp2 = ""
j = l - 1
while j >=0 and not list_s[j].isalnum():
 	temp2 += list_s[j]
 	j -= 1


s2 = ''.join(list_s[i:(j+1)])
s3 = re.sub(r'[!@#$%& ]+'," ",s2)


print temp1+s3+temp2[::-1]