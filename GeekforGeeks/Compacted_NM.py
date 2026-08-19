import json
import time

a_nm = json.loads('[1,1,2,2,1,1,2,2,2,2,3]')
b_nm = json.loads('[3,3,3,3,4,4,4,4,4,4,4]')

start_time = json.loads('["0","1","2","3","4","5","6","7","8","9","10"]')
end_time = json.loads('["1","2","3","4","5","6","7","8","9","10","11"]')

n = 11

prev_a_nm = a_nm[0]
prev_b_nm = b_nm[0]
prev_start_time = start_time[0]
prev_end_time = end_time[0]


for i in range(1,n):
    if a_nm[i] == prev_a_nm and b_nm[i] == prev_b_nm:
        prev_end_time = end_time[i]
    else:
        prev_a_nm = a_nm[i]
        prev_b_nm = b_nm[i]
        prev_start_time = start_time[i]
        prev_end_time = end_time[i]

print(json.dumps({ 'an': prev_a_nm, 'bn': prev_b_nm, 'st': prev_start_time, 'et': prev_end_time}))
