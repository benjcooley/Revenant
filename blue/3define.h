#ifndef __3DEFINE
#define __3DEFINE

#define RT_FLAT             0x00000010
#define RT_GORAUD           0x00000020
#define RT_TEXTURE          0x00000040  
#define RT_VECTORMAP        0x00000080
#define RT_WATER            0x00000100
#define RT_QUADSPRITE       0x00000200
#define RT_PERSPECTIVE      0x00000400
#define ST_MIDDLE           0x00000001
#define ST_FIRST            0x00000002
#define ST_LAST             0x00000003
#define ST_ZBUFFER          0x00000004

#define d_FLOATEND  (float)3.402823e38
#define d_INTEND    (int)0x7fffffff
#define d_FIXEDP    8
#define d_FIXEDPC   256
#define d_NAMESIZE  16
#define d_FILENAMESIZE 64
#define d_TESTINVIEW 0x80000000

#define d_DIVCUT 256
#define d_DIVCMP 255
#define d_DIVSHL 9

#define d_DIVROOT 65536/4+1

#define d_ALPHALVL 32
#define d_PIXELIMIT 8192

#define d_RATIO         256

#define d_DEFAULT       0xcdcdcdcd

#define NID_VVECTOR     0x0001
#define NID_PVECTOR     0x0002
#define NID_TEXTURE     0x0004
#define NID_VECTORMAP   0x0008
#define NID_FIXED       0x0010
#define NID_ANIME       0x0020
#define NID_PORTAL      0x0040

#define OBJ_MAXCLP      1024    // max count clump in object
#define OBJ_MAXMTR      1024    // max count materiar in object
#define OBJ_MAXMAPPING  1024    // max count mapping tile

#define PAI 3.1415926535897932384f
#define NST 1024     //number of sine'table.

#define LT_LEVEL 64
#define LT_POW   6

#endif