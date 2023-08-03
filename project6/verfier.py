from gmssl import sm3, func
import secrets
import socket

def verify(s,sig):
    for j in range(100):
        s =  sm3.sm3_hash(func.bytes_to_list(str(s).encode()))
    if s == sig:
        return True
    else:
        return False

if __name__ == "__main__":
    # 建立连接
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(('', 12301))
    print("verifer start...")

    data, addr = s.recvfrom(1024)
    ss = data.decode()
    
    s.sendto(data, addr)

    data, addr = s.recvfrom(1024)
    sig = data.decode()
    
    if (verify(ss,sig)==True):
        print("Verified!")
    else:
        print("Denied!")

    s.close()
    print("verifier finished...")
    
        
