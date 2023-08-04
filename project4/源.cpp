#include "SM3.h"
#include<iostream>
# include<windows.h>
using namespace std;

double time1 = 0;
double counts = 0;
LARGE_INTEGER nFreq;
LARGE_INTEGER nBeginTime;
LARGE_INTEGER nEndTime;

void BiToW(unsigned int Bi[], unsigned int W[])
{
	int i;
	unsigned int tmp;
	//同理，添加循环展开提高计算速率
	for (i = 0; i < 16; i += 4)
	{
		W[i] = Bi[i];
		W[i + 1] = Bi[i + 1];
		W[i + 2] = Bi[i + 2];
		W[i + 3] = Bi[i + 3];
	}
	//这里由于语句较为复杂，因此选择两次循环展开
	for (i = 16; i <= 67; i += 2)
	{
		tmp = W[i - 16] ^ W[i - 9] ^ SM3_rotl32(W[i - 3], 15);
		W[i] = SM3_p1(tmp) ^ (SM3_rotl32(W[i - 13], 7)) ^ W[i - 6];
		//把这里的i+1看做新的i即可
		tmp = W[i + 1 - 16] ^ W[i + 1 - 9] ^ SM3_rotl32(W[i + 1 - 3], 15);
		W[i + 1] = SM3_p1(tmp) ^ (SM3_rotl32(W[i + 1 - 13], 7)) ^ W[i + 1 - 6];

	}
}

void WToW1(unsigned int W[], unsigned int W1[])
{
	int i;
	//循环展开，便于提高速率，每次循环计算四次
	for (i = 0; i < 64; i += 4)
	{
		W1[i] = W[i] ^ W[i + 4];
		W1[i + 1] = W[i + 1] ^ W[i + 5];
		W1[i + 2] = W[i + 2] ^ W[i + 6];
		W1[i + 3] = W[i + 3] ^ W[i + 7];
	}
}

void CF(unsigned int W[], unsigned int W1[], unsigned int V[])
{
	unsigned int SS1;
	unsigned int SS2;
	unsigned int TT1;
	unsigned int TT2;
	unsigned int A, B, C, D, E, F, G, H;
	unsigned int T = SM3_T1;
	unsigned int FF;
	unsigned int GG;
	int j;
	//reg init,set ABCDEFGH=V0
	A = V[0];
	B = V[1];
	C = V[2];
	D = V[3];
	E = V[4];
	F = V[5];
	G = V[6];
	H = V[7];
	for (j = 0; j <= 63; j++)
	{
		//SS1
		T = j == 0 ? SM3_T1 : (j == 16 ? SM3_rotl32(SM3_T2, 16) : SM3_rotl32(T, 1));
		SS1 = SM3_rotl32((SM3_rotl32(A, 12) + E + T), 7);
		//SS2
		SS2 = SS1 ^ SM3_rotl32(A, 12);
		//TT1
		FF = j <= 15 ? SM3_ff0(A, B, C) : SM3_ff1(A, B, C);
		TT1 = FF + D + SS2 + *W1;
		W1++;
		//TT2
		GG = j <= 15 ? SM3_gg0(E, F, G) : SM3_gg1(E, F, G);
		TT2 = GG + H + SS1 + *W;
		W++;
		//D
		D = C;
		//C
		C = SM3_rotl32(B, 9);
		//B
		B = A;
		//A
		A = TT1;
		//H
		H = G;
		//G
		G = SM3_rotl32(F, 19);
		//F
		F = E;
		//E
		E = SM3_p0(TT2);
	}
	//update V
	V[0] = A ^ V[0];
	V[1] = B ^ V[1];
	V[2] = C ^ V[2];
	V[3] = D ^ V[3];
	V[4] = E ^ V[4];
	V[5] = F ^ V[5];
	V[6] = G ^ V[6];
	V[7] = H ^ V[7];
}

void BigEndian(unsigned char src[], unsigned int bytelen, unsigned char des[]) {
	unsigned char tmp = 0;
	unsigned int i = 0, loop_time = bytelen / 4;//可以认为这里是偶数
	//这里也可以进行二次循环展开，因为循环中的等式不相互影响
	for (i = 0; i < loop_time; i += 2)
	{
		tmp = des[4 * i];
		des[4 * i] = src[4 * i + 3];
		src[4 * i + 3] = tmp;
		tmp = des[4 * i + 1];
		des[4 * i + 1] = src[4 * i + 2];
		des[4 * i + 2] = tmp;

		tmp = des[4 * (i + 1)];
		des[4 * (i + 1)] = src[4 * (i + 1) + 3];
		src[4 * (i + 1) + 3] = tmp;
		tmp = des[4 * (i + 1) + 1];
		des[4 * (i + 1) + 1] = src[4 * (i + 1) + 2];
		des[4 * (i + 1) + 2] = tmp;
	}
}

