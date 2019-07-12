// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           directinput.cpp - DirectInput functions module              *  
// *************************************************************************

#define INITGUID
#include <dinput.h>
#include <stdio.h>

#include "revenant.h"
#include "mainwnd.h"
#include "directinput.h"

LPDIRECTINPUT DirectInput = NULL;

// *******************************
// * DirectInput Error Functions *
// *******************************

#define TRY_DI(exp) { { HRESULT rval = exp; if (rval != DI_OK) { TraceErrorDI(rval, __FILE__, __LINE__); return FALSE; } } }
  //Global Direct Draw error handler 

struct DIERRORSTRING
{
    HRESULT Error;
    char *lpszErrorStr;
};

static DIERRORSTRING dierrors[] =
{
  // DirectInput errors...  
    {DI_NOTATTACHED, "DI_NOTATTACHED"},
    {DI_BUFFEROVERFLOW, "DI_BUFFEROVERFLOW"},
    {DI_PROPNOEFFECT, "DI_PROPNOEFFECT"},
    {DI_NOEFFECT, "DI_NOEFFECT"},
    {DI_POLLEDDEVICE, "DI_POLLEDDEVICE"},
    {DI_DOWNLOADSKIPPED, "DI_DOWNLOADSKIPPED"},
    {DI_EFFECTRESTARTED, "DI_EFFECTRESTARTED"},
    {DI_TRUNCATED, "DI_TRUNCATED"},
    {DI_TRUNCATEDANDRESTARTED, "DI_TRUNCATEDANDRESTARTED"},
    {DIERR_OLDDIRECTINPUTVERSION, "DIERR_OLDDIRECTINPUTVERSION"},
    {DIERR_BETADIRECTINPUTVERSION, "DIERR_BETADIRECTINPUTVERSION"},
    {DIERR_BADDRIVERVER, "DIERR_BADDRIVERVER"},
    {DIERR_DEVICENOTREG, "DIERR_DEVICENOTREG"},
    {DIERR_NOTFOUND, "DIERR_NOTFOUND"},
    {DIERR_OBJECTNOTFOUND, "DIERR_OBJECTNOTFOUND"},
    {DIERR_INVALIDPARAM, "DIERR_INVALIDPARAM"},
    {DIERR_NOINTERFACE, "DIERR_NOINTERFACE"},
    {DIERR_GENERIC, "DIERR_GENERIC"},
    {DIERR_OUTOFMEMORY, "DIERR_OUTOFMEMORY"},
    {DIERR_UNSUPPORTED, "DIERR_UNSUPPORTED"},
    {DIERR_NOTINITIALIZED, "DIERR_NOTINITIALIZED"},
    {DIERR_ALREADYINITIALIZED, "DIERR_ALREADYINITIALIZED"},
    {DIERR_NOAGGREGATION, "DIERR_NOAGGREGATION"},
    {DIERR_OTHERAPPHASPRIO, "DIERR_OTHERAPPHASPRIO"},
    {DIERR_INPUTLOST, "DIERR_INPUTLOST"},
    {DIERR_ACQUIRED, "DIERR_ACQUIRED"},
    {DIERR_NOTACQUIRED, "DIERR_NOTACQUIRED"},
    {DIERR_READONLY, "DIERR_READONLY"},
    {DIERR_HANDLEEXISTS, "DIERR_HANDLEEXISTS"},
    {DIERR_INSUFFICIENTPRIVS, "DIERR_INSUFFICIENTPRIVS"},
    {DIERR_DEVICEFULL, "DIERR_DEVICEFULL"},
    {DIERR_MOREDATA, "DIERR_MOREDATA"},
    {DIERR_NOTDOWNLOADED, "DIERR_NOTDOWNLOADED"},
    {DIERR_HASEFFECTS, "DIERR_HASEFFECTS"},
    {DIERR_NOTEXCLUSIVEACQUIRED, "DIERR_NOTEXCLUSIVEACQUIRED"},
    {DIERR_INCOMPLETEEFFECT, "DIERR_INCOMPLETEEFFECT"},
    {DIERR_NOTBUFFERED, "DIERR_NOTBUFFERED"},
    {DIERR_EFFECTPLAYING, "DIERR_EFFECTPLAYING"},
};
#define NUMDIERRORS sizearray(dierrors)

 // Global DirectInput Error function
