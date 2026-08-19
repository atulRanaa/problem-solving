"""
Question1. Key Stroke: Write a program to simulate a keyboard with given Keys and their operation .Type of keys to simulate were: Alphanumeric space: print it as it is and shift cursor.
@ = toggles caps lock on if off and vice versa, initially it is off.
# = inserts a new line at cursor position and shift cursor.
/ = deletes one character at left and points cursor at that position.
? = works as down arrow if cursor is at last line nothing changes.
^ = works as up arrow if cursor is at first line nothing changes.
"""



# Python code
string=input()
caps=0
result=[]
j=0
i=0
pos=0
while (i<len(string)):
    if string[i]==&#039;@&#039;:
        if caps==1:
            caps=0
        else :
            caps=1
        i=i+1
    elif string[i]==&#039;#&#039;:
    #    print (result)
        result.insert(j,&#039;\n&#039;)
    #    print(result)
        j=j+1
        i=i+1
    elif string[i]==&#039;':
        i=i+1
        j=j+1
    elif string[i]=='/':
        j=j-1
        result=result[:j]+result[j+1:]
        i=i+1
    elif string[i]=='^':
        count=0
        p_j=j
        pos=0
        while (string[i]=='^'):
            count=count+1
            i=i+1
        j=j-1
        count=count+1
        check=1000000
        while(count and j>=0):
            if(result[j]=='\n'):
                if(p_j-j-1=check :
            while(check):
                 
                j=j+1;
                check=check-1
            j=j+1
        else :
            while(result[j]!='\n'):
                j=j+1;
            j=j-1
                 
    elif string[i]=='?':
        count=0
        p_j=j
        while (string[i]=='?'):
            count=count+1
            i=i+1
        j=j-1
         
        if count==1 :
             
            while(count>0):
                 
                if result[j]=='\n':
                    count=count-1
                     
                j=j+1
             
        elif count>1 :
         
            while(check):
                j=j+1;
                check=check-1
            j=j+1
         
    else:
        if (caps==1) and  string[i].islower():
            result.insert( j,string[i].upper())
        else :
            result.insert( j,string[i])
        j=j+1
        i=i+1
print (''.join(result))
