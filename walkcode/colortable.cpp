// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                colortable.cpp - Color table objects                   *
// *************************************************************************

#include <math.h>
#include "display.h"

#include "revenant.h"
#include "dls.h"

// Lighting
static double maxbrightness   = 50.0;           // Maximum brightness
static double attrange        = 0;              // (real intensity always cut to 63)
static double attmin          = 256 - attrange; // Minimum attenuation position
static double exponent        = -1.1;           // power of exponential curve
static double multiplierscale = 20.0;           // Scale of multipliers

// Color Table
static double MonoPercent = 0;
static int AmbientLight;
static SColor AmbientColor = { 255, 255, 255 };
static double AmbientMultiplier = 1.0;
static SColor LightColors[NUMBASELIGHTS] =
    { {255, 255, 255}, {10, 10, 255}, {255, 250, 20}, {80, 0, 200} };
static int LightMultipliers[NUMBASELIGHTS] = { 28, 28, 28, 28 };
static int LightUseCount[NUMBASELIGHTS] = { 0, 0, 0, 0 };

BYTE  DistTable[256][256];
BYTE  AngleTable[256][256];
short DistX[256];                   // Quick rotation lookups for facing system
short DistY[256];
BYTE  CosCos[64*256];
BYTE  IntCos[256*256];
BYTE  SinSin[64*256];
BYTE  LightDropOffTable[32768];

// Note, this buffer is pretty large, but not all of it is actually used
// However, the bottom 5 bits of the table lookups are used, so its garanteed that
// the table has data in at least 32 byte chunks, which incidently fit perfectly into
// a single cache entry.  Isn't that nice!
WORD LightTableBuffer[256 * 256 + 32]; // One 64k buffer plus 32 extra bytes for cache alignment
WORD *LightTable;

BYTE  ColorTable[32 * 256];
BYTE  ColorTable16[32 * 256];
BYTE  ColorTableUpperHi[32 * 256];
BYTE  ColorTableUpperLo[32 * 256];
BYTE  ColorTableLower[32 * 256];
DWORD Conv16to32Upper[128];
DWORD Conv16to32Lower[256];
BYTE  IntensityTableUpper[256];
BYTE  IntensityTableLower[256];
double BrightnessTable[256];
DWORD MMXLightTable[256];

// rounding function to avoid normal truncation on typecasting
int Round(double d)
{
    int i = (int)d;
    if ((d - (double)i) >= 0.5)
        i++;

    return i;
}

void SetAmbientLight(int ambient)
{
    if (ambient != -1)
        AmbientLight = ambient;

  // Reset light table colors (must change tables when AmbientLight value changes)
    for (int i = 0; i < NUMBASELIGHTS; i++)
        if (i == 0 || LightUseCount[i] > 0)
            SetLightColor(i, LightColors[i]);
}

int GetAmbientLight()
{
    return AmbientLight;
}

void SetAmbientColor(RSColor color)
{
    AmbientColor.red = color.red;
    AmbientColor.green = color.green;
    AmbientColor.blue = color.blue;

    for (int i = 0; i < NUMBASELIGHTS; i++)
        if (i == 0 || LightUseCount[i] > 0)
            SetLightColor(i, LightColors[i]);
}

void GetAmbientColor(RSColor color)
{
    color.red = AmbientColor.red;
    color.green = AmbientColor.green;
    color.blue = AmbientColor.blue;
}

double GetLightBrightness(int dist, int intensity, int multiplier)
{
    intensity = min(254, max(intensity, 1));
    dist = max(0, dist) * 254 / intensity;
    if (dist > 254)
        return 0.0;
    else
        return BrightnessTable[dist] * (double)multiplier / (multiplierscale / 2);
}

