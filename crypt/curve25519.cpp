#include "curve25519.hpp"

namespace gvl
{

namespace
{

void core(uint8_t const* Px, uint8_t* s, uint8_t const* k, uint8_t const* Gx);
int mula_small(uint8_t* p, uint8_t* q, int m, uint8_t* x, int n, int z);
int mula32(uint8_t* p, uint8_t* x, uint8_t* y, int t, int z);
void divmod(uint8_t* q, uint8_t* r, int n, uint8_t const* d, int t);
void x_to_y2(int10& t, int10& y2, int10& x);
void sqrt(int10& x, int10& u);
int is_negative(int10 const& x);

const int KEY_SIZE = 32;

/* 0 */
uint8_t const ZERO[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* the prime 2^255-19 */
uint8_t const PRIME[] =
{
    237, 255, 255, 255,
    255, 255, 255, 255,
    255, 255, 255, 255,
    255, 255, 255, 255,
    255, 255, 255, 255,
    255, 255, 255, 255,
    255, 255, 255, 255,
    255, 255, 255, 127
};

/* group order (a prime near 2^252+2^124) */
uint8_t const ORDER[] =
{
    237, 211, 245, 92,
    26,  99,  18,  88,
    214, 156, 247, 162,
    222, 249, 222, 20,
    0,   0,   0,   0, 
    0,   0,   0,   0,
    0,   0,   0,   0, 
    0,   0,   0,   16
};

/* smallest multiple of the order that's >= 2^255 */
uint8_t const ORDER_TIMES_8[] = {
    104, 159, 174, 231,
    210, 24,  147, 192,
    178, 230, 188, 23,
    245, 206, 247, 166,
    0,   0,   0,   0,  
    0,   0,   0,   0,
    0,   0,   0,   0,  
    0,   0,   0,   128
};

/* constants 2Gy and 1/(2Gy) */
int10 BASE_2Y = {{
    39999547, 18689728, 59995525, 1648697, 57546132,
    24010086, 19059592, 5425144, 63499247, 16420658
}};

int10 BASE_R2Y = {{
    5744, 8160848, 4790893, 13779497, 35730846,
    12541209, 49101323, 30047407, 40071253, 6226132
}};


int64_t const P25 = 33554431;	/* (1 << 25) - 1 */
int64_t const P26 = 67108863;	/* (1 << 26) - 1 */


void unpack(int10& x, uint8_t const* m)
{
    x[0] = ((m[0] & 0xFF))         | ((m[1] & 0xFF))<<8 |
	        (m[2] & 0xFF)<<16      | ((m[3] & 0xFF)& 3)<<24;
    x[1] = ((m[3] & 0xFF)&~ 3)>>2  | (m[4] & 0xFF)<<6 |
	        (m[5] & 0xFF)<<14 | ((m[6] & 0xFF)& 7)<<22;
    x[2] = ((m[6] & 0xFF)&~ 7)>>3  | (m[7] & 0xFF)<<5 |
	        (m[8] & 0xFF)<<13 | ((m[9] & 0xFF)&31)<<21;
    x[3] = ((m[9] & 0xFF)&~31)>>5  | (m[10] & 0xFF)<<3 |
	        (m[11] & 0xFF)<<11 | ((m[12] & 0xFF)&63)<<19;
    x[4] = ((m[12] & 0xFF)&~63)>>6 | (m[13] & 0xFF)<<2 |
	        (m[14] & 0xFF)<<10 |  (m[15] & 0xFF)    <<18;
    x[5] =  (m[16] & 0xFF)         | (m[17] & 0xFF)<<8 |
	        (m[18] & 0xFF)<<16 | ((m[19] & 0xFF)& 1)<<24;
    x[6] = ((m[19] & 0xFF)&~ 1)>>1 | (m[20] & 0xFF)<<7 |
	        (m[21] & 0xFF)<<15 | ((m[22] & 0xFF)& 7)<<23;
    x[7] = ((m[22] & 0xFF)&~ 7)>>3 | (m[23] & 0xFF)<<5 |
	        (m[24] & 0xFF)<<13 | ((m[25] & 0xFF)&15)<<21;
    x[8] = ((m[25] & 0xFF)&~15)>>4 | (m[26] & 0xFF)<<4 |
	        (m[27] & 0xFF)<<12 | ((m[28] & 0xFF)&63)<<20;
    x[9] = ((m[28] & 0xFF)&~63)>>6 | (m[29] & 0xFF)<<2 |
	        (m[30] & 0xFF)<<10 |  (m[31] & 0xFF)    <<18;
}

/* Check if reduced-form input >= 2^255-19 */
bool is_overflow(int10 const& x) {
    return (
        ((x[0] > P26-19)) &&
        ((x[1] & x[3] & x[5] & x[7] & x[9]) == P25) &&
        ((x[2] & x[4] & x[6] & x[8]) == P26)
        ) || (x[9] > P25);
}

/* Convert from internal format to little-endian uint8_t format.  The 
 * number must be in a reduced form which is output by the following ops:
 *     unpack, mul, sqr
 *     set --  if input in range 0 .. P25
 * If you're unsure if the number is reduced, first multiply it by 1.  */
void pack(int10& x, uint8_t* m)
{
    int ld = 0, ud = 0;
    int64_t t;
    ld = (is_overflow(x)?1:0) - ((x[9] < 0)?1:0);
    ud = ld * -(P25+1);
    ld *= 19;
    t = ld + x[0] + (x[1] << 26);
    m[ 0] = (uint8_t)t;
    m[ 1] = (uint8_t)(t >> 8);
    m[ 2] = (uint8_t)(t >> 16);
    m[ 3] = (uint8_t)(t >> 24);
    t = (t >> 32) + (x[2] << 19);
    m[ 4] = (uint8_t)t;
    m[ 5] = (uint8_t)(t >> 8);
    m[ 6] = (uint8_t)(t >> 16);
    m[ 7] = (uint8_t)(t >> 24);
    t = (t >> 32) + (x[3] << 13);
    m[ 8] = (uint8_t)t;
    m[ 9] = (uint8_t)(t >> 8);
    m[10] = (uint8_t)(t >> 16);
    m[11] = (uint8_t)(t >> 24);
    t = (t >> 32) + (x[4] <<  6);
    m[12] = (uint8_t)t;
    m[13] = (uint8_t)(t >> 8);
    m[14] = (uint8_t)(t >> 16);
    m[15] = (uint8_t)(t >> 24);
    t = (t >> 32) + x[5] + (x[6] << 25);
    m[16] = (uint8_t)t;
    m[17] = (uint8_t)(t >> 8);
    m[18] = (uint8_t)(t >> 16);
    m[19] = (uint8_t)(t >> 24);
    t = (t >> 32) + (x[7] << 19);
    m[20] = (uint8_t)t;
    m[21] = (uint8_t)(t >> 8);
    m[22] = (uint8_t)(t >> 16);
    m[23] = (uint8_t)(t >> 24);
    t = (t >> 32) + (x[8] << 12);
    m[24] = (uint8_t)t;
    m[25] = (uint8_t)(t >> 8);
    m[26] = (uint8_t)(t >> 16);
    m[27] = (uint8_t)(t >> 24);
    t = (t >> 32) + ((x[9] + ud) << 6);
    m[28] = (uint8_t)t;
    m[29] = (uint8_t)(t >> 8);
    m[30] = (uint8_t)(t >> 16);
    m[31] = (uint8_t)(t >> 24);
}




void set(int10& o, int i)
{
    o[0]=i;	o[1]=0;
    o[2]=0;	o[3]=0;
    o[4]=0;	o[5]=0;
    o[6]=0;	o[7]=0;
    o[8]=0;	o[9]=0;
}

void add(int10& xy, int10& x, int10& y)
{
    xy[0] = x[0] + y[0];	xy[1] = x[1] + y[1];
    xy[2] = x[2] + y[2];	xy[3] = x[3] + y[3];
    xy[4] = x[4] + y[4];	xy[5] = x[5] + y[5];
    xy[6] = x[6] + y[6];	xy[7] = x[7] + y[7];
    xy[8] = x[8] + y[8];	xy[9] = x[9] + y[9];
}

void sub(int10& xy, int10& x, int10& y)
{
    xy[0] = x[0] - y[0];	xy[1] = x[1] - y[1];
    xy[2] = x[2] - y[2];	xy[3] = x[3] - y[3];
    xy[4] = x[4] - y[4];	xy[5] = x[5] - y[5];
    xy[6] = x[6] - y[6];	xy[7] = x[7] - y[7];
    xy[8] = x[8] - y[8];	xy[9] = x[9] - y[9];
}

inline int64_t mul32_to64(int32_t a, int32_t b)
{
/*
#ifdef _MSC_VER
	return __emul(a, b);
#else*/
	return a * (int64_t)(b);
/*#endif*/
}


/********************* radix 2^8 math *********************/

void cpy32(uint8_t* d, uint8_t const* s)
{
    for (int i = 0; i < 32; i++)
        d[i] = s[i];
}

/* p[m..n+m-1] = q[m..n+m-1] + z * x */
/* n is the size of x */
/* n+m is the size of p and q */
int mula_small(uint8_t* p, uint8_t const* q, int m, uint8_t const* x, int n, int z)
{
    int v=0;
    for (int i=0;i<n;++i)
    {
        v+=(q[i+m] & 0xFF)+z*(x[i] & 0xFF);
        p[i+m]=(uint8_t)v;
        v>>=8;
    }
    return v;		
}

/* p += x * y * z  where z is a small integer
 * x is size 32, y is size t, p is size 32+t
 * y is allowed to overlap with p+32 if you don't care about the upper half  */
int mula32(uint8_t* p, uint8_t const* x, uint8_t const* y, int t, int z)
{
    int n = 31;
    int w = 0;
    int i = 0;
    for (; i < t; i++) {
        int zy = z * (y[i] & 0xFF);
        w += mula_small(p, p, i, x, n, zy) +
	        (p[i+n] & 0xFF) + zy * (x[n] & 0xFF);
        p[i+n] = (uint8_t)w;
        w >>= 8;
    }
    p[i+n] = (uint8_t)(w + (p[i+n] & 0xFF));
    return w >> 8;
}

/* divide r (size n) by d (size t), returning quotient q and remainder r
 * quotient is size n-t+1, remainder is size t
 * requires t > 0 && d[t-1] != 0
 * requires that r[-1] and d[-1] are valid memory locations
 * q may overlap with r+t */
void divmod(uint8_t* q, uint8_t* r, int n, uint8_t const* d, int t)
{
    int rn = 0;
    int dt = ((d[t-1] & 0xFF) << 8);
    if (t>1)
    {
        dt |= (d[t-2] & 0xFF);
    }
    while (n-- >= t)
    {
        int z = (rn << 16) | ((r[n] & 0xFF) << 8);
        if (n>0)
        {
	        z |= (r[n-1] & 0xFF);
        }
        z/=dt;
        rn += mula_small(r,r, n-t+1, d, t, -z);
        q[n-t+1] = (uint8_t)((z + rn) & 0xFF); /* rn is 0 or -1 (underflow) */
        mula_small(r,r, n-t+1, d, t, -rn);
        rn = (r[n] & 0xFF);
        r[n] = 0;
    }
    r[t-1] = (uint8_t)rn;
}

int numsize(uint8_t* x,int n)
{
    while (n--!=0 && x[n]==0)
        ;
    return n+1;
}

/* Returns x if a contains the gcd, y if b.
 * Also, the returned buffer contains the inverse of a mod b,
 * as 32-uint8_t signed.
 * x and y must have 64 bytes space for temporary use.
 * requires that a[-1] and b[-1] are valid memory locations  */
uint8_t* egcd32(uint8_t* x, uint8_t* y, uint8_t* a, uint8_t* b)
{
    int an, bn = 32, qn, i;
    for (i = 0; i < 32; i++)
        x[i] = y[i] = 0;
    x[0] = 1;
    an = numsize(a, 32);
    if (an==0)
        return y;	/* division by zero */
    uint8_t* temp=new uint8_t[32];
    while (true)
    {
        qn = bn - an + 1;
        divmod(temp, b, bn, a, an);
        bn = numsize(b, bn);
        if (bn==0)
	        return x;
        mula32(y, x, temp, qn, -1);

        qn = an - bn + 1;
        divmod(temp, a, an, b, bn);
        an = numsize(a, an);
        if (an==0)
	        return y;
        mula32(x, y, temp, qn, -1);
    }
}


/* Multiply a number by a small integer i range -185861411 .. 185861411.
 * The output is i reduced form, the input x need not be.  x and xy may point
 * to the same buffer. */
void mul_small(int10& xy, int10& x, int32_t y)
{
    int64_t t;
    t = mul32_to64(x[8],y);
    xy[8] = (t & ((1 << 26) - 1));
    t = (t >> 26) + mul32_to64(x[9],y);
    xy[9] = (t & ((1 << 25) - 1));
    t = 19 * (t >> 25) + mul32_to64(x[0],y);
    xy[0] = (t & ((1 << 26) - 1));
    t = (t >> 26) + mul32_to64(x[1],y);
    xy[1] = (t & ((1 << 25) - 1));
    t = (t >> 25) + mul32_to64(x[2],y);
    xy[2] = (t & ((1 << 26) - 1));
    t = (t >> 26) + mul32_to64(x[3],y);
    xy[3] = (t & ((1 << 25) - 1));
    t = (t >> 25) + mul32_to64(x[4],y);
    xy[4] = (t & ((1 << 26) - 1));
    t = (t >> 26) + mul32_to64(x[5],y);
    xy[5] = (t & ((1 << 25) - 1));
    t = (t >> 25) + mul32_to64(x[6],y);
    xy[6] = (t & ((1 << 26) - 1));
    t = (t >> 26) + mul32_to64(x[7],y);
    xy[7] = (t & ((1 << 25) - 1));
    t = (t >> 25) + xy[8];
    xy[8] = (t & ((1 << 26) - 1));
    xy[9] += int32_t(t >> 26);
}

void mul(int10& dest, int10& x, int10& y)
{
    int32_t x_0=x[0],x_1=x[1],x_2=x[2],x_3=x[3],x_4=x[4],
	     x_5=x[5],x_6=x[6],x_7=x[7],x_8=x[8],x_9=x[9];
    int32_t y_0=y[0],y_1=y[1],y_2=y[2],y_3=y[3],y_4=y[4],
	     y_5=y[5],y_6=y[6],y_7=y[7],y_8=y[8],y_9=y[9];
    int64_t t;
    
    t = mul32_to64(x_0, y_8) + mul32_to64(x_2, y_6) + mul32_to64(x_4, y_4) + mul32_to64(x_6, y_2) +
	    mul32_to64(x_8, y_0) + 2 * (mul32_to64(x_1, y_7) + mul32_to64(x_3, y_5) +
			    mul32_to64(x_5, y_3) + mul32_to64(x_7, y_1)) + 38 *
	    mul32_to64(x_9, y_9);
    dest[8] = (t & ((1 << 26) - 1));
    t = (t >> 26) + mul32_to64(x_0, y_9) + mul32_to64(x_1, y_8) + mul32_to64(x_2, y_7) +
	    mul32_to64(x_3, y_6) + mul32_to64(x_4, y_5) + mul32_to64(x_5, y_4) +
	    mul32_to64(x_6, y_3) + mul32_to64(x_7, y_2) + mul32_to64(x_8, y_1) +
	    mul32_to64(x_9, y_0);
    dest[9] = (t & ((1 << 25) - 1));
    t = mul32_to64(x_0, y_0) + 19 * ((t >> 25) + mul32_to64(x_2, y_8) + mul32_to64(x_4, y_6)
		    + mul32_to64(x_6, y_4) + mul32_to64(x_8, y_2)) + 38 *
	    (mul32_to64(x_1, y_9) + mul32_to64(x_3, y_7) + mul32_to64(x_5, y_5) +
	     mul32_to64(x_7, y_3) + mul32_to64(x_9, y_1));
    dest[0] = (t & ((1 << 26) - 1));
    t = (t >> 26) + mul32_to64(x_0, y_1) + mul32_to64(x_1, y_0) + 19 * (mul32_to64(x_2, y_9)
		    + mul32_to64(x_3, y_8) + mul32_to64(x_4, y_7) + mul32_to64(x_5, y_6) +
		    mul32_to64(x_6, y_5) + mul32_to64(x_7, y_4) + mul32_to64(x_8, y_3) +
		    mul32_to64(x_9, y_2));
    dest[1] = (t & ((1 << 25) - 1));
    t = (t >> 25) + mul32_to64(x_0, y_2) + mul32_to64(x_2, y_0) + 19 * (mul32_to64(x_4, y_8)
		    + mul32_to64(x_6, y_6) + mul32_to64(x_8, y_4)) + 2 * mul32_to64(x_1, y_1)
		    + 38 * (mul32_to64(x_3, y_9) + mul32_to64(x_5, y_7) +
				    mul32_to64(x_7, y_5) + mul32_to64(x_9, y_3));
    dest[2] = (t & ((1 << 26) - 1));
    t = (t >> 26) + mul32_to64(x_0, y_3) + mul32_to64(x_1, y_2) + mul32_to64(x_2, y_1) +
	    mul32_to64(x_3, y_0) + 19 * (mul32_to64(x_4, y_9) + mul32_to64(x_5, y_8) +
			    mul32_to64(x_6, y_7) + mul32_to64(x_7, y_6) +
			    mul32_to64(x_8, y_5) + mul32_to64(x_9, y_4));
    dest[3] = (t & ((1 << 25) - 1));
    t = (t >> 25) + mul32_to64(x_0, y_4) + mul32_to64(x_2, y_2) + mul32_to64(x_4, y_0) + 19 *
	    (mul32_to64(x_6, y_8) + mul32_to64(x_8, y_6)) + 2 * (mul32_to64(x_1, y_3) +
						     mul32_to64(x_3, y_1)) + 38 *
	    (mul32_to64(x_5, y_9) + mul32_to64(x_7, y_7) + mul32_to64(x_9, y_5));
    dest[4] = (t & ((1 << 26) - 1));
    t = (t >> 26) + mul32_to64(x_0, y_5) + mul32_to64(x_1, y_4) + mul32_to64(x_2, y_3) +
	    mul32_to64(x_3, y_2) + mul32_to64(x_4, y_1) + mul32_to64(x_5, y_0) + 19 *
	    (mul32_to64(x_6, y_9) + mul32_to64(x_7, y_8) + mul32_to64(x_8, y_7) +
	     mul32_to64(x_9, y_6));
    dest[5] = (t & ((1 << 25) - 1));
    t = (t >> 25) + mul32_to64(x_0, y_6) + mul32_to64(x_2, y_4) + mul32_to64(x_4, y_2) +
	    mul32_to64(x_6, y_0) + 19 * mul32_to64(x_8, y_8) + 2 * (mul32_to64(x_1, y_5) +
			    mul32_to64(x_3, y_3) + mul32_to64(x_5, y_1)) + 38 *
	    (mul32_to64(x_7, y_9) + mul32_to64(x_9, y_7));
    dest[6] = (t & ((1 << 26) - 1));
    t = (t >> 26) + mul32_to64(x_0, y_7) + mul32_to64(x_1, y_6) + mul32_to64(x_2, y_5) +
	    mul32_to64(x_3, y_4) + mul32_to64(x_4, y_3) + mul32_to64(x_5, y_2) +
	    mul32_to64(x_6, y_1) + mul32_to64(x_7, y_0) + 19 * (mul32_to64(x_8, y_9) +
			    mul32_to64(x_9, y_8));
    dest[7] = (t & ((1 << 25) - 1));
    t = (t >> 25) + dest[8];
    dest[8] = (t & ((1 << 26) - 1));
    dest[9] += int32_t(t >> 26);
}

void sqr(int10& y, int10& x)
{
    
    int32_t x_0=x[0],x_1=x[1],x_2=x[2],x_3=x[3],x_4=x[4],
	     x_5=x[5],x_6=x[6],x_7=x[7],x_8=x[8],x_9=x[9];
    int64_t t;
    
    t = mul32_to64(x_4, x_4) + 2 * (mul32_to64(x_0, x_8) + mul32_to64(x_2, x_6)) + 38 *
	    mul32_to64(x_9, x_9) + 4 * (mul32_to64(x_1, x_7) + mul32_to64(x_3, x_5));
    y[8] = (t & ((1 << 26) - 1));
    t = (t >> 26) + 2 * (mul32_to64(x_0, x_9) + mul32_to64(x_1, x_8) + mul32_to64(x_2, x_7) +
		    mul32_to64(x_3, x_6) + mul32_to64(x_4, x_5));
    y[9] = (t & ((1 << 25) - 1));
    t = 19 * (t >> 25) + mul32_to64(x_0, x_0) + 38 * (mul32_to64(x_2, x_8) +
		    mul32_to64(x_4, x_6) + mul32_to64(x_5, x_5)) + 76 * (mul32_to64(x_1, x_9)
		    + mul32_to64(x_3, x_7));
    y[0] = (t & ((1 << 26) - 1));
    t = (t >> 26) + 2 * mul32_to64(x_0, x_1) + 38 * (mul32_to64(x_2, x_9) +
		    mul32_to64(x_3, x_8) + mul32_to64(x_4, x_7) + mul32_to64(x_5, x_6));
    y[1] = (t & ((1 << 25) - 1));
    t = (t >> 25) + 19 * mul32_to64(x_6, x_6) + 2 * (mul32_to64(x_0, x_2) +
		    mul32_to64(x_1, x_1)) + 38 * mul32_to64(x_4, x_8) + 76 *
		    (mul32_to64(x_3, x_9) + mul32_to64(x_5, x_7));
    y[2] = (t & ((1 << 26) - 1));
    t = (t >> 26) + 2 * (mul32_to64(x_0, x_3) + mul32_to64(x_1, x_2)) + 38 *
	    (mul32_to64(x_4, x_9) + mul32_to64(x_5, x_8) + mul32_to64(x_6, x_7));
    y[3] = (t & ((1 << 25) - 1));
    t = (t >> 25) + mul32_to64(x_2, x_2) + 2 * mul32_to64(x_0, x_4) + 38 *
	    (mul32_to64(x_6, x_8) + mul32_to64(x_7, x_7)) + 4 * mul32_to64(x_1, x_3) + 76 *
	    mul32_to64(x_5, x_9);
    y[4] = (t & ((1 << 26) - 1));
    t = (t >> 26) + 2 * (mul32_to64(x_0, x_5) + mul32_to64(x_1, x_4) + mul32_to64(x_2, x_3))
	    + 38 * (mul32_to64(x_6, x_9) + mul32_to64(x_7, x_8));
    y[5] = (t & ((1 << 25) - 1));
    t = (t >> 25) + 19 * mul32_to64(x_8, x_8) + 2 * (mul32_to64(x_0, x_6) +
		    mul32_to64(x_2, x_4) + mul32_to64(x_3, x_3)) + 4 * mul32_to64(x_1, x_5) +
		    76 * mul32_to64(x_7, x_9);
    y[6] = (t & ((1 << 26) - 1));
    t = (t >> 26) + 2 * (mul32_to64(x_0, x_7) + mul32_to64(x_1, x_6) + mul32_to64(x_2, x_5) +
		    mul32_to64(x_3, x_4)) + 38 * mul32_to64(x_8, x_9);
    y[7] = (t & ((1 << 25) - 1));
    t = (t >> 25) + y[8];
    y[8] = (t & ((1 << 26) - 1));
    y[9] += int32_t(t >> 26);
}

void recip(int10& y, int10& x, int sqrtassist)
{
	int10 t0, t1, t2, t3, t4;
	       
    /* the chain for x^(2^255-21) is straight from djb's implementation */
    sqr(t1, x);	/*  2 == 2 * 1	*/
    sqr(t2, t1);	/*  4 == 2 * 2	*/
    sqr(t0, t2);	/*  8 == 2 * 4	*/
    mul(t2, t0, x);	/*  9 == 8 + 1	*/
    mul(t0, t2, t1);	/* 11 == 9 + 2	*/
    sqr(t1, t0);	/* 22 == 2 * 11	*/
    mul(t3, t1, t2);	/* 31 == 22 + 9
			    == 2^5   - 2^0	*/
    sqr(t1, t3);	/* 2^6   - 2^1	*/
    sqr(t2, t1);	/* 2^7   - 2^2	*/
    sqr(t1, t2);	/* 2^8   - 2^3	*/
    sqr(t2, t1);	/* 2^9   - 2^4	*/
    sqr(t1, t2);	/* 2^10  - 2^5	*/
    mul(t2, t1, t3);	/* 2^10  - 2^0	*/
    sqr(t1, t2);	/* 2^11  - 2^1	*/
    sqr(t3, t1);	/* 2^12  - 2^2	*/
    for (int i = 1; i < 5; i++)
    {
	    sqr(t1, t3);
	    sqr(t3, t1);
    } /* t3 */		/* 2^20  - 2^10	*/
    mul(t1, t3, t2);	/* 2^20  - 2^0	*/
    sqr(t3, t1);	/* 2^21  - 2^1	*/
    sqr(t4, t3);	/* 2^22  - 2^2	*/
    for (int i = 1; i < 10; i++)
    {
	    sqr(t3, t4);
	    sqr(t4, t3);
    } /* t4 */		/* 2^40  - 2^20	*/
    mul(t3, t4, t1);	/* 2^40  - 2^0	*/
    for (int i = 0; i < 5; i++) {
	    sqr(t1, t3);
	    sqr(t3, t1);
    } /* t3 */		/* 2^50  - 2^10	*/
    mul(t1, t3, t2);	/* 2^50  - 2^0	*/
    sqr(t2, t1);	/* 2^51  - 2^1	*/
    sqr(t3, t2);	/* 2^52  - 2^2	*/
    for (int i = 1; i < 25; i++)
    {
	    sqr(t2, t3);
	    sqr(t3, t2);
    } /* t3 */		/* 2^100 - 2^50 */
    mul(t2, t3, t1);	/* 2^100 - 2^0	*/
    sqr(t3, t2);	/* 2^101 - 2^1	*/
    sqr(t4, t3);	/* 2^102 - 2^2	*/
    for (int i = 1; i < 50; i++)
    {
	    sqr(t3, t4);
	    sqr(t4, t3);
    } /* t4 */		/* 2^200 - 2^100 */
    mul(t3, t4, t2);	/* 2^200 - 2^0	*/
    for (int i = 0; i < 25; i++)
    {
	    sqr(t4, t3);
	    sqr(t3, t4);
    } /* t3 */		/* 2^250 - 2^50	*/
    mul(t2, t3, t1);	/* 2^250 - 2^0	*/
    sqr(t1, t2);	/* 2^251 - 2^1	*/
    sqr(t2, t1);	/* 2^252 - 2^2	*/
    if (sqrtassist!=0)
    {
	    mul(y, x, t2);	/* 2^252 - 3 */
    }
    else
    {
	    sqr(t1, t2);	/* 2^253 - 2^3	*/
	    sqr(t2, t1);	/* 2^254 - 2^4	*/
	    sqr(t1, t2);	/* 2^255 - 2^5	*/
	    mul(y, t1, t0);	/* 2^255 - 21	*/
    }
}

/* checks if x is "negative", requires reduced input */
int is_negative(int10 const& x)
{
    return (int)(((is_overflow(x) || (x[9] < 0))?1:0) ^ (x[0] & 1));
}

void sqrt(int10& x, int10& u)
{
	int10 v, t1, t2;

    add(t1, u, u);	/* t1 = 2u		*/
    recip(v, t1, 1);	/* v = (2u)^((p-5)/8)	*/
    sqr(x, v);		/* x = v^2		*/
    mul(t2, t1, x);	/* t2 = 2uv^2		*/
    --t2[0];		/* t2 = 2uv^2-1		*/
    mul(t1, v, t2);	/* t1 = v(2uv^2-1)	*/
    mul(x, u, t1);	/* x = uv(2uv^2-1)	*/
}


/********************* Elliptic curve *********************/

/* y^2 = x^3 + 486662 x^2 + x  over GF(2^255-19) */

/* t1 = ax + az
 * t2 = ax - az  */
void mont_prep(int10& t1, int10& t2, int10& ax, int10& az)
{
    add(t1, ax, az);
    sub(t2, ax, az);
}

/* A = P + Q   where
 *  X(A) = ax/az
 *  X(P) = (t1+t2)/(t1-t2)
 *  X(Q) = (t3+t4)/(t3-t4)
 *  X(P-Q) = dx
 * clobbers t1 and t2, preserves t3 and t4  */
void mont_add(int10& t1, int10& t2, int10& t3, int10& t4, int10& ax, int10& az, int10& dx)
{
    mul(ax, t2, t3);
    mul(az, t1, t4);
    add(t1, ax, az);
    sub(t2, ax, az);
    sqr(ax, t1);
    sqr(t1, t2);
    mul(az, t1, dx);
}

/* B = 2 * Q   where
 *  X(B) = bx/bz
 *  X(Q) = (t3+t4)/(t3-t4)
 * clobbers t1 and t2, preserves t3 and t4  */
void mont_dbl(int10& t1, int10& t2, int10& t3, int10& t4,int10& bx, int10& bz)
{
    sqr(t1, t3);
    sqr(t2, t4);
    mul(bx, t1, t2);
    sub(t2, t1, t2);
    mul_small(bz, t2, 121665);
    add(t1, t1, bz);
    mul(bz, t1, t2);
}

/* Y^2 = X^3 + 486662 X^2 + X
 * t is a temporary  */
void x_to_y2(int10& t, int10& y2, int10& x)
{
    sqr(t, x);
    mul_small(y2, x, 486662);
    add(t, t, y2);
    ++t[0];
    mul(y2, t, x);
}

/* P = kG   and  s = sign(P)/k  */
void core(uint8_t* Px, uint8_t* s, uint8_t const* k, uint8_t const* Gx)
{
	int10 dx, t1, t2, t3, t4;
    
    int10 x[3], z[3];
    
    /* unpack the base */
    if (Gx)
	    unpack(dx, Gx);
    else
	    set(dx, 9);

    /* 0G = point-at-infinity */
    set(x[0], 1);
    set(z[0], 0);

    /* 1G = G */
    x[1] = dx;
    set(z[1], 1);

    for (int i = 32; i-- > 0; )
    {
	    for (int j = 8; j--> 0; )
	    {
		    /* swap arguments depending on bit */
		    int bit1 = ((k[i] & 0xFF) >> j) & 1;
		    int bit0 = (~(k[i] & 0xFF) >> j) & 1;
		    int10& ax = x[bit0];
		    int10& az = z[bit0];
		    int10& bx = x[bit1];
		    int10& bz = z[bit1];

		    /* a' = a + b	*/
		    /* b' = 2 b	*/
		    mont_prep(t1, t2, ax, az);
		    mont_prep(t3, t4, bx, bz);
		    mont_add(t1, t2, t3, t4, ax, az, dx);
		    mont_dbl(t1, t2, t3, t4, bx, bz);
	    }
    }

    recip(t1, z[0], 0);
    mul(dx, x[0], t1);
    pack(dx, Px);

    /* calculate s such that s abs(P) = G  .. assumes G is std base point */
    if (s)
    {
	    x_to_y2(t2, t1, dx);	/* t1 = Py^2  */
	    recip(t3, z[1], 0);	/* where Q=P+G ... */
	    mul(t2, x[1], t3);	/* t2 = Qx  */
	    add(t2, t2, dx);	/* t2 = Qx + Px  */
	    t2[0] += 9 + 486662;	/* t2 = Qx + Px + Gx + 486662  */
	    dx[0] -= 9;		/* dx = Px - Gx  */
	    sqr(t3, dx);	/* t3 = (Px - Gx)^2  */
	    mul(dx, t2, t3);	/* dx = t2 (Px - Gx)^2  */
	    sub(dx, dx, t1);	/* dx = t2 (Px - Gx)^2 - Py^2  */
	    dx[0] -= 39420360;	/* dx = t2 (Px - Gx)^2 - Py^2 - Gy^2  */
	    mul(t1, dx, BASE_R2Y);	/* t1 = -Py  */
	    if (is_negative(t1)!=0)	/* sign is 1, so just copy  */
		    cpy32(s, k);
	    else			/* sign is -1, so negate  */
		    mula_small(s, ORDER_TIMES_8, 0, k, 32, -1);

	    /* reduce s mod q
	     * (is this needed?  do it just in case, it's fast anyway) */
	    //divmod((dstptr) t1, s, 32, order25519, 32);

	    /* take reciprocal of s mod q */
	    uint8_t* temp1=new uint8_t[32];
	    uint8_t* temp2=new uint8_t[64];
	    uint8_t* temp3=new uint8_t[64];
	    cpy32(temp1, ORDER);
	    cpy32(s, egcd32(temp2, temp3, s, temp1));
	    if ((s[31] & 0x80)!=0)
		    mula_small(s, s, 0, ORDER, 32, 1);
    }
}

} // namespace



/********* DIGITAL SIGNATURES *********/

/* deterministic EC-KCDSA
 *
 *    s is the private key for signing
 *    P is the corresponding public key
 *    Z is the context data (signer public key or certificate, etc)
 *
 * signing:
 *
 *    m = hash(Z, message)
 *    x = hash(m, s)
 *    keygen25519(Y, NULL, x);
 *    r = hash(Y);
 *    h = m XOR r
 *    sign25519(v, h, x, s);
 *
 *    output (v,r) as the signature
 *
 * verification:
 *
 *    m = hash(Z, message);
 *    h = m XOR r
 *    verify25519(Y, v, h, P)
 *
 *    confirm  r == hash(Y)
 *
 * It would seem to me that it would be simpler to have the signer directly do 
 * h = hash(m, Y) and send that to the recipient instead of r, who can verify 
 * the signature by checking h == hash(m, Y).  If there are any problems with 
 * such a scheme, please let me know.
 *
 * Also, EC-KCDSA (like most DS algorithms) picks x random, which is a waste of 
 * perfectly good entropy, but does allow Y to be calculated in advance of (or 
 * parallel to) hashing the message.
 */

/* Signature generation primitive, calculates (x-h)s mod q
 *   v  [out] signature value
 *   h  [in]  signature hash (of message, signature pub key, and context data)
 *   x  [in]  signature private key
 *   s  [in]  private key for signing
 * returns true on success, false on failure (use different x or h)
 */
 


bool curve25519::sign(uint8_t* v, uint8_t const* h, uint8_t const* x, uint8_t const* s)
{
    /* v = (x - h) s  mod q  */
    uint8_t tmp1[65];
    uint8_t tmp2[33];
    int w;
    int i;
    for (i = 0; i < 32; i++)
	    v[i] = 0;
    i = mula_small(v, x, 0, h, 32, -1);
    mula_small(v, v, 0, ORDER, 32, (15-v[31])/16);
    mula32(tmp1, v, s, 32, 1);
    divmod(tmp2, tmp1, 64, ORDER, 32);
    for (w = 0, i = 0; i < 32; i++)
	    w |= v[i] = tmp1[i];
    return w != 0;
}

/* Signature verification primitive, calculates Y = vP + hG
 *   Y  [out] signature public key
 *   v  [in]  signature value
 *   h  [in]  signature hash
 *   P  [in]  public key
 */
void curve25519::verify(uint8_t* Y, uint8_t const* v, uint8_t const* h, uint8_t const* P)
{
    /* Y = v abs(P) + h G  */
    uint8_t d[32];
    
    int10 p[2], s[2], yx[3], yz[3], t1[3], t2[3];
    	
    int vi = 0, hi = 0, di = 0, nvh=0, i, j, k;

    /* set p[0] to G and p[1] to P  */

    set(p[0], 9);
    unpack(p[1], P);

    /* set s[0] to P+G and s[1] to P-G  */

    /* s[0] = (Py^2 + Gy^2 - 2 Py Gy)/(Px - Gx)^2 - Px - Gx - 486662  */
    /* s[1] = (Py^2 + Gy^2 + 2 Py Gy)/(Px - Gx)^2 - Px - Gx - 486662  */

    x_to_y2(t1[0], t2[0], p[1]);	/* t2[0] = Py^2  */
    sqrt(t1[0], t2[0]);	/* t1[0] = Py or -Py  */
    j = is_negative(t1[0]);		/*      ... check which  */
    t2[0][0] += 39420360;		/* t2[0] = Py^2 + Gy^2  */
    mul(t2[1], BASE_2Y, t1[0]);/* t2[1] = 2 Py Gy or -2 Py Gy  */
    sub(t1[j], t2[0], t2[1]);	/* t1[0] = Py^2 + Gy^2 - 2 Py Gy  */
    add(t1[1-j], t2[0], t2[1]);/* t1[1] = Py^2 + Gy^2 + 2 Py Gy  */
    t2[0] = p[1];		/* t2[0] = Px  */
    t2[0][0] -= 9;			/* t2[0] = Px - Gx  */
    sqr(t2[1], t2[0]);		/* t2[1] = (Px - Gx)^2  */
    recip(t2[0], t2[1], 0);	/* t2[0] = 1/(Px - Gx)^2  */
    mul(s[0], t1[0], t2[0]);	/* s[0] = t1[0]/(Px - Gx)^2  */
    sub(s[0], s[0], p[1]);	/* s[0] = t1[0]/(Px - Gx)^2 - Px  */
    s[0][0] -= 9 + 486662;		/* s[0] = X(P+G)  */
    mul(s[1], t1[1], t2[0]);	/* s[1] = t1[1]/(Px - Gx)^2  */
    sub(s[1], s[1], p[1]);	/* s[1] = t1[1]/(Px - Gx)^2 - Px  */
    s[1][0] -= 9 + 486662;		/* s[1] = X(P-G)  */
    mul_small(s[0], s[0], 1);	/* reduce s[0] */
    mul_small(s[1], s[1], 1);	/* reduce s[1] */


    /* prepare the chain  */
    for (i = 0; i < 32; i++)
    {
	    vi = (vi >> 8) ^ (v[i] & 0xFF) ^ ((v[i] & 0xFF) << 1);
	    hi = (hi >> 8) ^ (h[i] & 0xFF) ^ ((h[i] & 0xFF) << 1);
	    nvh = ~(vi ^ hi);
	    di = (nvh & (di & 0x80) >> 7) ^ vi;
	    di ^= nvh & (di & 0x01) << 1;
	    di ^= nvh & (di & 0x02) << 1;
	    di ^= nvh & (di & 0x04) << 1;
	    di ^= nvh & (di & 0x08) << 1;
	    di ^= nvh & (di & 0x10) << 1;
	    di ^= nvh & (di & 0x20) << 1;
	    di ^= nvh & (di & 0x40) << 1;
	    d[i] = (uint8_t)di;
    }

    di = ((nvh & (di & 0x80) << 1) ^ vi) >> 8;

    /* initialize state */
    set(yx[0], 1);
    yx[1] = p[di];
    yx[2] = s[0];
    set(yz[0], 0);
    set(yz[1], 1);
    set(yz[2], 1);

    /* y[0] is (even)P + (even)G
     * y[1] is (even)P + (odd)G  if current d-bit is 0
     * y[1] is (odd)P + (even)G  if current d-bit is 1
     * y[2] is (odd)P + (odd)G
     */

    vi = 0;
    hi = 0;

    /* and go for it! */
    for (i = 32; i-- > 0; )
    {
	    vi = (vi << 8) | (v[i] & 0xFF);
	    hi = (hi << 8) | (h[i] & 0xFF);
	    di = (di << 8) | (d[i] & 0xFF);

	    for (j = 8; j-- > 0; )
	    {
		    mont_prep(t1[0], t2[0], yx[0], yz[0]);
		    mont_prep(t1[1], t2[1], yx[1], yz[1]);
		    mont_prep(t1[2], t2[2], yx[2], yz[2]);

		    k = ((vi ^ vi >> 1) >> j & 1)
		      + ((hi ^ hi >> 1) >> j & 1);
		    mont_dbl(yx[2], yz[2], t1[k], t2[k], yx[0], yz[0]);

		    k = (di >> j & 2) ^ ((di >> j & 1) << 1);
		    mont_add(t1[1], t2[1], t1[k], t2[k], yx[1], yz[1],
				    p[di >> j & 1]);

		    mont_add(t1[2], t2[2], t1[0], t2[0], yx[2], yz[2],
				    s[((vi ^ hi) >> j & 2) >> 1]);
	    }
    }

    k = (vi & 1) + (hi & 1);
    recip(t1[0], yz[k], 0);
    mul(t1[1], yx[k], t1[0]);

    pack(t1[1], Y);
}


void curve25519::clamp(uint8_t* k)
{
    k[31] &= 0x7F;
    k[31] |= 0x40;
    k[ 0] &= 0xF8;
}

/* Key-pair generation
 *   P  [out] your public key
 *   s  [out] your private key for signing
 *   k  [out] your private key for key agreement
 *   k  [in]  32 random bytes
 * s may be NULL if you don't care
 *
 * WARNING: if s is not NULL, this function has data-dependent timing */
void curve25519::keygen(uint8_t* P, uint8_t* s, uint8_t* k)
{
    clamp(k);
    core(P, s, k, 0);
}

/* Key agreement
 *   Z  [out] shared secret (needs hashing before use)
 *   k  [in]  your private key for key agreement
 *   P  [in]  peer's public key
 */
void curve25519::curve(uint8_t* Z, uint8_t* k, uint8_t* P)
{
    core(Z, 0, k, P);
}

} // namespace gvl
