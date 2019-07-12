// ****************************************
// * TMultiData - Multiple resource array *
// ****************************************

#ifndef _MULTIDAT_H
#define _MULTIDAT_H

#define MAXMULTIOFFSETS 256

_CLASSDEF(TMultiData)
class TMultiData
{
  public:
    int numoffsets;                     // Number of resource offsets
    OFFSET names[MAXMULTIOFFSETS];      // Array of offsets to resource names
    OFFSET offsets[MAXMULTIOFFSETS];    // Array of offsets to resources
};

#endif