void SetMonoPercent(int pcnt)
{
    return;     // it's not working right now so don't bother

    int loop;

    if (pcnt < 0)
        pcnt = 0;
    else if (pcnt > 100)
        pcnt = 100;
        
   // Color tables for 16 to 32 bit conversion
    MonoPercent = (double)pcnt / 100.0;
    
    if (Display->BitsPerPixel() == 15)
    {
        for(loop = 0; loop < 256; loop++)
        {
            BYTE red   = ((loop & 124) >> 2) << 3;
            BYTE green = ((loop & 3) << 3) << 3;
            BYTE blue = 0;
            BYTE rm = (BYTE)(double)((double)red * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);
            BYTE gm = (BYTE)(double)((double)green * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);
            BYTE bm = (BYTE)(double)((double)blue * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);
                        
            Conv16to32Upper[loop] = (DWORD)(((DWORD)rm << 0) |
                 ((DWORD)gm << 8) | ((DWORD)bm << 16));
        }

        for(loop = 0; loop < 256; loop++)
        {
            BYTE red = 0;
            BYTE green = ((loop & 224) >> 5) << 3;
            BYTE blue  = (loop & 31) << 3;
            BYTE rm = (BYTE)(double)((double)red * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);
            BYTE gm = (BYTE)(double)((double)green * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);
            BYTE bm = (BYTE)(double)((double)blue * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);

            Conv16to32Lower[loop] = (DWORD)(((DWORD)rm << 0) |
                 ((DWORD)gm << 8) | ((DWORD)bm << 16));
        }
    }
    else
    {
        for(loop = 0; loop < 128; loop++)
        {
            BYTE red   = ((loop & 0xF8) >> 3) << 3;
            BYTE green = ((loop & 0x07) << 3) << 2;
            BYTE blue = 0;
            BYTE rm = (BYTE)(double)((double)red * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);
            BYTE gm = (BYTE)(double)((double)green * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);
            BYTE bm = (BYTE)(double)((double)blue * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);

            Conv16to32Upper[loop] = (DWORD)(((DWORD)rm << 0) |
                ((DWORD)gm << 8) | ((DWORD)bm << 16));
        }

        for(loop = 0; loop < 256; loop++)
        {
            BYTE red = 0;
            BYTE green = ((loop & 0xE0) >> 5) << 2;
            BYTE blue  = (loop & 0x01F) << 3;
            BYTE rm = (BYTE)(double)((double)red * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);
            BYTE gm = (BYTE)(double)((double)green * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);
            BYTE bm = (BYTE)(double)((double)blue * (1.0 - MonoPercent) + 
                ((double)red + (double)green + (double)blue) * MonoPercent / 3.0);

            Conv16to32Lower[loop] = (DWORD)(((DWORD)rm << 0) |
                ((DWORD)gm << 8) | ((DWORD)bm << 16));
        }
    }
}

int GetMonoPercent()
{
    return (int)(MonoPercent * 100.0);
}

// Move this number down to increase light accuracy, up to increase onscreen light quantity
#define TOLERANCE       3

int NewLightIndex(RSColor color, int mult)
{
  // Match color with lighting colors   
    int bestid = 0;
    int bestdiff = 255 + 255 + 255 + 255;
    for (int i = 0; i < 4; i++)
    {
        if (i == 0 || LightUseCount[i] > 0)
        {
            int diff = abs(color.red - LightColors[i].red) + 
                abs(color.green - LightColors[i].green) +
                abs(color.blue - LightColors[i].blue) +
                (mult > 0 ? abs(mult - LightMultipliers[i]) : 0);
            if (diff < bestdiff)
            {
                bestid = i;
                bestdiff = diff;
            }
        }
    }

    int id;
    for (id = 1; id < NUMBASELIGHTS; id++)
        if (LightUseCount[id] < 1)
            break;

    if (bestdiff < TOLERANCE || id >= NUMBASELIGHTS)
    {
        LightUseCount[bestid]++;
        return bestid;
    }

    SetLightColor(id, color, mult > 0 ? mult : 28);
    LightUseCount[id] = 1;

    return id;
}

void FreeLightIndex(int id)
{
    LightUseCount[id]--;
}

void ClearLights()
{
    for (int i = 1; i < NUMBASELIGHTS; i++)
        LightUseCount[i] = 0;
}

