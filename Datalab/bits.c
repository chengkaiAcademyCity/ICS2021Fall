/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 * Kai Cheng 2000013172
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* We do not support C11 <threads.h>.  */
/*
 * bitNot - ~x without using ~
 *   Example: bitNot(0) = 0xffffffff
 *   Legal ops: ! & ^ | + << >>
 *   Max ops: 12
 *   Rating: 1
 */
int bitNot(int x)
{
    /* To make every bit of a number reversed. */
    return x ^ ((0xff << 24) + (0xff << 16) + (0xff << 8) + 0xff);
}
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y)
{
    /* To implement Xor. */
    return (~(x & y)) & (~((~x) & (~y)));
}
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x)
{
    /* To return 1 if all odd-numbered bits in word set to 1. */
    return !(~(x | (0x55 + (0x55 << 8) + (0x55 << 16) + (0x55 << 24))));
}
/*
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x18765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 3
 */
int rotateRight(int x, int n)
{
    /* Rotate x to the right by n. */
    return (x >> n) & (~((~0) << (33 + (~n)))) | (x << (33 + (~n)));
}
/*
 * palindrome - return 1 if x is palindrome in binary form,
 *   return 0 otherwise
 *   A number is palindrome if it is the same when reversed
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   Example: palindrome(0xff0000ff) = 1,
 *            palindrome(0xff00ff00) = 0
 *   Legal ops: ~ ! | ^ & << >> +
 *   Max ops: 40
 *   Rating: 4

 */
int palindrome(int x)
{
    /*
    return !(x^((((((((((((((((((x<<16)+((x>>16)&0xffff))&0xff00ff00)>>8)&0xffffff)+((((x<<16)+((x>>16)&0xffff))&0xff00ff)<<8)
    )&0xf0f0f0f0)>>4)&0xfffffff)+((((((((x<<16)+((x>>16)&0xffff))&0xff00ff00)>>8)&0xffffff)+((((x<<16)+((x>>16)&0xffff))&0xff00ff)<<8)
    )&0xf0f0f0f)<<4))&0xcccccccc)>>2)&0x3fffffff)+((((((((((((x<<16)+((x>>16)&0xffff))&0xff00ff00)>>8)&0xffffff)+((((x<<16)+((x>>16)&0xffff))&0xff00ff)<<8)
    )&0xf0f0f0f0)>>4)&0xfffffff)+((((((((x<<16)+((x>>16)&0xffff))&0xff00ff00)>>8)&0xffffff)+((((x<<16)+((x>>16)&0xffff))&0xff00ff)<<8)
    )&0xf0f0f0f)<<4))&0x33333333)<<2))&0xaaaaaaaa)>>1)&0x7fffffff)+((((((((((((((((x<<16)+((x>>16)&0xffff))&0xff00ff00)>>8)&0xffffff)+((((x<<16)+((x>>16)&0xffff))&0xff00ff)<<8)
    )&0xf0f0f0f0)>>4)&0xfffffff)+((((((((x<<16)+((x>>16)&0xffff))&0xff00ff00)>>8)&0xffffff)+((((x<<16)+((x>>16)&0xffff))&0xff00ff)<<8)
    )&0xf0f0f0f)<<4))&0xcccccccc)>>2)&0x3fffffff)+((((((((((((x<<16)+((x>>16)&0xffff))&0xff00ff00)>>8)&0xffffff)+((((x<<16)+((x>>16)&0xffff))&0xff00ff)<<8)
    )&0xf0f0f0f0)>>4)&0xfffffff)+((((((((x<<16)+((x>>16)&0xffff))&0xff00ff00)>>8)&0xffffff)+((((x<<16)+((x>>16)&0xffff))&0xff00ff)<<8)
    )&0xf0f0f0f)<<4))&0x33333333)<<2))&0x55555555)<<1)));
    */

    int x1 = (x << 16) + ((x >> 16) & 0xffff);
    int x2 = (((x1 & 0xff00ff00) >> 8) & 0xffffff) + ((x1 & 0xff00ff) << 8);
    int x3 = (((x2 & 0xf0f0f0f0) >> 4) & 0xfffffff) + ((x2 & 0xf0f0f0f) << 4);
    int x4 = (((x3 & 0xcccccccc) >> 2) & 0x3fffffff) + ((x3 & 0x33333333) << 2);
    int x5 = (((x4 & 0xaaaaaaaa) >> 1) & 0x7fffffff) + ((x4 & 0x55555555) << 1);
    return !(x ^ x5);
}
/*
 * countConsecutive1 - return the number of consecutive 1‘s
 *   in the binary form of x
 *   Examples: countConsecutive1(0xff00ff00) = 2,
 *             countConsecutive1(0xff10ff10) = 4,
 *             countConsecutive1(0xff80ff80) = 2,
 *             countConsecutive1(0b00101010001111110101110101110101) = 10
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Rating: 4
 */
