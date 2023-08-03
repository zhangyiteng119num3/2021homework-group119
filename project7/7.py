from gmssl import sm3, func
import secrets
import socket
def build(s,number):
    digit  =len(number)
    s_ = [0]*digit
    for i in range(digit):
        s_[i] = s[i]
    for i in range(digit):
        for j in range(number[i]):
            s[i] = sm3.sm3_hash(func.bytes_to_list(str(s_[i]).encode()))
    return s_,s

def big_hash(s):
    digit = len(s)
    t = ""
    for i in range(digit):
        t = t + str(s[i])
    t = sm3.sm3_hash(func.bytes_to_list(t.encode()))
    return t
        
def chain(number,Max):
    digit = len(number)
    #s,未进行过hash
    s = []
    for i in range(digit):
        s.append(secrets.randbits(128))
    #initialize
    s,s_initialize = build(s,number)
    numList=[]
    List=[]
    numList.append(number)
    List.append(big_hash(s_initialize))
    number_ = number
    for i in range(digit-1,0,-1):
        number_[i] = Max
        number_[i-1] -= 1
        numList.append(number_)
        s,s_ = build(s,number_)
        List.append(big_hash(s_))
    return s,numList,List

def compare(number_,number):
    digit = len(number)
    i = 0
    while(number_[i] == number[i]):
        i += 1
    if (digit-i-1)<0:
        return 0
    return (digit-i-1)

def prove(number_,s,j,numList,List):
    digit = len(number_)
    for i in range(digit):
        if number_[i]>numList[j][i]:
            print("The number given by the proof is too small!")
            return
    s,s_= build(s,numList[j])
    if List[j]!=big_hash(s_): #应当是sig(List[j]),此程序未写签名函数
        print("Invalid signature!")
        return
    print("valid proof!")
    return


#_______main________
Max = 4
number_=[2,4,3,4]
number =[3,1,4,1]
s,numList,List = chain(number,Max)
j = compare(number_,number)
prove(number_,s,j,numList,List)


        
        
        
    
            
                
