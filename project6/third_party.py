from gmssl import sm3, func
import secrets
import socket
def third_party(birth):
    seed = secrets.randbits(128)
    s = sm3.sm3_hash(func.bytes_to_list(str(seed).encode()))
    yr = 2000
    i = yr - birth + 100
    sig = s
    for j in range(i):
        sig =  sm3.sm3_hash(func.bytes_to_list(str(sig).encode()))
    return s, sig

dic={"Alice":1978,"Bob":2009,"Eve":1994}


if __name__ == "__main__":
    # 建立连接
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(('', 12300))
    print("third party start...")

    data, addr = s.recvfrom(1024)
    data = data.decode()
    birth = dic.get(data,None)
    ss,sig = third_party(birth)

    data = ss
    s.sendto(data.encode(), addr)
    data, addr = s.recvfrom(1024)
    if (data.decode() == ss):
        data = sig
        s.sendto(data.encode(), addr)

    s.close()
    print("third party finished...")
    
        