void SetLightColor(int id, RSColor color, int mult)
{
    if ((DWORD)id >= NUMBASELIGHTS)
        return;

    if (mult >= 0)
        LightMultipliers[id] = mult;
    double multiplier = (double)LightMultipliers[id] / multiplierscale;

    double ared = (double)AmbientColor.red / 255.0;
    double agreen = (double)AmbientColor.green / 255.0;
    double ablue = (double)AmbientColor.blue / 255.0;
    double am = 1.0 / max(ared, max(agreen, ablue));
    ared = ared * am;
    agreen = agreen * am;
    ablue = ablue * am;

    double lred = (double)color.red / 255.0;
    double lgreen = (double)color.green / 255.0;
    double lblue = (double)color.blue / 255.0;
    double lm = 1.0 / max(lred, max(lgreen, lblue));
    lred = lred * lm;
    lgreen = lgreen * lm;
    lblue = lblue * lm;

    LightColors[id] = color;

    BYTE red, green, blue;

    for(int loop = 0; loop < 64; loop++)
    {
        double aintensity = (double)AmbientLight / 255.0;
        double intensity = (1.0 - aintensity) * ((double)loop / 63.0);

        if (intensity > 1.0) 
            intensity = 1.0;

        int mmxclr = 80;

        red = (BYTE)min((double)mmxclr * ared * aintensity * AmbientMultiplier, 255.0);
        red = (BYTE)min((double)mmxclr * lred * intensity * multiplier + (double)red, 255.0);
        green = (BYTE)min((double)mmxclr * agreen * aintensity * AmbientMultiplier, 255.0);
        green = (BYTE)min((double)mmxclr * lgreen * intensity * multiplier + (double)green, 255.0);
        blue = (BYTE)min((double)mmxclr * ablue * aintensity * AmbientMultiplier, 255.0);
        blue = (BYTE)min((double)mmxclr * lblue * intensity * multiplier + (double)blue, 255.0);

        MMXLightTable[(loop * 4) + id] = ((DWORD)blue << 16) | ((DWORD)green << 8) | ((DWORD)red);

        for(int loop2 = 0; loop2 < 32; loop2++)
        {
            int clr = loop2 << 3;

            WORD color = 0;

            red = (BYTE)min((double)clr * ared * aintensity * AmbientMultiplier, 31.0);
            red = (BYTE)min((double)clr * lred * intensity * multiplier + (double)red, 31.0);

            if (Display->BitsPerPixel() == 16)
            {
                green = (BYTE)min((double)(clr << 1) * agreen * aintensity *
                    AmbientMultiplier, 63.0);
                green = (BYTE)min((double)(clr << 1) * lgreen * intensity * 
                                  multiplier + (double)green, 63.0);

                color |= (WORD)(red << 11);
            }

            else
            {
                green = (BYTE)min((double)clr * agreen * aintensity * 
                    AmbientMultiplier, 31.0);
                green = (BYTE)min((double)clr * lgreen * intensity * 
                                  multiplier + (double)green, 31.0);

                color |= (WORD)(red << 10);
            }

            blue = (BYTE)min((double)clr * ablue * aintensity * AmbientMultiplier, 31.0);
            blue = (BYTE)min((double)clr * lblue * intensity * multiplier + (double)blue, 31.0);

            color |= (WORD)(green << 5);
            color |= (WORD)blue;

            LightTable[(loop * 4 * 256) + (id * 256) + loop2] = color;
        }
    }
}

void GetLightColor(int id, RSColor color, int &mult)
{
    if ((DWORD)id >= NUMBASELIGHTS)
        return;
    color = LightColors[id];
    mult = LightMultipliers[id];
}

