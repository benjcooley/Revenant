#ifndef __3GCOLORT
#define __3GCOLORT

class C3GColorTable 
{
public :
	C3GColorTable();
	void MakeTable(int colorcount);
	~C3GColorTable();

	unsigned short* pm_colortable;
	int m_colorcount;
	int m_colorshift;

};

extern C3GColorTable colortable;

#endif