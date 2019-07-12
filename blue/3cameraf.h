#ifndef __CAMERAFIX__
#define __CAMERAFIX__

class C3DCamera
{
public :
    C3DCamera();
    ~C3DCamera();

    void DrawGoraudZbuffer(sAGU_GORAUD p, unsigned short zvalue);

    void DrawTextureAndModulationGoraudZbuffer(sAGU_TEXTUREANDGORAUD p, short *tmap, int tpowu, int tumask, int tvmask, unsigned short zvalue);
    void DrawTextureAndModulationGoraudZbufferWOff(sAGU_TEXTUREANDGORAUD p, short *tmap, int tpowu, int tumask, int tvmask, unsigned short zvalue);
    void DrawTextureAndModulationGoraudZbufferROff(sAGU_TEXTUREANDGORAUD p, short *tmap, int tpowu, int tumask, int tvmask, unsigned short zvalue);
    void DrawTextureAndModulationGoraud(sAGU_TEXTUREANDGORAUD p, short *tmap, int tpowu, int tumask, int tvmask);

    void DrawTextureZbuffer4444(sAGU_TEXTURE p, short *tmap, int tpowu, int tumask, int tvmask, 
                                unsigned short zvalue, unsigned short zwritevalue);
    void DrawTextureZbuffer4444WOff(sAGU_TEXTURE p, short *tmap, int tpowu, int tumask, int tvmask, 
                                       unsigned short zvalue);
    void DrawTextureZbuffer4444ROff(sAGU_TEXTURE p, short *tmap, int tpowu, int tumask, int tvmask, 
                                       unsigned short zvalue);
    void DrawTexture4444(sAGU_TEXTURE p, short *tmap, int tpowu, int tumask, int tvmask);

    void SetScreenCenter(int x, int y);
    void SetStartEnd(int startx, int endx, int starty, int endy, int screenx);
    void SetVideoBuffer(short* video, short* zbuffer);

    void SetAlphaTable(unsigned short* alphatable, int alphacount, int alphashift);
    void SetDivTable(int* divtable);
    void SetAddlightTable(unsigned int* savecarrytable, unsigned short* addlighttable);
    void Set4444Table(unsigned short* fortable);

    void InitScreen();
    void InitLens(float fov, float aspect);
    int GetCenterX() { return m_centerx; };
    int GetCenterY() { return m_centery; };
    int GetStartX() { return m_startx; };
    int GetEndX() { return m_endx; };
    int GetStartY() { return m_starty; };
    int GetEndY() { return m_endy; };

    // it will be used as internal variable.
    int         m_originx;
    int         m_originy;
    int         m_clippx;
    int         m_clippy;
    short*      pm_videoorigin;
    short*      pm_zbufferorigin;
    // argument will be saved on here.
    int         m_startx;
    int         m_endx;
    int         m_starty;
    int         m_endy;
    int         m_screenx;
    int         m_centerx;
    int         m_centery;
    short*      pm_video;
    short*      pm_zbuffer;

    unsigned int*   pm_savecarrytable;
    unsigned short* pm_addlighttable;
    int*            pm_divtable;
    unsigned short* pm_alphatable;
    int             m_alphacount;
    int             m_alphashift;
    unsigned short* pm_fortable;
};
#endif