int countConsecutive1(int x)
{
    /* To return the number of consecutive 1's. */
    // int y=x&(x<<1);
    int x0 = x ^ (x & (x << 1));
    int a = 0xaa;
    int c = 0xcc;
    int f0 = 0xf0;
    int ff = 0xff;
    int a1 = (a << 16) + a;
    int aa = (a1 << 8) + a1;
    int c1 = (c << 16) + c;
    int cc = (c1 << 8) + c1;
    int f01 = (f0 << 16) + f0;
    int f0f0 = (f01 << 8) + f01;
    int f4 = (ff << 16) + ff;
    int f5 = (ff << 8) + ff;
    int x1 = (((x0 & aa) >> 1) & (~(1 << 31))) + (x0 & (~aa));
    int x2 = (((x1 & cc) >> 2) & (~(3 << 30))) + (x1 & (~cc));
    int x3 = ((x2 & f0f0) >> 4) + (x2 & (~f0f0));
    int x4 = ((x3 & (~f4)) >> 8) + (x3 & f4);
    int x5 = ((x4 & (~f5)) >> 16) + (x4 & f5);
    // int y1=(((y&0xaaaaaaaa)>>1)&0x7fffffff)+(y&0x55555555);
    // int y2=(((y1&0xcccccccc)>>2)&0x3fffffff)+(y1&0x33333333);
    // int y3=((y2&0xf0f0f0f0)>>4)+(y2&0xf0f0f0f);
    // int y4=((y3&0xff00ff00)>>8)+(y3&0xff00ff);
    // int y5=((y4&0xffff0000)>>16)+(y4&0xffff);
    return x5;
}
/*
 * counter1To5 - return 1 + x if x < 5, return 1 otherwise, we ensure that 1<=x<=5
 *   Examples counter1To5(2) = 3,  counter1To5(5) = 1
 *   Legal ops: ~ & ! | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int counter1To5(int x)
{
    /* To return 1 + x if x < 5, return 1 otherwise. */
    int y = x + 3;
    int z = !(y >> 3);
    int w = z | (z << 1) | (z << 2) | (z << 3);
    return (w & x) + 1;
}
/*
 * fullSub - 4-bits sub using bit-wise operations only.
 *   (0 <= x, y < 16)
 *   Example: fullSub(12, 7) = 0x5,
 *            fullSub(7, 8) = 0xf,
 *   Legal ops: ~ | ^ & << >>
 *   Max ops: 35
 *   Rating: 2
 */
int fullSub(int x, int y)
{
    int y0 = ~y;
    int x1 = x ^ y0;
    int y1 = (x & y0) << 1;
    int x2 = x1 ^ y1;
    int y2 = (x1 & y1) << 1;
    int x3 = x2 ^ y2;
    int y3 = (x2 & y2) << 1;
    int x4 = x3 ^ y3;
    int y4 = (x3 & y3) << 1;
    int x5 = x4 | y4;
    int xx1 = x5 ^ 1;
    int yy1 = (x5 & 1) << 1;
    int xx2 = xx1 ^ yy1;
    int yy2 = (xx1 & yy1) << 1;
    int xx3 = xx2 ^ yy2;
    int yy3 = (xx2 & yy2) << 1;
    int xx4 = xx3 ^ yy3;
    int yy4 = (xx3 & yy3) << 1;
    int xx5 = xx4 | yy4;
    return xx5 & 0xf;
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y)
{
    // return (!((((y)>>1)+1+(~(x>>1)))>>31))&((!!(~(y+1+(~x))))&(!!(y^((~0)^(1<<31)))));
    return (!!(((y ^ x) >> 31) & (x >> 31))) | ((!((y ^ x) >> 31)) & (!((((y)) + 1 + (~(x))) >> 31)));
}
/*
 * sm2tc - Convert from sign-magnitude to two's complement
 *   where the MSB is the sign bit
 *   Example: sm2tc(0x80000005) = -5.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int sm2tc(int x)
{
    /* Convert from sign-magnitude to two's complement. */
    int m = ((~0) ^ (1 << 31));
    int n = x & m;
    int d = (x ^ n) >> 31;
    return (d & ((~n) + 1)) | ((~d) & n);
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y)
{
    /* To add two numbers but when positive overflow occurs, returns maximum possible value, and when negative overflow occurs, it returns minimum positive value. */
    int min = 1 << 31;
    int z0 = x + y;
    int z = z0 | (((!(x ^ (min))) & (!(y ^ (min)))) << 31);
    int rz = ((!z)) + 1 + (~1);
    int w = ((~(x ^ y)) & (z ^ x)) >> 31;
    // int f=((!!(z))<<31)>>31;
    int r = ((x) >> 31); //&f;
    int check = w & rz;
    return (z & (~w)) | ((check & (r & ((min))))) | ((check & ((~r) & (~(min)))));
}
/*
 * trueFiveEighths - multiplies by 5/8 rounding toward 0,
 *  avoiding errors due to overflow
 *  Examples: trueFiveEighths(11) = 6
 *            trueFiveEighths(-9) = -5
 *            trueFiveEighths(0x30000000) = 0x1E000000 (no overflow)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 4
 */