void TraceErrorDI(HRESULT Err, char *file, int line)
{       
    char *dierr;
    char err[1024];

    for (int c = 0; c < NUMDIERRORS; c++)
    {
        if (Err == dierrors[c].Error)
        {
            dierr = dierrors[c].lpszErrorStr; 
            break;
        }
    }
    if (c >= NUMDIERRORS)
        sprintf(dierr, "Unknown Error"); 

    sprintf(err, "DirectX Error %s\nin file %s at line %d", dierr, file, line);
    FatalError(err);
}

// *************************
// * DirectInput Functions *
// *************************

BOOL InitializeDirectInput()
{
    if (DirectInput != NULL)
        return TRUE;

   // create the DirectInput 5.0 interface object
    DWORD err = DirectInputCreate(hInstance, DIRECTINPUT_VERSION, &DirectInput, NULL);
    if (err != DI_OK)
    {
        _RPT0(_CRT_WARN, "DIRECTINPUT: Unable to initialize DirectInput");
        DirectInput = NULL;
        return FALSE;
    }

   return TRUE;
}

void CloseDirectInput()
{
    if (!DirectInput)
        return;

    CloseJoysticks();
    DirectInput->Release();

    DirectInput = NULL;
}

// **********************
// * Joystick Functions *
// **********************

#define MAXDBLTAPTICKS 16

_STRUCTDEF(SJoystick)
struct SJoystick
{
    LPDIRECTINPUTDEVICE2 joydev;
    DWORD states[MAXDBLTAPTICKS];
    DWORD changed[MAXDBLTAPTICKS];
    int curstate;
    DWORD laststate;
    DWORD lastdblstate;
};

static int numjoysticks = 0;
static SJoystick joysticks[MAXJOYSTICKS];

// Called to set word length properties on joystick device
HRESULT SetWordProperty(LPDIRECTINPUTDEVICE2 pdev, REFGUID guidProperty,
                   DWORD dwObject, DWORD dwHow, DWORD dwValue)
{
   DIPROPDWORD dipdw;

   dipdw.diph.dwSize       = sizeof(dipdw);
   dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
   dipdw.diph.dwObj        = dwObject;
   dipdw.diph.dwHow        = dwHow;
   dipdw.dwData            = dwValue;

   return pdev->SetProperty(guidProperty, &dipdw.diph);

}

// Called by joystick init enum function to create joystick devices
BOOL FAR PASCAL EnumJoystickFunc(LPCDIDEVICEINSTANCE pdinst, LPVOID)
{
   // create the DirectInput joystick device
    LPDIRECTINPUTDEVICE joydev;
    LPDIRECTINPUTDEVICE2 joydev2;
    TRY_DI(DirectInput->CreateDevice(pdinst->guidInstance, &joydev, NULL));
    TRY_DI(joydev->QueryInterface(IID_IDirectInputDevice2, (LPVOID *)&joydev2));
    TRY_DI(joydev2->SetDataFormat(&c_dfDIJoystick));
    TRY_DI(joydev2->SetCooperativeLevel(MainWindow.Hwnd(), 
        DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));

    // set X-axis range to (-1000 ... +1000)
    // This lets us test against 0 to see which way the stick is pointed.

    DIPROPRANGE diprg;
    diprg.diph.dwSize       = sizeof(diprg);
    diprg.diph.dwHeaderSize = sizeof(diprg.diph);
    diprg.diph.dwObj        = DIJOFS_X;
    diprg.diph.dwHow        = DIPH_BYOFFSET;
    diprg.lMin              = -1000;
    diprg.lMax              = +1000;

    TRY_DI(joydev2->SetProperty(DIPROP_RANGE, &diprg.diph));

    //
    // And again for Y-axis range
    //
    diprg.diph.dwObj        = DIJOFS_Y;

    TRY_DI(joydev2->SetProperty(DIPROP_RANGE, &diprg.diph));

    // set X axis dead zone to 50% (to avoid accidental turning)
    // Units are ten thousandths, so 50% = 5000/10000.

    TRY_DI(SetWordProperty(joydev2, DIPROP_DEADZONE, DIJOFS_X, DIPH_BYOFFSET, 5000));

    // set Y axis dead zone to 50% (to avoid accidental thrust)
    // Units are ten thousandths, so 50% = 5000/10000.
    TRY_DI(SetWordProperty(joydev2, DIPROP_DEADZONE, DIJOFS_Y, DIPH_BYOFFSET, 5000));

    // Start getting input!
    TRY_DI(joydev2->Acquire());

    joysticks[numjoysticks].joydev = joydev2;
    joysticks[numjoysticks].laststate = 0;
    numjoysticks++;
 
    if (numjoysticks < MAXJOYSTICKS)
        return DIENUM_CONTINUE;
    else
        return FALSE;
}

