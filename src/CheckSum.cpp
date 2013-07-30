//
// Copyright (C) 2013  Ioan Chera
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
////////////////////////////////////////////////////////////////////////////////
//
// MODULE FOR VARIOUS CHECKSUM CALCULATORS
// MD5 data calculator
//
////////////////////////////////////////////////////////////////////////////////

#include "wl_def.h"
#include "i_system.h"
#include "CheckSum.h"

CheckSum mapsegsChecksum;

//
// MD5Input
//
// Message prepared for MD5 input (padded and ended)
//
// used by the MD5 calculator
//
class MD5Input
{
    uint8_t *controlledMessage;
    size_t sizepadded;
public:
    MD5Input(const void *message, size_t messize)
    {
        // put this value at the end
        uint64_t sizelarge = (uint64_t)messize;
        // this will be the operated size of the message
        // must have room left for 8 + 1 byte (9)
        sizepadded = messize / 64 * 64 + (messize % 64 > 55 ? 128 : 64);
        controlledMessage = (uint8_t *)I_CheckedMalloc(sizepadded);
        memset(controlledMessage, 0, sizepadded);
        
        memcpy(controlledMessage, message, messize);
        // the 1-bit terminator
        memcpy(controlledMessage + messize, "\x80", sizeof(uint8_t));
        // the file size
        memcpy(controlledMessage + sizepadded - sizeof(uint64_t), &sizelarge,
               sizeof(uint64_t));
    }
    ~MD5Input()
    {
        free(controlledMessage);
    }
    
    inline const uint8_t *ControlledMessage() const
    {
        return this->controlledMessage;
    }
    inline size_t SizePadded() const {return this->sizepadded;}
};

//
// leftRotate
//
// Does the rotation as described in the MD5 documentation
//
inline static uint32_t leftRotate(uint32_t x, uint32_t c)
{
	return (x << c) | (x >> (32 - c));
}

//
// CalculateMD5Checksum
//
// Calculate MD5 hash of
//
// Algorithm from http://en.wikipedia.org/wiki/MD5
//
// WARNING! dest must be allocated with 16 bytes
//
void CheckSum::CalculateMD5Checksum(const void *message,
                                 size_t messize)
{
    MD5Input input(message, messize);
	
	// Note: All variables are unsigned 32 bit and wrap modulo 2^32 when
    // calculating
	
	// r specifies the per-round shift amounts
	uint32_t r[64] =
	{7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
	5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
	4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
		6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
	
	
	// (Or just use the following table):
	uint32_t k[64] =
	{
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };
	
	// Initialize variables:
	uint32_t h0 = 0x67452301;   //A
	uint32_t h1 = 0xefcdab89;   //B
	uint32_t h2 = 0x98badcfe;   //C
	uint32_t h3 = 0x10325476;   //D
	
	uint32_t w[16];
	for(size_t chunkpos = 0; chunkpos < input.SizePadded(); chunkpos += 64)
	{
		for(size_t wordpos = 0; wordpos < 16; ++wordpos)
		{
			memcpy(&w[wordpos], input.ControlledMessage() + chunkpos +
                   wordpos * 2, sizeof(uint32_t));
		}
		uint32_t a = h0, b = h1, c = h2, d = h3, f, g, temp;
		for(uint32_t i = 0; i < 64; ++i)
		{
			if(i <= 15)
			{
				f = (b & c) | (~b & d);
				g = i;
			}
			else if(16 <= i && i <= 31)
			{
				f = (d & b) | (~d & c);
				g = (5 * i + 1) % 16;
			}
			else if(32 <= i && i <= 47)
			{
				f = b ^ c ^ d;
				g = (3 * i + 5) % 16;
			}
			else if(48 <= i && i <= 63)
			{
				f = c ^ (b | ~d);
				g = (7 * i) % 16;
			}
			temp = d;
			d = c;
			c = b;
			b = b + leftRotate(a + f + k[i] + w[g], r[i]);
			a = temp;
		}
		
		h0 += a;
		h1 += b;
		h2 += c;
		h3 += d;
	}
	
	// put it in little endian!!

	digeststring[0]  = (uint8_t)( h0        % 256);
	digeststring[1]  = (uint8_t)((h0 >>  8) % 256);
	digeststring[2]  = (uint8_t)((h0 >> 16) % 256);
	digeststring[3]  = (uint8_t)((h0 >> 24));
	
	digeststring[4]  = (uint8_t)( h1        % 256);
	digeststring[5]  = (uint8_t)((h1 >>  8) % 256);
	digeststring[6]  = (uint8_t)((h1 >> 16) % 256);
	digeststring[7]  = (uint8_t)((h1 >> 24));
	
	digeststring[8]  = (uint8_t)( h2        % 256);
	digeststring[9]  = (uint8_t)((h2 >>  8) % 256);
	digeststring[10] = (uint8_t)((h2 >> 16) % 256);
	digeststring[11] = (uint8_t)((h2 >> 24));
	
	digeststring[12] = (uint8_t)( h3        % 256);
	digeststring[13] = (uint8_t)((h3 >>  8) % 256);
	digeststring[14] = (uint8_t)((h3 >> 16) % 256);
	digeststring[15] = (uint8_t)((h3 >> 24));
}

//
// byteToReadableHex
//
// Converts a single byte into 2 ASCII characters of hex number, lower case
//
static const char *byteToReadableHex(uint8_t b)
{
    static char ret[3];
    snprintf(ret, 3, "%02x", b);
    return ret;
}

//
// GetHumanReadableHex
//
// Obtains the human-readable sequence of hex numbers, in lower-case (as 
// returned by the md5 command-line executable)
//
void CheckSum::GetHumanReadableHex(PString &dest) const
{
    dest.clear();
    for (int i = 0; i < 16; ++i)
    {
        dest.concat(byteToReadableHex(digeststring[i]), 2);
    }
}

//
// unfoldedMapSegs
//
// Helper function to unfold mapsegs
//
static const size_t mapSegsUnitLength = sizeof(word) * maparea;
static const size_t mapSegsLength = MAPPLANES * mapSegsUnitLength;
inline static const uint8_t *unfoldedMapSegs()
{
    static uint8_t mapSegsLinear[mapSegsLength];
    
    for(size_t i = 0; i < MAPPLANES; ++i)
    {
        memcpy(mapSegsLinear + i * mapSegsUnitLength, mapsegs[i],
               mapSegsUnitLength);
    }
    
    return mapSegsLinear;
}

//
// CalculateMapsegsChecksum
//
// Do the save map to file checksum thingie
//
void CalculateMapsegsChecksum()
{
    mapsegsChecksum.CalculateMD5Checksum(unfoldedMapSegs(), mapSegsLength);
    
}