void MakeColorTables()
{

  // Round up light table pointer to nearest 32 (so 32 color values fit in one cache entry)
    LightTable = (WORD *)(((DWORD)LightTableBuffer + 31) & 0xFFFFFFE0);

  // Make Falloff Table 
    double minpower = pow(256.0, exponent);
    double maxpower = pow(1.0, exponent) - minpower;
    for (int dist = 0; dist < 256; dist++)
    {
        double p = pow((double)(dist + 1.0), exponent) - minpower;
        double brightness = min(p / maxpower * maxbrightness, 1.0);

        if (dist > attmin)
            brightness = brightness * max((attrange - ((double)dist - attmin)) / 
                         attrange, 0);

        BrightnessTable[dist] = brightness;
    }

  // Make alpha channel conversion table here.. 

    int red, green, blue;

    for (int loop = 0; loop < 32; loop++)
    {
        for(int loop2 = 0; loop2 < 256; loop2++)
        {

            if (Display->BitsPerPixel() == 16)
            {
                red   = (loop2 & 0xf8) >> 3;
                red   = red * loop / 31;
                green = (loop2 & 0x07) << 3;
                green = green * loop / 31;
                ColorTableUpperHi[loop * 256 + loop2] = (BYTE)((red << 3) 
                                                      | (green >> 3));
                IntensityTableUpper[loop2] = red + green; // Green is twice red
            }
            
            else
            {
                red   = (loop2 & 0x7C) >> 2;
                red   = red * loop / 31;
                green = (loop2 & 0x03) << 3;
                green = green * loop / 31;
                ColorTableUpperHi[loop * 256 + loop2] = (BYTE)((red << 2) 
                                                      | (green >> 3));
                IntensityTableUpper[loop2] = red + (green * 2); // Green is twice red

            }

            ColorTableUpperLo[loop * 256 + loop2] = (BYTE)((green & 7) << 5);
         }
    }

    for (loop = 0; loop < 32; loop++)
    {
        for(int loop2 = 0; loop2 < 256; loop2++)
        {
            green = (loop2 & 0xE0) >> 5;
            green = green * loop / 31;
            blue  = (loop2 & 0x1F);
            blue  = blue * loop / 31;

            ColorTableLower[loop * 256 + loop2] = (BYTE)((green << 5) | blue);

            IntensityTableLower[loop2] = (green * 2) + 0; //(blue / 3);
                // Blue is 1/3rd red
        }
    }
    
    BYTE *index = IntCos;   

    for (short y = 0; y < 256; y++)
    {
        for(short x = 0; x < 256; x++)
        {
            signed char looppos = (signed char)y;
            double a1           = (double)x / 256.0 * M_2PI;
            double brightness   = (double)sin(a1) * (double)looppos/127.0 * 31.0;

            *(index++)   = (BYTE)brightness;

            dist     = Round(sqrt((double)x * (double)x + (double)y * (double)y));
            BYTE ang;
            if (x == 0)
                ang = 64;               // otherwise it cuts off a bit short
            else
                ang = (BYTE)Round((atan2l(y, x) / M_2PI) * 256.0);

            if (dist >= 255)
                DistTable[x][y] = (BYTE)255;
            else
                DistTable[x][y] = dist;
            
            AngleTable[x][y]    = ang;
        }
    }

    // make this as explicit as possible to avoid any precision errors
    for (loop = 0; loop < 64; loop++)
    {
        if (loop == 0)
        {
            DistX[0] = 0;
            DistY[0] = 256;
        }
        else
        {
            double angle = (loop * M_2PI) / 256.0;
            DistX[loop] = (short) Round(256.0 * sin(angle));
            if (loop == 32)
                DistY[32] = DistX[32];
            else
                DistY[loop] = (short) Round(256.0 * cos(angle));
        }
        
    }
    // and just copy to the other three quadrants with appropriate sign changes
    for (loop = 64; loop < 128; loop++)
    {
        DistX[loop] = DistY[loop - 64];
        DistY[loop] = -(DistX[loop - 64]);
    }
    for (loop = 128; loop < (128+64); loop++)
    {
        DistX[loop] = -(DistX[loop - 128]);
        DistY[loop] = -(DistY[loop - 128]);
    }
    for (loop = (128+64); loop < 256; loop++)
    {
        DistX[loop] = -(DistX[loop - 128]);
        DistY[loop] = -(DistY[loop - 128]);
    }

    signed char *coscosidx = (signed char *)CosCos;   
    index = SinSin;   
    
    for (loop = 0; loop < 64; loop++)
    {
        for (int loop2 = 0; loop2 < 256; loop2++)
        {
            signed char loop2pos = (signed char)loop2;

            double b2    = (double)loop / 128.0 * M_2PI;
            double delta = (double)loop2pos / 256.0 * M_2PI;
            double a2    = (double)loop2 / 256.0 * M_2PI;
    
            double brightness = sin(b2) * cos(delta) * 127.0;
            *(coscosidx++)    = (signed char)brightness;

            brightness = cos(b2) * cos(a2) * 31.0;
            *(index++) = (BYTE)brightness;
        }
    }


    for(dist = 0; dist < 256; dist++)
    {
        int brightness = (int)(BrightnessTable[dist] * 63.0);
    
        LightDropOffTable[dist] = (BYTE)min((int)brightness, 0x63) << 2;
    }
    
    for(loop = 0; loop < 32; loop++)
    {
        for(int loop2 = 0; loop2 < 256; loop2++)
        {
            red = (BYTE)((float)(loop2) * ((float)(loop) / 31.0));
            ColorTable[loop * 256 + loop2] = (BYTE)red;
        }
    }

  // Set Initial Light Colors
    SColor color;
    color.red = color.green = color.blue = 255;
    SetAmbientColor(color);                     // also inits all lights

  // Set 16 to 32 bit conversion table
    SetMonoPercent(0);
}

