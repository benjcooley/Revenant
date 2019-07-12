/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: d3dmath.h
 *
 ***************************************************************************/
#ifndef __D3DMATH_H__
#define __D3DMATH_H__

#include <math.h>

/*
 * Normalises the vector v
 */
LPD3DVECTOR D3DVECTORNormalise(LPD3DVECTOR v);

/*
 * Calculates cross product of a and b.
 */
LPD3DVECTOR D3DVECTORCrossProduct(LPD3DVECTOR lpd, LPD3DVECTOR lpa, LPD3DVECTOR lpb);

/*
 * Clears matrix to the identity matrix.
 */
LPD3DMATRIX D3DMATRIXClear(LPD3DMATRIX d);

/*
 * lpDst = lpSrc1 * lpSrc2
 * lpDst can be equal to lpSrc1 or lpSrc2
 */
LPD3DMATRIX MultiplyD3DMATRIX(LPD3DMATRIX lpDst, LPD3DMATRIX lpSrc1, 
                              LPD3DMATRIX lpSrc2);


/*
 * Transform 3D vector by matrix
 */
void D3DMATRIXTransform(LPD3DMATRIX lpm, LPD3DVECTOR lpV, LPD3DVECTOR lpD);

/*
 * -1 d = a
 */
LPD3DMATRIX D3DMATRIXInvert(LPD3DMATRIX d, LPD3DMATRIX a);

/*
 * Set the rotation part of a matrix such that the vector lpD is the new
 * z-axis and lpU is the new y-axis.
 */
LPD3DMATRIX D3DMATRIXSetRotation(LPD3DMATRIX lpM, LPD3DVECTOR lpD, LPD3DVECTOR lpU);

/*
 * Translates points relative origin.
 */
LPD3DMATRIX D3DMATRIXTranslate(LPD3DMATRIX lpDst, LPD3DVECTOR lpV);

/*
 * Moves points relative to origin.  Inverse of translate
 * x, y, and z orign.
 */
LPD3DMATRIX D3DMATRIXMove(LPD3DMATRIX lpDst, LPD3DVECTOR lpV);

/*
 * Set the scale part of a matrix such that the vector lpV is the new
 * x, y, and z orign.
 */
LPD3DMATRIX D3DMATRIXScale(LPD3DMATRIX lpDst, LPD3DVECTOR lpV);

/*
 * Set the X rotation components of a matrix given the angle in radians.
 */
LPD3DMATRIX D3DMATRIXRotateX(LPD3DMATRIX lpDst, D3DVALUE a);

/*
 * Set the Y rotation components of a matrix given the angle in radians.
 */
LPD3DMATRIX D3DMATRIXRotateY(LPD3DMATRIX lpDst, D3DVALUE a);

/*
 * Set the Z rotation components of a matrix given the angle in radians.
 */
LPD3DMATRIX D3DMATRIXRotateZ(LPD3DMATRIX lpDst, D3DVALUE a);

/*
 * Calculates a point along a B-Spline curve defined by four points. p
 * n output, contain the point. t                                Position
 * along the curve between p2 and p3.  This position is a float between 0
 * and 1. p1, p2, p3, p4    Points defining spline curve. p, at parameter
 * t along the spline curve
 */
void spline(LPD3DVECTOR p, float t, LPD3DVECTOR p1, LPD3DVECTOR p2,
            LPD3DVECTOR p3, LPD3DVECTOR p4);


#endif // __D3DMATH_H__

