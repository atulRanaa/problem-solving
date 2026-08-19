"""
Question2. This program was something like executing commands of assembly language in your program. Set of commands given were:
1. ECHO 1: prints the number. Eg. ECHO 1 prints 1
2. Exit: exits the program.
3. SET a 0: assign variable a value 0.
4. ADD 2 3 z: this means z = 2+3 assign sum of first two values to third one.
5. GOTO and LABEL: works as label and Goto defined in c language but label can be before or after goto
6. IF and END: If IF condition is true then statements between IF and END commands get executed otherwise not. Eg. IF a 10 statement1 statement2 END i.e., if a=10.
7. CONTINUE: works as defined in c language.
Variable names will only be alphabetic [a-z] and default values of variables is 0 no need to define or set before use.
Examples:

Input        Output
   SET a 0       1
   LABEL 100     2
   ADD a 1 a     3
   ECHO a        4
   IF a 5        5
   EXIT
   END    
   GOTO 100 
"""

#Python code 
from sys import stdin
newdict = {}
dict={}
i=0
for line in stdin:
    list1=line.split(" ")
    list1[-1] = list1[-1].strip()
    newdict[i]=list1
    i=i+1
key=0
j=0
key_if=[]
label={}
while (key in newdict):
#    print (key)
    if newdict[key][0]=="SET":
        if not newdict[key][2].isalpha():
            dict[newdict[key][1]]= int(newdict[key][2])
        else:
            dict[newdict[key][1]]= int(dict[newdict[key][2]])
        key=key+1
    elif newdict[key][0]=="ADD":
        #print(key)
        if not newdict[key][1].isalpha() and not newdict[key][2].isalpha():
            dict[newdict[key][3]]=int(newdict[key][1])+int(newdict[key][2])
        elif not newdict[key][1].isalpha():
            dict[newdict[key][3]]=int(newdict[key][1])+int(dict[newdict[key][2]])
        elif not newdict[key][2].isalpha():
            dict[newdict[key][3]]=int(dict[newdict[key][1]])+int(newdict[key][2])
        elif newdict[key][1] in dict and newdict[key][2] in dict :
            dict[newdict[key][3]]=dict[newdict[key][1]]+dict[newdict[key][2]]
        key=key+1
    elif newdict[key][0]=="ECHO":
        if newdict[key][1].isalpha():
            print (dict[newdict[key][1]])
        else :
             print (int(newdict[key][1]))
        key=key+1
    elif newdict[key][0]=="EXIT":
        break;
    elif newdict[key][0]=="IF" :
        key_if=key_if+[key+1]
        if newdict[key][1] in dict and newdict[key][2] in dict :
            if(dict[newdict[key][1]]==int(dict[newdict[key][2]])):
                key=key+1
        elif(dict[newdict[key][1]]==int(newdict[key][2])):
             key=key+1;
        elif  dict[newdict[key][1]]!=int(newdict[key][2])  :
                while (newdict[key][0]!="END"):
                    key=key+1
                key=key+1
                key_if.pop()
        elif newdict[key][2].isalpha() and dict[newdict[key][1]]!=dict[newdict[key][2]]:
                while (newdict[key][0]!="END"):
                    key=key+1
                key=key+1
                key_if.pop()
    elif newdict[key][0]=="CONTINUE" :
        key=key_if[-1]
    elif newdict[key][0]=="LABEL" :
        label[newdict[key][1]]=key
        key=key+1
    #    print (key)
    elif newdict[key][0]=="GOTO":
        #print(label[newdict[key][1]])
        key=label[newdict[key][1]]
