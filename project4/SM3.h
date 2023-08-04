#pragma once
#pragma once
/************************************************************************
 FileName:
 SM3.h
 Version:
 SM3_V1.1
 Date:
 Sep 18,2016
 Description:
 This headfile provide macro defination, parameter definition
and function declaration needed in SM3 algorithm implement
 Function List:
 1.SM3_256 //calls SM3_init, SM3_process and SM3_done to calculate hash value
 2.SM3_init //init the SM3 state
 3.SM3_process //compress the the first len/64 blocks of the message
 4.SM3_done //compress the rest message and output the hash value
 5.SM3_compress //called by SM3_process and SM3_done, compress a single block of message
 6.BiToW //called by SM3_compress,to calculate W from Bi
 7.WToW1 //called by SM3_compress, calculate W' from W
 8.CF //called by SM3_compress, to calculate CF function.
 9.BigEndian //called by SM3_compress and SM3_done.GM/T 0004-2012 requires to use
big-endian.
 //if CPU uses little-endian, BigEndian function is a necessary call to
change the
 //little-endian format into big-endian format.
 10.SM3_SelfTest //test whether the SM3 calculation is correct by comparing the hash result
with the standard data
 History:
 1. Date: Sep 18,2016
 Author: Mao Yingying, Huo Lili
 Modification: 1)add notes to all the functions
 2)add SM3_SelfTest function
************************************************************************/
#include <string.h>
const unsigned int SM3_len = 256;
const unsigned int SM3_T1 = 0x79CC4519;
const unsigned int SM3_T2 = 0x7A879D8A;
const unsigned int SM3_IVA = 0x7380166f;
const unsigned int SM3_IVB = 0x4914b2b9;
const unsigned int SM3_IVC = 0x172442d7;
const unsigned int SM3_IVD = 0xda8a0600;
const unsigned int SM3_IVE = 0xa96f30bc;
const unsigned int SM3_IVF = 0x163138aa;
const unsigned int SM3_IVG = 0xe38dee4d;
const unsigned int SM3_IVH = 0xb0fb0e4e;
/* Various logical functions */
template<typename T1, typename T2>
inline unsigned int SM3_rotl32(const T1& x, const T2& n) {
	return (((unsigned int)x) << n) | (((unsigned int)x) >> (32 - n));
}
template<typename T1, typename T2>
inline unsigned int SM3_rotr32(const T1& x, const T2& n) {
	return (((unsigned int)x) >> n) | (((unsigned int)x) << (32 - n));
}
template<typename T>
inline unsigned int SM3_p1(const T& x) {
	return x ^ SM3_rotl32(x, 15) ^ SM3_rotl32(x, 23);
}
template<typename T>
inline unsigned int SM3_p0(const T& x) {
	return x ^ SM3_rotl32(x, 9) ^ SM3_rotl32(x, 17);
}
template<typename T1, typename T2, typename T3>
inline unsigned int SM3_ff0(const T1& a, const T2& b, const T3& c) {
	return a ^ b ^ c;
}
template<typename T1, typename T2, typename T3>
inline unsigned int SM3_ff1(const T1& a, const T2& b, const T3& c) {
	return (a & b) | (a & c) | (b & c);
}
template<typename T1, typename T2, typename T3>
inline unsigned int SM3_gg0(const T1& a, const T2& b, const T3& c) {
	return a ^ b ^ c;
}
template<typename T1, typename T2, typename T3>
inline unsigned int SM3_gg1(const T1& a, const T2& b, const T3& c) {
	return (a & b) | ((~a) & c);
}
typedef struct {
	unsigned int state[8];
	unsigned int length;
	unsigned int curlen;
	unsigned char buf[64];
} SM3_STATE;
void BiToW(unsigned int Bi[], unsigned int W[]);
void WToW1(unsigned int W[], unsigned int W1[]);
void CF(unsigned int Wj[], unsigned int Wj1[], unsigned int V[]);
void BigEndian(unsigned char src[], unsigned int bytelen, unsigned char des[]);
void SM3_init(SM3_STATE* md);
void SM3_compress(SM3_STATE* md);
void SM3_process(SM3_STATE* md, unsigned char buf[], int len);
void SM3_done(SM3_STATE* md, unsigned char* hash);
void SM3_256(unsigned char buf[], int len, unsigned char hash[]);
int SM3_SelfTest();