#include <stdlib.h>
#ifndef hmac_md5_INCLUDED
#define hmac_md5_INCLUDED

#ifndef ARCH_IS_BIG_ENDIAN
#define ARCH_IS_BIG_ENDIAN 0 
#endif


#ifndef md5_INCLUDED
#  define md5_INCLUDED

typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
	md5_word_t count[2];	/* message length in bits, lsw first */
	md5_word_t abcd[4];		/* digest buffer */
	md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;

#ifdef __cplusplus
extern "C" 
{
	#endif
	
	/* Initialize the algorithm. */
	void md5_init(md5_state_t *pms);
	
	/* Append a string to the message. */
	void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);
	
	/* Finish the message and return the digest. */
	void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);
	
#ifdef __cplusplus
	}  /* end extern "C" */
#endif
	
	#endif /* md5_INCLUDED */

	#include <string.h>
	
	#undef BYTE_ORDER	/* 1 = big-endian, -1 = little-endian, 0 = unknown */
#ifdef ARCH_IS_BIG_ENDIAN
	#  define BYTE_ORDER (ARCH_IS_BIG_ENDIAN ? 1 : -1)
#else
	#  define BYTE_ORDER 0
#endif
	
	#define T_MASK ((md5_word_t)~0)
	#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87)
	#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9)
	#define T3    0x242070db
	#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111)
	#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050)
	#define T6    0x4787c62a
	#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ec)
	#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afe)
	#define T9    0x698098d8
	#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850)
	#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44e)
	#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841)
	#define T13    0x6b901122
	#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6c)
	#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71)
	#define T16    0x49b40821
	#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9d)
	#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbf)
	#define T19    0x265e5a51
	#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855)
	#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2)
	#define T22    0x02441453
	#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197e)
	#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437)
	#define T25    0x21e1cde6
	#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829)
	#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278)
	#define T28    0x455a14ed
	#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16fa)
	#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07)
	#define T31    0x676f02d9
	#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375)
	#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bd)
	#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097e)
	#define T35    0x6d9d6122
	#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3)
	#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bb)
	#define T38    0x4bdecfa9
	#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49f)
	#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438f)
	#define T41    0x289b7ec6
	#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805)
	#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7a)
	#define T44    0x04881d05
	#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6)
	#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661a)
	#define T47    0x1fa27cf8
	#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99a)
	#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbb)
	#define T50    0x432aff97
	#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58)
	#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6)
	#define T53    0x655b59c3
	#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336d)
	#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82)
	#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22e)
	#define T57    0x6fa87e4f
	#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191f)
	#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebceb)
	#define T60    0x4e0811a1
	#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817d)
	#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dca)
	#define T63    0x2ad7d2bb
	#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6e)
	
	
	static void
	md5_process(md5_state_t *pms, const md5_byte_t *data /*[64]*/)
	{
		md5_word_t
		a = pms->abcd[0], b = pms->abcd[1],
		c = pms->abcd[2], d = pms->abcd[3];
		md5_word_t t;
#if BYTE_ORDER > 0
		/* Define storage only for big-endian CPUs. */
		md5_word_t X[16];
#else
		/* Define storage for little-endian or both types of CPUs. */
		md5_word_t xbuf[16];
		const md5_word_t *X;
#endif
		
		{
#if BYTE_ORDER == 0
			/*
			* Determine dynamically whether this is a big-endian or
			* little-endian machine, since we can use a more efficient
			* algorithm on the latter.
			*/
			static const int w = 1;
			
			if (*((const md5_byte_t *)&w)) /* dynamic little-endian */
#endif
#if BYTE_ORDER <= 0		/* little-endian */
			{
				/*
				* On little-endian machines, we can process properly aligned
				* data without copying it.
				*/
				if (!((data - (const md5_byte_t *)0) & 3)) {
					/* data are properly aligned */
					X = (const md5_word_t *)data;
				} else {
					/* not aligned */
					memcpy(xbuf, data, 64);
					X = xbuf;
				}
			}
#endif
#if BYTE_ORDER == 0
			else			/* dynamic big-endian */
#endif
#if BYTE_ORDER >= 0		/* big-endian */
			{
				/*
				* On big-endian machines, we must arrange the bytes in the
				* right order.
				*/
				const md5_byte_t *xp = data;
				int i;
				
#  if BYTE_ORDER == 0
				X = xbuf;		/* (dynamic only) */
#  else
				#    define xbuf X		/* (static only) */
#  endif
				for (i = 0; i < 16; ++i, xp += 4)
					xbuf[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);
			}
#endif
		}
		
		#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
		
		/* Round 1. */
		/* Let [abcd k s i] denote the operation
		a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
		#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
		#define SET(a, b, c, d, k, s, Ti)\
		t = a + F(b,c,d) + X[k] + Ti;\
		a = ROTATE_LEFT(t, s) + b
		/* Do the following 16 operations. */
		SET(a, b, c, d,  0,  7,  T1);
		SET(d, a, b, c,  1, 12,  T2);
		SET(c, d, a, b,  2, 17,  T3);
		SET(b, c, d, a,  3, 22,  T4);
		SET(a, b, c, d,  4,  7,  T5);
		SET(d, a, b, c,  5, 12,  T6);
		SET(c, d, a, b,  6, 17,  T7);
		SET(b, c, d, a,  7, 22,  T8);
		SET(a, b, c, d,  8,  7,  T9);
		SET(d, a, b, c,  9, 12, T10);
		SET(c, d, a, b, 10, 17, T11);
		SET(b, c, d, a, 11, 22, T12);
		SET(a, b, c, d, 12,  7, T13);
		SET(d, a, b, c, 13, 12, T14);
		SET(c, d, a, b, 14, 17, T15);
		SET(b, c, d, a, 15, 22, T16);
		#undef SET
		
		/* Round 2. */
		/* Let [abcd k s i] denote the operation
		a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
		#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
		#define SET(a, b, c, d, k, s, Ti)\
		t = a + G(b,c,d) + X[k] + Ti;\
		a = ROTATE_LEFT(t, s) + b
		/* Do the following 16 operations. */
		SET(a, b, c, d,  1,  5, T17);
		SET(d, a, b, c,  6,  9, T18);
		SET(c, d, a, b, 11, 14, T19);
		SET(b, c, d, a,  0, 20, T20);
		SET(a, b, c, d,  5,  5, T21);
		SET(d, a, b, c, 10,  9, T22);
		SET(c, d, a, b, 15, 14, T23);
		SET(b, c, d, a,  4, 20, T24);
		SET(a, b, c, d,  9,  5, T25);
		SET(d, a, b, c, 14,  9, T26);
		SET(c, d, a, b,  3, 14, T27);
		SET(b, c, d, a,  8, 20, T28);
		SET(a, b, c, d, 13,  5, T29);
		SET(d, a, b, c,  2,  9, T30);
		SET(c, d, a, b,  7, 14, T31);
		SET(b, c, d, a, 12, 20, T32);
		#undef SET
		
		/* Round 3. */
		/* Let [abcd k s t] denote the operation
		a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
		#define H(x, y, z) ((x) ^ (y) ^ (z))
		#define SET(a, b, c, d, k, s, Ti)\
		t = a + H(b,c,d) + X[k] + Ti;\
		a = ROTATE_LEFT(t, s) + b
		/* Do the following 16 operations. */
		SET(a, b, c, d,  5,  4, T33);
		SET(d, a, b, c,  8, 11, T34);
		SET(c, d, a, b, 11, 16, T35);
		SET(b, c, d, a, 14, 23, T36);
		SET(a, b, c, d,  1,  4, T37);
		SET(d, a, b, c,  4, 11, T38);
		SET(c, d, a, b,  7, 16, T39);
		SET(b, c, d, a, 10, 23, T40);
		SET(a, b, c, d, 13,  4, T41);
		SET(d, a, b, c,  0, 11, T42);
		SET(c, d, a, b,  3, 16, T43);
		SET(b, c, d, a,  6, 23, T44);
		SET(a, b, c, d,  9,  4, T45);
		SET(d, a, b, c, 12, 11, T46);
		SET(c, d, a, b, 15, 16, T47);
		SET(b, c, d, a,  2, 23, T48);
		#undef SET
		
		/* Round 4. */
		/* Let [abcd k s t] denote the operation
		a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
		#define I(x, y, z) ((y) ^ ((x) | ~(z)))
		#define SET(a, b, c, d, k, s, Ti)\
		t = a + I(b,c,d) + X[k] + Ti;\
		a = ROTATE_LEFT(t, s) + b
		/* Do the following 16 operations. */
		SET(a, b, c, d,  0,  6, T49);
		SET(d, a, b, c,  7, 10, T50);
		SET(c, d, a, b, 14, 15, T51);
		SET(b, c, d, a,  5, 21, T52);
		SET(a, b, c, d, 12,  6, T53);
		SET(d, a, b, c,  3, 10, T54);
		SET(c, d, a, b, 10, 15, T55);
		SET(b, c, d, a,  1, 21, T56);
		SET(a, b, c, d,  8,  6, T57);
		SET(d, a, b, c, 15, 10, T58);
		SET(c, d, a, b,  6, 15, T59);
		SET(b, c, d, a, 13, 21, T60);
		SET(a, b, c, d,  4,  6, T61);
		SET(d, a, b, c, 11, 10, T62);
		SET(c, d, a, b,  2, 15, T63);
		SET(b, c, d, a,  9, 21, T64);
		#undef SET
		
		/* Then perform the following additions. (That is increment each
		of the four registers by the value it had before this block
		was started.) */
		pms->abcd[0] += a;
		pms->abcd[1] += b;
		pms->abcd[2] += c;
		pms->abcd[3] += d;
	}
	
	void
	md5_init(md5_state_t *pms)
	{
		pms->count[0] = pms->count[1] = 0;
		pms->abcd[0] = 0x67452301;
		pms->abcd[1] = /*0xefcdab89*/ T_MASK ^ 0x10325476;
		pms->abcd[2] = /*0x98badcfe*/ T_MASK ^ 0x67452301;
		pms->abcd[3] = 0x10325476;
	}
	
	void
	md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes)
	{
		const md5_byte_t *p = data;
		int left = nbytes;
		int offset = (pms->count[0] >> 3) & 63;
		md5_word_t nbits = (md5_word_t)(nbytes << 3);
		
		if (nbytes <= 0)
			return;
		
		/* Update the message length. */
		pms->count[1] += nbytes >> 29;
		pms->count[0] += nbits;
		if (pms->count[0] < nbits)
			pms->count[1]++;
		
		/* Process an initial partial block. */
		if (offset) {
			int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);
			
			memcpy(pms->buf + offset, p, copy);
			if (offset + copy < 64)
				return;
			p += copy;
			left -= copy;
			md5_process(pms, pms->buf);
		}
		
		/* Process full blocks. */
		for (; left >= 64; p += 64, left -= 64)
			md5_process(pms, p);
		
		/* Process a final partial block. */
		if (left)
			memcpy(pms->buf, p, left);
	}
	
	void
	md5_finish(md5_state_t *pms, md5_byte_t digest[16])
	{
		static const md5_byte_t pad[64] = {
			0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};
		md5_byte_t data[8];
		int i;
		
		/* Save the length before padding. */
		for (i = 0; i < 8; ++i)
			data[i] = (md5_byte_t)(pms->count[i >> 2] >> ((i & 3) << 3));
		/* Pad to 56 bytes mod 64. */
		md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
		/* Append the length. */
		md5_append(pms, data, 8);
		for (i = 0; i < 16; ++i)
			digest[i] = (md5_byte_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
	}


#ifdef __cplusplus
extern "C" 
{
	#endif
	
	void hmac_md5(unsigned char* text, int text_len, unsigned char* key, int key_len, md5_byte_t* digest);
	
#ifdef __cplusplus
	}  /* end extern "C" */
#endif
	
	#endif /* hmac_md5_INCLUDED */

	
	void
	hmac_md5(text, text_len, key, key_len, digest)
	unsigned char*  text;                /* pointer to data stream */
	int             text_len;            /* length of data stream */
	unsigned char*  key;                 /* pointer to authentication key */
	int             key_len;             /* length of authentication key */
	md5_byte_t*     digest;              /* caller digest to be filled in */
	
	{
		md5_state_t context;
		unsigned char k_ipad[65];    /* inner padding -
		* key XORd with ipad
		*/
		unsigned char k_opad[65];    /* outer padding -
		* key XORd with opad
		*/
		unsigned char tk[16];
		int i;
		/* if key is longer than 64 bytes reset it to key=MD5(key) */
		if (key_len > 64) {
			
			md5_state_t      tctx;
			
			md5_init(&tctx);
			md5_append(&tctx, key, key_len);
			md5_finish(&tctx, tk);
			
			key = tk;
			key_len = 16;
		}
		
		/*
		* the HMAC_MD5 transform looks like:
		*
		* MD5(K XOR opad, MD5(K XOR ipad, text))
		*
		* where K is an n byte key
		* ipad is the byte 0x36 repeated 64 times
		* opad is the byte 0x5c repeated 64 times
		* and text is the data being protected
		*/
		
		/* start out by storing key in pads */
		bzero( k_ipad, sizeof k_ipad);
		bzero( k_opad, sizeof k_opad);
		bcopy( key, k_ipad, key_len);
		bcopy( key, k_opad, key_len);
		
		/* XOR key with ipad and opad values */
		for (i=0; i<64; i++) {
			k_ipad[i] ^= 0x36;
			k_opad[i] ^= 0x5c;
		}
		/*
		* perform inner MD5
		*/
		md5_init(&context);                   /* init context for 1st
		* pass */
		md5_append(&context, k_ipad, 64);     /* start with inner pad */
		md5_append(&context, text, text_len); /* then text of datagram */
		md5_finish(&context, digest);         /* finish up 1st pass */
		/*
		* perform outer MD5
		*/
		md5_init(&context);                   /* init context for 2nd
		* pass */
		md5_append(&context, k_opad, 64);     /* start with outer pad */
		md5_append(&context, digest, 16);     /* then results of 1st
		* hash */
		md5_finish(&context, digest);          /* finish up 2nd pass */
	}
	

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

struct lppShortAnchorPosition_s {
	float position[3];
	uint8_t hash[8];
} __attribute__((packed));
	
	
void md5(char *input, int length, unsigned char *output) {
	md5_state_t tctx;
	md5_init(&tctx);
	md5_append(&tctx, (unsigned char *)input, length);
	md5_finish(&tctx, output);
}
	
void populateKeys(char keys[100][16], int count) {
	strcpy(keys[0], "1234567890123456");
	for (int i = 1; i < count; i++) {
		char *previousKey = keys[i-1];
		unsigned char newKey[16];
		md5(previousKey, 16, newKey);
		memcpy(keys[i], newKey, 16);
	}
} 
	
	
	static md5_state_t hash_state;
	
	static void genMD5(md5_byte_t *input, uint8_t len, md5_byte_t *output) {
		md5_init(&hash_state);
		md5_append(&hash_state, input, len);
		md5_finish(&hash_state, output);
	}
	
void printKey(md5_byte_t *key) {
	for (int i = 0; i < 16; i++)
		printf("0x%02x, ", key[i]);
	putchar('\n');
}
	
int main(int argc, char *argv[]) {
	const char ids[8] = {'0','1','2','3','4','5','6','7'};
	const int len = 16;
	md5_byte_t keychain[len] = {'\0'}; // 50 lpp/s over 10 minutes of keysize 8
	
	
	const int keychainSize = 200;
	md5_byte_t skeychain[keychainSize+1][16] = {'\0'}; // last one is commitment and unused


	md5_byte_t s = 0x0;
	genMD5(&s, 1, skeychain[0]);
	
	printf("s = 0x0 \n");

	for (int i = 1; i < keychainSize+2;i++) {
		genMD5(skeychain[i-1], 16, skeychain[i]);
		i < keychainSize+1 ? printf("key at %d = ", i-1) : printf("commitment = 0x");		
		printKey(skeychain[i-1]);
	}

	for (int index = 0; index < 8;index++) {
		keychain[0]=ids[index];
		for (int i = 1; i < len; i++) {
			md5_byte_t output[16];
			genMD5(&keychain[i-1], 1, output);
			keychain[i] = output[0];
		}
		
		printf("keychain for anchor id=%d\n", index);

		for (int i = 0; i < len; i++)
			printf("%02x", keychain[i]);
		putchar('\n');
	}
	md5_byte_t key[8] = {'0','0','0','0','0','0','0','0'};
	key[0] = keychain[len-(1%len)];

	
	
	

	/*
	char keys[100][16];
	populateKeys(keys, 100);
	printf("now printing");
	
	for (int i = 0; i < 16; i++)
		printf("%02x", keys[0][i]);
	*/
	
	/*
	anchors = [Anchor('0',-1,-1,0),
	Anchor('1',-1,+1,1),
	Anchor('2',+1,+1,0),
	Anchor('3',+1,-1,1),
	Anchor('4',-1,-1,1),
	Anchor('5',-1,+1,0),
	Anchor('6',+1,+1,1),
	Anchor('7',+1,-1,0)]*/
	unsigned char hashebytes[8][16] = { {0x1f,0x2e,0x2b,0x19,0xf2,0xb9,0xdb,0x68,0x5e,0xf0,0x5b,0x65,0x38,0x5a,0x40,0x62},
										{0xaf,0xc1,0xd3,0xc0,0xf6,0x82,0xb3,0xe6,0x9b,0xe9,0xff,0xfe,0x71,0x39,0xe0,0x68},
										{0xe4,0x8f,0xd3,0xe1,0xc9,0x4b,0xec,0xc9,0x27,0x61,0x82,0x7a,0x68,0x00,0x3c,0xdc},
										{0xa9,0x2c,0x1a,0xef,0x23,0xfb,0x38,0xdf,0xeb,0x27,0x78,0xbc,0xe8,0x0f,0x7b,0xf8},
										{0xcb,0x4d,0x44,0x6a,0x2e,0xbf,0xf9,0x49,0x5e,0x60,0x24,0x7b,0x83,0x5f,0xa1,0xf2},
										{0x06,0xe3,0xf9,0xff,0x1f,0x8d,0xb3,0x29,0x87,0x8c,0x17,0x15,0x29,0xd5,0x94,0x8a},
										{0xc9,0xcc,0xdc,0xfe,0xa6,0x75,0x0d,0xda,0x1c,0x5e,0x82,0x0f,0x4e,0xca,0xcb,0x5e},
										{0x39,0xb5,0xd4,0xeb,0x7f,0xed,0x44,0xdd,0x43,0x70,0x21,0x2c,0xff,0x27,0x43,0x17}};
/*
	static const float constellation[8][3] = {{ -1.0,-1.0,0.0 },
		{ -1.0,+1.0,1.0 },
		{ +1.0,+1.0,0.0 },
		{ +1.0,-1.0,1.0 },
		{ -1.0,-1.0,1.0 },
		{ -1.0,+1.0,0.0 },
		{ +1.0,+1.0,1.0 },
		{ +1.0,-1.0,0.0 }};  
	
	
	for (int i = 0; i< 8; i++) {
		md5_state_t      tctx;
		unsigned char tk[16];
		
		md5_init(&tctx);
		md5_append(&tctx, constellation[i], 12);
		md5_finish(&tctx, tk);

		for (int j = 0; j < 16; j++)
			printf("0x%02x,", tk[j]);

		putchar('\n');
	}
*/
	struct lppShortAnchorPosition_s pos;
	pos.position[0] = -1.0;
	pos.position[1] = -1.0;
	pos.position[2] = 0;
	
	struct lppShortAnchorPosition_s *posptr = &pos;

	char *hexstring = "0xafc1d3c0f682b3e69be9fffe7139e068";
	float mypos[3];
	

	char msg[12] = "123456789012";
	char *someKey = "key";
	
	md5_state_t      tctx;
	unsigned char tk[16];

	md5_init(&tctx);
	md5_append(&tctx, pos.position, 12);
	md5_finish(&tctx, tk);
	
	printf("tk=");
	for (int i = 0; i < 16; i++)
		printf("%02x", tk[i]);
	
	printf("\nhashbytes[0]=");
	
	for (int i = 0; i < 16; i++)
		printf("%02x", hashebytes[0][i]);

	if (memcmp(tk, hashebytes[0], 16) == 0) 
		printf("equal");
		
	md5_byte_t mybytes[16];
	sscanf(hexstring,"%x",&mybytes); 
	
	putchar('\n');
	printf("h=");
	for (int i = 0; i < 16; i++)
		printf("%02x", tk[i]);
	putchar('\n');
	
	md5_byte_t mac_digest[16];
	
		
	memcpy(msg, &pos, 12);


	hmac_md5((unsigned char *)msg, 12, (unsigned char *)someKey, 3, mac_digest); // msg is 12 bytes, key is 3 bytes, write result to mac_digest
	
	memcpy(posptr->hash, mac_digest, 8);
	
	
	char test[100];
	printf("the test '%s'\n",test);
	
	unsigned char encTK[16];

	for (int i = 0; i < 16; i++) 
		sscanf(tk + 2*i,"%2s",&encTK[i]);

	printf("encoded = %s\n",encTK);
	strcpy(test, tk);
	printf("the test '%s'\n",test);
	
	strcpy(test, mac_digest);
	printf("the test '%s'\n",test);

	
	printf("\nhmac=");
	for (int i = 0; i < 16; i++)
		printf("%02x", mac_digest[i]);


	hmac_md5((unsigned char*)&pos, 12, (unsigned char *)someKey, 3, mac_digest); // msg is 12 bytes, key is 3 bytes, write result to mac_digest
	
	printf("\nhmac=");
	for (int i = 0; i < 16; i++)
		printf("%02x", mac_digest[i]);
	
	
	hmac_md5((unsigned char*)&pos, 12, NULL, 0, mac_digest); // msg is 12 bytes, key is 3 bytes, write result to mac_digest
	
	printf("\nhmac=");
	for (int i = 0; i < 16; i++)
		printf("%02x", mac_digest[i]);

	
	
}