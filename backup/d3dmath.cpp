/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: d3dmath.cpp
 *
 ***************************************************************************/

#include <d3d.h>
#include "d3dmath.h"

typedef D3DVALUE MATRIX[4][4];

static D3DMATRIX IDENTITY = {
    D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0)
};

/*
 * Normalises the vector v
 */
LPD3DVECTOR 
D3DVECTORNormalise(LPD3DVECTOR v)
{
    float vx, vy, vz, inv_mod;
    vx = v->x;
    vy = v->y;
    vz = v->z;
    if ((vx == 0) && (vy == 0) && (vz == 0))
        return v;
    inv_mod = (float)(1.0 / sqrt(vx * vx + vy * vy + vz * vz));
    v->x = vx * inv_mod;
    v->y = vy * inv_mod;
    v->z = vz * inv_mod;
    return v;
}


/*
 * Calculates cross product of a and b.
 */
LPD3DVECTOR 
D3DVECTORCrossProduct(LPD3DVECTOR lpd, LPD3DVECTOR lpa, LPD3DVECTOR lpb)
{

    lpd->x = lpa->y * lpb->z - lpa->z * lpb->y;
    lpd->y = lpa->z * lpb->x - lpa->x * lpb->z;
    lpd->z = lpa->x * lpb->y - lpa->y * lpb->x;
    return lpd;
}

/*
 * Clears matrix to the identity matrix.
 */
LPD3DMATRIX D3DMATRIXClear(LPD3DMATRIX lpMat)
{
    memcpy(lpMat, &IDENTITY, sizeof(D3DMATRIX));
    return lpMat;
}

/*
 * lpDst = lpSrc1 * lpSrc2
 * lpDst can be equal to lpSrc1 or lpSrc2
 */
LPD3DMATRIX MultiplyD3DMATRIX(LPD3DMATRIX lpDst, LPD3DMATRIX lpSrc1, 
                              LPD3DMATRIX lpSrc2)
{
    MATRIX &M1 = *(MATRIX *)lpSrc1;
    MATRIX &M2 = *(MATRIX *)lpSrc2;
    MATRIX D;
    int i, r, c;

    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            D[r][c] = (float)0.0;
            for (i = 0; i < 4; i++)
                D[r][c] += M1[r][i] * M2[i][c];
        }
    }

    memcpy(lpDst, D, sizeof(D3DMATRIX));

    return lpDst;
}

void D3DMATRIXTransform(LPD3DMATRIX lpm, LPD3DVECTOR lpV, LPD3DVECTOR lpD)
{
    D3DVECTOR v;
    v.x = lpV->x;
    v.y = lpV->y;
    v.z = lpV->z;

    lpD->x = lpm->_11 * v.x  + lpm->_21 * v.y + lpm->_31 * v.z + lpm->_41;
    lpD->y = lpm->_12 * v.x  + lpm->_22 * v.y + lpm->_32 * v.z + lpm->_42;
    lpD->z = lpm->_13 * v.x  + lpm->_23 * v.y + lpm->_33 * v.z + lpm->_43;
}

/*
 * -1 d = a
 */
LPD3DMATRIX D3DMATRIXInvert(LPD3DMATRIX d, LPD3DMATRIX a)
{
    d->_11 = a->_11;
    d->_12 = a->_21;
    d->_13 = a->_31;
    d->_14 = a->_14;

    d->_21 = a->_12;
    d->_22 = a->_22;
    d->_23 = a->_32;
    d->_24 = a->_24;

    d->_31 = a->_13;
    d->_32 = a->_23;
    d->_33 = a->_33;
    d->_34 = a->_34;

    d->_41 = a->_14;
    d->_42 = a->_24;
    d->_43 = a->_34;
    d->_44 = a->_44;

    return d;
}


/*
 * Set the rotation part of a matrix such that the vector lpD is the new
 * z-axis and lpU is the new y-axis.
 */
LPD3DMATRIX D3DMATRIXSetRotation(LPD3DMATRIX lpM, LPD3DVECTOR lpD, LPD3DVECTOR lpU)
{
    float t;
    D3DVECTOR d, u, r;

    /*
     * Normalise the direction vector.
     */
    d.x = lpD->x;
    d.y = lpD->y;
    d.z = lpD->z;
    D3DVECTORNormalise(&d);

    u.x = lpU->x;
    u.y = lpU->y;
    u.z = lpU->z;
    /*
     * Project u into the plane defined by d and normalise.
     */
    t = u.x * d.x + u.y * d.y + u.z * d.z;
    u.x -= d.x * t;
    u.y -= d.y * t;
    u.z -= d.z * t;
    D3DVECTORNormalise(&u);

    /*
     * Calculate the vector pointing along the matrix x axis (in a right
     * handed coordinate system) using cross product.
     */
    D3DVECTORCrossProduct(&r, &u, &d);

    lpM->_11 = r.x;
    lpM->_12 = r.y, lpM->_13 = r.z;
    lpM->_21 = u.x;
    lpM->_22 = u.y, lpM->_23 = u.z;
    lpM->_31 = d.x;
    lpM->_32 = d.y;
    lpM->_33 = d.z;

    return lpM;
}

