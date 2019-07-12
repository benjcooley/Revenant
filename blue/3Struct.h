#ifndef __3STRUCT
#define __3STRUCT

#include "3define.h"

// use model data of 3d

struct sPOINT {
    int x, y, z, temp;
};

struct sVERTEX {
    float x, y, z, temp;
};

struct sVERTEXfix {
    int x, y, z, temp;
};

struct sPOLYGON {
    long    ver1;
    long    ver2;
    long    ver3;
    long    temp;
};

struct sUV {
    long u, v;
};

struct sTEXTURE {
    sUV ver1;
    sUV ver2;
    sUV ver3;
    sUV temp;
};

// use transfrom

struct sVECTOR {
    float x, y, z, temp;
};

struct sVECTORfix {
    int x, y, z, temp;
};

struct sMATRIX {
    float xx, xy, xz;
    float yx, yy, yz;
    float zx, zy, zz;
    float mx, my, mz;
};

struct sMATRIXfix {
    int xx, xy, xz;
    int yx, yy, yz;
    int zx, zy, zz;
    int mx, my, mz;
};

struct sQUATERNION {
    float x, y, z, w;
    float mx, my, mz;
};

#define d_MATERIALHEADER    (d_NAMESIZE+d_FILENAMESIZE*3+sizeof(long)*10)

struct s3DMATERIAL
{
    char        name[d_NAMESIZE];
    long        ambient;
    long        diffuse;
    long        specular;
    float       shininess;
    float       shinstr;
    float       alpha;
    float       selfillumin;
    float       txtpercent;
    char        txtfname[d_FILENAMESIZE];
    float       alppercent;
    char        alpfname[d_FILENAMESIZE];
    float       bmppercent;
    char        bmpfname[d_FILENAMESIZE];

    long        tpowu, tpowv, tsizeu, tsizev;
    char*       txtmap;
    long        apowu, apowv, asizeu, asizev;
    char*       alpmap;
    long        bpowu, bpowv, bsizeu, bsizev;
    char*       bmpmap;
    char*       colort;
};

#define d_CLUMPHEADER   (d_NAMESIZE+sizeof(sMATRIX)+sizeof(long)*4+sizeof(sVERTEX))

struct s3DCLUMP {
    char            name[d_NAMESIZE];
// vertex.
    long            numver;
    long            numpol;
// clump was initlized this matrix.
    sMATRIX         clpmat;
// position
    sVERTEX         average;
    long            attribute;
    long            materialindex;

/////////////////// this part same file.

    sVERTEX*        vertex;
    sVECTOR*        vvector;
// polygon.

    sPOLYGON*       polygon;
    sVECTOR*        pvector;
// texture.
    sTEXTURE*       texture;
// buffer.
    sVERTEX*        verbuff; // buffer size = numver * sizeof(sVERTEX);
    sVECTORfix*     vecbuff; // buffer size = numver * sizeof(sVERTEX);
    int*            averagebuff; // buffer size = numpol * 4byte;
    int*            sortbuff;
    s3DMATERIAL*    mtl;

};

struct  s3DANIMATION 
{

};

#define d_OBJECTHEADER  (d_NAMESIZE+16)

struct  s3DOBJECT {
    char            name[d_NAMESIZE];
    long            numclp;
    long            numver;
    long            numpol;
    long            nummtl;
    s3DCLUMP**  Pclump;
    s3DMATERIAL**   Pmaterial;
};


// use agumunt

struct sAGU_FLAT_VER {
    int x, y;
};
struct sAGU_FLAT {
    sAGU_FLAT_VER   v1, v2, v3; 
};

struct sAGU_GORAUD_VER 
{
    int x, y;
    unsigned int c;
};
struct sAGU_GORAUD 
{
    sAGU_GORAUD_VER     v1, v2, v3; 
};

struct sAGU_TEXTURE_VER 
{
    int x, y, u, v;
};

struct sAGU_TEXTURE 
{
    sAGU_TEXTURE_VER    v1, v2, v3; 
};

struct sAGU_TWOTEXTURE_VER 
{
    int x, y, u1, v1, u2, v2;
};

struct sAGU_TWOTEXTURE 
{
    sAGU_TWOTEXTURE_VER v1, v2, v3; 
};

struct sAGU_TEXTURE3D_VER
{
    int x, y, z, u, v;
};

struct sAGU_TEXTURE3D
{
    sAGU_TEXTURE3D_VER  v1, v2, v3; 
};

struct sAGU_TEXTUREANDGORAUD_VER 
{
    int x, y, u, v, c;
};
struct sAGU_TEXTUREANDGORAUD 
{
    sAGU_TEXTUREANDGORAUD_VER   v1, v2, v3;
};

struct sAGU_QUADTEXTURE 
{
    sAGU_TEXTURE_VER    v1, v2, v3, v4;
};

#endif