void SM3_init(SM3_STATE* md) {
	md->curlen = md->length = 0;
	md->state[0] = SM3_IVA;
	md->state[1] = SM3_IVB;
	md->state[2] = SM3_IVC;
	md->state[3] = SM3_IVD;
	md->state[4] = SM3_IVE;
	md->state[5] = SM3_IVF;
	md->state[6] = SM3_IVG;
	md->state[7] = SM3_IVH;
}

void SM3_compress(SM3_STATE* md) {
	unsigned int W[68];
	unsigned int W1[64];
	//if CPU uses little-endian, BigEndian function is a necessary call
	BigEndian(md->buf, 64, md->buf);
	BiToW((unsigned int*)md->buf, W);
	WToW1(W, W1);
	CF(W, W1, md->state);
}

void SM3_process(SM3_STATE* md, unsigned char* buf, int len) {
	while (len--) {
		/* copy byte */
		md->buf[md->curlen] = *buf++;
		md->curlen++;
		/* is 64 bytes full? */
		if (md->curlen == 64) {
			SM3_compress(md);
			md->length += 512;
			md->curlen = 0;
		}
	}
}

void SM3_done(SM3_STATE* md, unsigned char hash[])
{
	int i;
	unsigned char tmp = 0;
	/* increase the bit length of the message */
	md->length += md->curlen << 3;
	/* append the '1' bit */
	md->buf[md->curlen] = 0x80;
	md->curlen++;
	if (md->curlen > 56) {
		for (; md->curlen < 64;) {
			md->buf[md->curlen] = 0;
			md->curlen++;
		}
		SM3_compress(md);
		md->curlen = 0;
	}
	/* if the length is less than 56 bytes, pad upto 56 bytes of zeroes */
	for (; md->curlen < 56;) {
		md->buf[md->curlen] = 0;
		md->curlen++;
	}
	/* since all messages are under 2^32 bits we mark the top bits zero */
	for (i = 56; i < 60; i++) {
		md->buf[i] = 0;
	}
	/* append length */
	md->buf[63] = md->length & 0xff;
	md->buf[62] = (md->length >> 8) & 0xff;
	md->buf[61] = (md->length >> 16) & 0xff;
	md->buf[60] = (md->length >> 24) & 0xff;
	SM3_compress(md);
	/* copy output */
	memcpy(hash, md->state, SM3_len / 8);
	BigEndian(hash, SM3_len / 8, hash);//if CPU uses little-endian, BigEndian function is a necessary call
}

void SM3_256(unsigned char buf[], int len, unsigned char hash[])
{
	SM3_STATE md;
	SM3_init(&md);
	SM3_process(&md, buf, len);
	SM3_done(&md, hash);
}

int SM3_SelfTest()
{
	unsigned int i = 0, a = 1, b = 1;
	unsigned char Msg1[3] = { 0x61,0x62,0x63 };
	int MsgLen1 = 3;
	unsigned char MsgHash1[32] = { 0 };
	unsigned char StdHash1[32] =
	{ 0x66,0xC7,0xF0,0xF4,0x62,0xEE,0xED,0xD9,0xD1,0xF2,0xD4,0x6B,0xDC,0x10,0xE4,0xE2,

	  0x41,0x67,0xC4,0x87,0x5C,0xF2,0xF7,0xA2,0x29,0x7D,0xA0,0x2B,0x8F,0x4B,0xA8,0xE0
	};
	unsigned char Msg2[64] =
	{ 0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,

	  0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,

	  0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,

	  0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64
	};
	int MsgLen2 = 64;
	unsigned char MsgHash2[32] = { 0 };
	unsigned char StdHash2[32] =
	{ 0xde,0xbe,0x9f,0xf9,0x22,0x75,0xb8,0xa1,0x38,0x60,0x48,0x89,0xc1,0x8e,0x5a,0x4d,

	  0x6f,0xdb,0x70,0xe5,0x38,0x7e,0x57,0x65,0x29,0x3d,0xcb,0xa3,0x9c,0x0c,0x57,0x32
	};
	SM3_256(Msg1, MsgLen1, MsgHash1);
	SM3_256(Msg2, MsgLen2, MsgHash2);
	a = memcmp(MsgHash1, StdHash1, SM3_len / 8);
	b = memcmp(MsgHash2, StdHash2, SM3_len / 8);
	return (a == 0) && (b == 0) ? 0 : 1;
}
int main() {
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);
	cout << "对比结果为：" << SM3_SelfTest() << endl;
	//这里输出0，说明利用该代码加密的密文和标准密文相同，表明这两个消息的加密值均正确。
	QueryPerformanceCounter(&nEndTime);
	time1 = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;
	cout << "运行时间为："<<1000*time1<<"ms";
	return 0;
}
