#!/bin/python3

import os
import sys

order = {}
maxRange = 2**63 -1

def valid_N(q):

    if( len(q) != 8):
        return False

    try:
        if not (int(q[1])>= 1 and int(q[1])<=maxRange):
            return False
    except ValueError:
        return False
    
    if not q[2].isdigit():
        return False
    if not q[3].isalpha():
        return False
    
    if q[4] not in ['M','L','I']: 
        return False
    if q[5] not in ['B','S']:
        return False
    try:
        float(q[6])
        if q[4]=='M' and q[6]!='0.00':
            return False
    except ValueError:
        return False

    try:
        if not (int(q[7])>= 1 and int(q[7])<=maxRange):
            return False
    except ValueError:
        return False

    if q[1] in order:
        return False

    return True
def New(q):
    if valid_N(q):
        order[q[1]] = q[2:]
        return q[1]+" - Accept"
    else:
        return q[1]+" - Reject - 303 - Invalid order details"


def processQueries(queries, n):
    ans_q = []
    for i in range(n):

        q = queries[i].split(',')
        if q[0]=='N':
            ans_q += [New(q)]
        if q[0]=='X':
            ans_q += [q[1]+" - CancelAccept"]
        if q[0]=='A':
            ans_q += [q[1]+" - AmendAccept"]
    return ans_q

if __name__ == '__main__':
    sys.stdin=open('input.txt','r')
    sys.stdout=open('output.txt','w')

    queries_size = int(raw_input())
    queries = []
    for _ in range(queries_size):
        queries_item = raw_input()
        queries.append(queries_item)

    response = processQueries(queries,queries_size)
    print("\n".join(response))