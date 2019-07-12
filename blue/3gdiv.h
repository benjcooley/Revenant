#ifndef __3GDIV
#define __3GDIV

class C3GDivTable
{
public :
    C3GDivTable();
    void MakeTable(int colorcount);
    ~C3GDivTable();

    int* pm_divtable;

};

extern C3GDivTable divtable;

#endif