/*
 * Set translates a matrix such that the vector lpV is the new
 * x, y, and z orign.
 */
LPD3DMATRIX D3DMATRIXTranslate(LPD3DMATRIX lpDst, LPD3DVECTOR lpV)
{
    MATRIX m;
    memcpy(&m, &IDENTITY, sizeof(D3DMATRIX));
    m[3][0] = lpV->x;
    m[3][1] = lpV->y;
    m[3][2] = lpV->z;
    MultiplyD3DMATRIX(lpDst, lpDst, (LPD3DMATRIX)&m);
    return lpDst;
}

/*
 * Set the translation part of a matrix such that the vector lpV is the new
 * x, y, and z orign.
 */
LPD3DMATRIX D3DMATRIXMove(LPD3DMATRIX lpDst, LPD3DVECTOR lpV)
{
    MATRIX m;
    memcpy(&m, &IDENTITY, sizeof(D3DMATRIX));
    m[3][0] = -lpV->x;
    m[3][1] = -lpV->y;
    m[3][2] = -lpV->z;
    MultiplyD3DMATRIX(lpDst, lpDst, (LPD3DMATRIX)&m);
    return lpDst;
}

/*
 * Set the scale part of a matrix such that the vector lpV is the new
 * x, y, and z orign.
 */
LPD3DMATRIX D3DMATRIXScale(LPD3DMATRIX lpDst, LPD3DVECTOR lpV)
{
    MATRIX m;
    memcpy(&m, &IDENTITY, sizeof(D3DMATRIX));
    m[0][0] = lpV->x;
    m[1][1] = lpV->y;
    m[2][2] = lpV->z;
    MultiplyD3DMATRIX(lpDst, lpDst, (LPD3DMATRIX)&m);
    return lpDst;
}

/*
 * Set the X rotation components of a matrix given the angle in radians.
 */
LPD3DMATRIX D3DMATRIXRotateX(LPD3DMATRIX lpDst, D3DVALUE a)
{
    float c = (float)cos((double)a);
    float s = (float)sin((double)a);
    MATRIX m;
    memcpy(&m, &IDENTITY, sizeof(D3DMATRIX));
    m[1][1] = c;
    m[1][2] = s;
    m[2][1] = -s;
    m[2][2] = c;
    MultiplyD3DMATRIX(lpDst, lpDst, (LPD3DMATRIX)&m);
    return lpDst;
}

/*
 * Set the Y rotation components of a matrix given the angle in radians.
 */
LPD3DMATRIX D3DMATRIXRotateY(LPD3DMATRIX lpDst, D3DVALUE a)
{
    float c = (float)cos((double)a);
    float s = (float)sin((double)a);
    MATRIX m;
    memcpy(&m, &IDENTITY, sizeof(D3DMATRIX));
    m[0][0] = c;
    m[0][2] = -s;
    m[2][0] = s;
    m[2][2] = c;
    MultiplyD3DMATRIX(lpDst, lpDst, (LPD3DMATRIX)&m);
    return lpDst;
}

/*
 * Set the Z rotation components of a matrix given the angle in radians.
 */
LPD3DMATRIX D3DMATRIXRotateZ(LPD3DMATRIX lpDst, D3DVALUE a)
{
    float c = (float)cos((double)a);
    float s = (float)sin((double)a);
    MATRIX m;
    memcpy(&m, &IDENTITY, sizeof(D3DMATRIX));
    m[0][0] = c;
    m[0][1] = s;
    m[1][0] = -s;
    m[1][1] = c;
    MultiplyD3DMATRIX(lpDst, lpDst, (LPD3DMATRIX)&m);
    return lpDst;
}

/*
 * Calculates a point along a B-Spline curve defined by four points. p
 * n output, contain the point. t                                Position
 * along the curve between p2 and p3.  This position is a float between 0
 * and 1. p1, p2, p3, p4    Points defining spline curve. p, at parameter
 * t along the spline curve
 */
void 
spline(LPD3DVECTOR p, float t, LPD3DVECTOR p1, LPD3DVECTOR p2,
       LPD3DVECTOR p3, LPD3DVECTOR p4)
{
    double t2, t3;
    float m1, m2, m3, m4;

    t2 = (double)(t * t);
    t3 = t2 * (double)t;

    m1 = (float)((-1.0 * t3) + (2.0 * t2) + (-1.0 * (double)t));
    m2 = (float)((3.0 * t3) + (-5.0 * t2) + (0.0 * (double)t) + 2.0);
    m3 = (float)((-3.0 * t3) + (4.0 * t2) + (1.0 * (double)t));
    m4 = (float)((1.0 * t3) + (-1.0 * t2) + (0.0 * (double)t));

    m1 /= (float)2.0;
    m2 /= (float)2.0;
    m3 /= (float)2.0;
    m4 /= (float)2.0;

    p->x = p1->x * m1 + p2->x * m2 + p3->x * m3 + p4->x * m4;
    p->y = p1->y * m1 + p2->y * m2 + p3->y * m3 + p4->y * m4;
    p->z = p1->z * m1 + p2->z * m2 + p3->z * m3 + p4->z * m4;
}

