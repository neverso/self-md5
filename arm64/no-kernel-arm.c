#include <fcntl.h>
#include <linux/if_alg.h>
#include <sys/syscall.h>
#include <unistd.h>
#define BLOCK_LEN 64 // In bytes

/****************************** MACROS ******************************/
#define ROTLEFT(a, b) ((a << b) | (a >> (32 - b)))

#define FF(x, y, z) ((x & y) | (~x & z))

#define GG(x, y, z) ((x & z) | (y & ~z))

#define HH(x, y, z) (x ^ y ^ z)

#define II(x, y, z) (y ^ (x | ~z))

// static long double fsin_my(long double a)
// {
//     long double res;
//     // prof wiht register
//     asm __volatile__("fsin\n\t"
//                      : "=t"(res)
//                      : "0"(a)
//                      : "memory");

//     return (res) > 0 ? res : -res;
// }
unsigned int KK[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
    0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
    0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x2441453,  0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
    0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
    0xd4ef3085, 0x4881d05,  0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
    0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
// #define ull unsigned long long
#define START 0x400000
const char ss[] = {7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21};
int main(int argc, char *argv[])
{

    char *data = (char *)START;
    unsigned int hash[4] = {(unsigned int)(0x67452301), (unsigned int)(0xEFCDAB89), (unsigned int)(0x98BADCFE), (unsigned int)(0x10325476)};
    const short len = 888;
    const short new_len = ((((len + 8) / 64) + 1) * 64) - 8;
    data[len] = 0x80;
    *(unsigned long long *)(data + new_len) = len << 3;

    for (short j = 0; j < new_len; j += BLOCK_LEN)
    {
        unsigned int *m = (unsigned int *)&data[j];
        unsigned int A, B, C, D;
        A = hash[0];
        B = hash[1];
        C = hash[2];
        D = hash[3];
        char g;
        unsigned int inc;
        for (char i = 0; i < 64; ++i)
        {
            switch (i / 16)
            {
            case 0:
                inc = FF(B, C, D);
                g = i;
                break;
            case 1:
                inc = GG(B, C, D);
                g = (5 * i + 1) % 16;
                break;
            case 2:
                inc = HH(B, C, D);
                g = (3 * i + 5) % 16;
                break;
            case 3:
                inc = II(B, C, D);
                g = (7 * i) % 16;
            }
            // unsigned int t = (unsigned int)((unsigned long long)4294967296 * KK[i]);
            inc += A + m[g] + KK[i];
            A = D;
            D = C;
            C = B;
            B += ROTLEFT(inc, ss[(i / 16) * 4 + (i % 4)]);
        }
        hash[1] += B;
        hash[2] += C;
        hash[3] += D;
        hash[0] += A;
    }
    const char *chash = (unsigned char *)&hash;
    for (unsigned char i = 0; i < 32; i++)
    {
        char a = (chash[i >> 1] >> (4 * (1 - i % 2))) & 0xF;
        char c = a >= 10 ? a + ('a' - 10) : a + '0';
        // syscall(SYS_write, 1, &c, 1);
        write(1,&c,1);
    }
    // syscall(SYS_exit, 0);
    exit(0);
}
