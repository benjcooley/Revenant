#ifndef __3GARGB4444T
#define __3GARGB4444T

class C3GARGB4444Table 
{
public :
    C3GARGB4444Table();
    void MakeTable(int colorcount);
    ~C3GARGB4444Table();

    unsigned short* pm_table;
    int m_colorcount;
    int m_colorshift;

};

extern C3GARGB4444Table argb4444table;

#endif