// Initializes joystick devices
BOOL InitializeJoysticks()
{
    if (!DirectInput)
        return FALSE;

    TRY_DI(DirectInput->EnumDevices(DIDEVTYPE_JOYSTICK,
        EnumJoystickFunc, NULL, DIEDFL_ATTACHEDONLY));

    return numjoysticks > 0;
}

void CloseJoysticks()
{
    if (numjoysticks <= 0)
        return;

    for (int c = 0; c < numjoysticks; c++)
    {
        joysticks[c].joydev->Unacquire();
        joysticks[c].joydev->Release();
        joysticks[c].joydev = NULL;
        joysticks[c].curstate = 0;
        for (int d = 0; d < DoubleTapTicks; d++)
        {
            joysticks[c].states[d] = 0;
            joysticks[c].changed[d] = 0;
        }
        joysticks[c].laststate = 0;
        joysticks[c].lastdblstate = 0;
    }

    numjoysticks = 0;
}

int NumJoysticks()
{
    return numjoysticks;
}

void GetJoystickState(int joynum, DWORD *state, DWORD *changed, DWORD *dblstate, DWORD *dblchanged)
{
   HRESULT hRes;
   DIJOYSTATE js;

    if (numjoysticks <= 0)
    {
        *state = *changed = 0;
        return;
    }

  // Make sure DoubleTapTicks is a sane number
    if (DoubleTapTicks < 0)
        DoubleTapTicks = 0;
    else if (DoubleTapTicks > MAXDBLTAPTICKS)
        DoubleTapTicks = MAXDBLTAPTICKS;

  // poll the joystick to read the current state
   hRes = joysticks[joynum].joydev->Poll();

   // get data from the joystick
   hRes = joysticks[joynum].joydev->GetDeviceState(sizeof(DIJOYSTATE), &js);

   if(hRes != DI_OK)
   {
      // did the read fail because we lost input for some reason?
      // if so, then attempt to reacquire.  If the second acquire
      // fails, then the error from GetDeviceData will be
      // DIERR_NOTACQUIRED, so we won't get stuck an infinite loop.
      if(hRes == DIERR_INPUTLOST)
         joysticks[joynum].joydev->Acquire();

      // get data from the joystick
      hRes = joysticks[joynum].joydev->GetDeviceState(sizeof(DIJOYSTATE), &js);
      if (hRes != DI_OK)
      {
        *state = joysticks[joynum].laststate;
        *changed = 0;
        return;
      }
   }

  // Ok, setup button stuff
   DWORD buttons = 0;
   DWORD lastbuttons = joysticks[joynum].laststate;

  // Convert joystick state to button flags
   if (js.lX < 0 && js.lY < 0)
      buttons |= JOYSTICK_UPLEFT;
   else if (js.lX == 0 && js.lY < 0)
      buttons |= JOYSTICK_UP;
   else if (js.lX > 0 && js.lY < 0)
      buttons |= JOYSTICK_UPRIGHT;
   else if (js.lX < 0 && js.lY == 0)
      buttons |= JOYSTICK_LEFT;
   else if (js.lX > 0 && js.lY == 0)
      buttons |= JOYSTICK_RIGHT;
   else if (js.lX < 0 && js.lY > 0)
      buttons |= JOYSTICK_DOWNLEFT;
   else if (js.lX == 0 && js.lY > 0)
      buttons |= JOYSTICK_DOWN;
   else if (js.lX > 0 && js.lY > 0)
      buttons |= JOYSTICK_DOWNRIGHT;

   for (int c = 0; c < 24; c++)
   {
        if (js.rgbButtons[c] & 0x80)
            buttons |= (JOYSTICK_BUTTON1 << c);
   }

    if (buttons != lastbuttons)
        buttons = buttons * 1;

 // Figure out what's changed since last call
   DWORD changebuttons = lastbuttons ^ buttons;

 // Get double tap buttons states stuff so we can proces double taps
   DWORD lastdblbuttons = joysticks[joynum].lastdblstate;
   DWORD dblbuttons = lastdblbuttons;
 
 // Now go through double tap saved button states, and check for double taps
   DWORD btn = 1;
   for (c = 0; c < 32; c++, btn <<= 1)
   {
      // Button is still down, is unchanged, and were doing a dblbutton for it...
        if ((btn & buttons) && (btn && lastdblbuttons))
        {  
            dblbuttons |= btn;
            buttons &= ~btn;
        }

     // Button was just pressed down, check for a new double button press...
        else if ((btn & changebuttons) && (btn & buttons) && !(btn & lastdblbuttons))
        {
            for (int d = 0; d < DoubleTapTicks; d++)
            {
              // If same button was pressed down recently, this is a double tap...
                if (btn & joysticks[joynum].states[d] & joysticks[joynum].changed[d])
                {
                    dblbuttons |= btn;
                    buttons &= ~btn;
                    break;
                }
            }
        }
        
      // Button is no longer down, clear all double button press...
        else if (!(btn && buttons) && (btn & lastdblbuttons))
        {
            dblbuttons &= ~btn;
        }
    }

  // Get doubletap changed buttons
    DWORD dblchangebuttons = dblbuttons ^ lastdblbuttons;

  // Save current button state, so we can use it later...
    joysticks[joynum].states[joysticks[joynum].curstate] = buttons;
    joysticks[joynum].changed[joysticks[joynum].curstate] = changebuttons;
    joysticks[joynum].curstate++;
    if (joysticks[joynum].curstate >= DoubleTapTicks)
        joysticks[joynum].curstate = 0;

  // Normal buttons
    *state = buttons;
    *changed = changebuttons;

  // Now just set the dbltap buttons
    *dblstate = dblbuttons;
    *dblchanged = dblchangebuttons;
  
  // Ok.. save the previous states of the buttons so we can detect when they change
    joysticks[joynum].laststate = buttons;       // Normal buttons
    joysticks[joynum].lastdblstate = dblbuttons; // Double taps
}

// Generates VK compatible codes given the values in state, and changed.  The code returned
// from the previous call must be passed in 'code' in order to get the next code, or code
// must be -1 when the funciton is first called.  The function returns 'FALSE' when there
// are no more codes.
BOOL GetJoystickKeyCode(DWORD state, DWORD changed, DWORD dblstate, DWORD dblchanged, int &code, BOOL &down)
{
    if (code < VK_JOYFIRST || code > VK_JOYLAST)
        code = VK_JOYFIRST - 1;

    code++;

    if (state)
        state = state * 1;
    
    while (code <= VK_JOYLAST)
    {
        if (code < (VK_JOYFIRST | VK_DBLTAP))
        {
          // Do normal codes
            if (changed & (1 << (code - VK_JOYFIRST)))
            {
                down = (state & (1 << (code - VK_JOYFIRST))) != 0;
                return TRUE;
            }
        }
        else
        {
          // Do doubletap codes
            if (dblchanged & (1 << (code - (VK_JOYFIRST | VK_DBLTAP))))
            {
                down = (dblstate & (1 << (code - (VK_JOYFIRST | VK_DBLTAP)))) != 0;
                return TRUE;
            }
        }

        code++;
    }

    code = -1;
    down = FALSE;

    return FALSE;
}
