from gmssl import sm3, func
import secrets
import socket
#This is Alice
yr = 2000
def prove(s,birth):
    for j in range(yr- birth):
        s =  sm3.sm3_hash(func.bytes_to_list(str(s).encode()))
    return s

if __name__ == "__main__":
    # 建立连接
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print("connected...")

    data = "Alice"
    s.sendto(data.encode(), ("127.0.0.1", 12300))
    data, addr = s.recvfrom(1024)
    ss = data.decode()
    s.sendto(data, ("127.0.0.1", 12300))

    data, addr = s.recvfrom(1024)
    sig =data.decode()

    print("s=",ss)
    print("sig=",sig)
    s.close()
    print("client finished...")

birth = 1978
if __name__ == "__main__":
    # 建立连接
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print("connected...")
    data = prove(ss,birth)
    s.sendto(str(data).encode(), ("127.0.0.1", 12301))
    data, addr = s.recvfrom(1024)
    data = data.decode()
    data = sig
    s.sendto(str(data).encode(), ("127.0.0.1", 12301))
    
    s.close()
    print("Alice finished...")