int trueFiveEighths(int x)
{
    /* To multiply by 5/8 rounding toward 0. */
    // int f0=(~0)<<16;
    // // int xr=(x&(~f0));//+((!(!(x>>31)))<<1);
    // int xl=x&f0;
    // // int tr=(xr<<6);
    // int rr=(tr+(tr<<2))>>3;//+((((!!(x>>31))<<1)+(!!(x>>31)))<<6))>>3;
    // int tl=(xl>>6);
    // int ll=(tl+(tl<<2))>>3;
    // int rx=1+(~x);
    // // int f0=(~0)<<16;
    // int rxr=(rx&(~f0));//+((!(!(x>>31)))<<1);
    // int rxl=rx&f0;
    // int rtr=(rxr<<6);
    // int rrr=(rtr+(rtr<<2))>>3;//+((((!!(x>>31))<<1)+(!!(x>>31)))<<6))>>3;
    // int rtl=(rxl>>6);
    // int rll=(rtl+(rtl<<2))>>3;
    // // return ((~(!(~x)+0))<<31)&(
    // // return
    // int re=(ll<<6)+(rr>>6);
    // int rre=1+(~((rll<<6)+(rrr>>6)));
    // if(x==-2147483648)return -1342177280;
    // return (((x&(1<<31))>>31)&rre)|((~((x&(1<<31))>>31))&re);//+!(~x);

    int mid = x >> 31;
    int did = mid & 1;
    int rx = ((mid) ^ x) + did;
    int f0 = (0xff); //<<28;
    // int xl=(x&f0)>>6;
    // int xr=x&(~f0);
    // int tl=(xl+(xl<<2))>>3;
    // int re=((xr+(xr<<2))>>3)+(tl<<6);
    int rxl = (rx & (~f0)) >> 5;
    int rxr = rx & (f0);
    int rtl = (rxl + (rxl << 2)) << 2;
    int rre = ((rxr + (rxr << 2)) >> 3) + (rtl);
    int rrre = ((mid) ^ rre) + did;
    int flag = ((!(x << 1)) << 31) >> 31;
    return ((~flag) & rrre) | (flag & (rre));
}
/*
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf)
{
    /* Return bit-level equivalent of expression 2*f for floating point argument f. */
    unsigned s = uf & 0x80000000;
    unsigned exp = (uf & 0x7f800000);
    unsigned nexp;
    if (!exp)
        return (uf << 1) | s;
    else
    {
        if (!(exp ^ (0x7f800000)))
        {
            if (!((uf - s) ^ 0x7f800000))
                return uf;
            else
                return uf;
        }
        else
        {
            nexp = (exp + (1 << 23));
            if (!(nexp ^ 0x7f800000))
                return s | nexp;
            else
                return (uf - exp) | nexp;
        }
    }
}
/*
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf)
{
    /* Return bit-level equivalent of expression 0.5*f for floating point argument f. */
    unsigned s = uf & 0x80000000;
    unsigned unsf = uf - s;
    unsigned exp = (uf & 0x7f800000);
    unsigned nexp;
    if ((!exp) || (!(exp ^ (0x00800000))))
    {
        unsigned tmp = (unsf >> 1) | s;
        unsigned odd = (tmp & 1) && (uf & 1);
        if (odd)
            return tmp + 1;
        else
            return tmp;
    }
    else
    {
        if (!(exp ^ (0x7f800000)))
        {
            if (!((unsf) ^ 0x7f800000))
                return uf;
            else
                return uf;
        }
        else
        {
            nexp = (exp - (1 << 23));
            return (uf - exp) | nexp;
        }
    }
}
/*
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf)
{
    /* Return bit-level equivalent of expression (int) f for floating point argument f. */
    unsigned s = uf & 0x80000000;
    // unsigned unsf=uf-s;
    int exp = (uf & 0x7f800000);
    int frac = uf & 0x7fffff;
    int unsi, dig;
    // if(uf==0x3f800000)return 1;
    // if(uf==0xbf800000)return -1;
    if (!exp)
    {
        return 0;
    }
    else
    {
        if (!(exp ^ (0x7f800000)))
            return 0x80000000u;
        else
        {
            dig = (exp >> 23) - 150;
            if (dig >= 8)
                return 0x80000000u;
            else if (dig > 0)
                unsi = (frac + 0x800000) << (dig); // cout<<unsi<<endl;
            else
            {
                dig = -dig;
                if (dig >= 24)
                    return 0;
                else
                {
                    unsi = (frac + 0x800000) >> (dig);
                }
            }
            if (s)
                return 1 + (~unsi);
            else
                return unsi;
        }
    }
}
/*
 * float_pwr2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_pwr2(int x)
{
    /* Return bit-level equivalent of the expression 2.0^x for any 32-bit integer x. */
    int exp = x + 127;
    int delta = 1 - exp;
    if (exp >= 255 || x >= 255)
        return 0x7f800000;
    else if (exp > 0)
        return exp << 23;
    else
    {
        if (delta <= 23)
            return 0x800000 >> delta;
        else
            return 0;
    }
}