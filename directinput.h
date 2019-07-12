// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             directinput.h  - DirectInput Include File                 *
// *************************************************************************

#ifndef _DIRECTINPUT_H
#define _DIRECTINPUT_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

BOOL InitializeDirectInput();
  // Initializes direct input (ignore BOOL, never returns if error)
void CloseDirectInput();
  // Closes direct input object

// **********************
// * Joystick interface *
// **********************

#define JOYSTICK_UPLEFT         0x00000001
#define JOYSTICK_UP             0x00000002
#define JOYSTICK_UPRIGHT        0x00000004
#define JOYSTICK_LEFT           0x00000008
#define JOYSTICK_RIGHT          0x00000010
#define JOYSTICK_DOWNLEFT       0x00000020
#define JOYSTICK_DOWN           0x00000040
#define JOYSTICK_DOWNRIGHT      0x00000080
#define JOYSTICK_BUTTON1        0x00000100
#define JOYSTICK_BUTTON2        0x00000200
#define JOYSTICK_BUTTON3        0x00000400
#define JOYSTICK_BUTTON4        0x00000800
#define JOYSTICK_BUTTON5        0x00001000
#define JOYSTICK_BUTTON6        0x00002000
#define JOYSTICK_BUTTON7        0x00004000
#define JOYSTICK_BUTTON8        0x00008000
#define JOYSTICK_BUTTON9        0x00010000
#define JOYSTICK_BUTTON10       0x00020000
#define JOYSTICK_BUTTON11       0x00040000
#define JOYSTICK_BUTTON12       0x00080000
#define JOYSTICK_BUTTON13       0x00100000
#define JOYSTICK_BUTTON14       0x00200000
#define JOYSTICK_BUTTON15       0x00400000
#define JOYSTICK_BUTTON16       0x00800000
#define JOYSTICK_BUTTON17       0x01000000
#define JOYSTICK_BUTTON18       0x02000000
#define JOYSTICK_BUTTON19       0x04000000
#define JOYSTICK_BUTTON20       0x08000000
#define JOYSTICK_BUTTON21       0x10000000
#define JOYSTICK_BUTTON22       0x20000000
#define JOYSTICK_BUTTON23       0x40000000
#define JOYSTICK_BUTTON24       0x80000000

// Gravis codes
#define JOYSTICK_R          0x00000100
#define JOYSTICK_Y          0x00000200
#define JOYSTICK_G          0x00000400
#define JOYSTICK_B          0x00000800
#define JOYSTICK_L1         0x00001000
#define JOYSTICK_R1         0x00002000
#define JOYSTICK_L2         0x00004000
#define JOYSTICK_R2         0x00008000
#define JOYSTICK_SELECT     0x00010000
#define JOYSTICK_START      0x00020000

// Virtual key code ids
#define VK_JOYUPLEFT        (1024 + 0)
#define VK_JOYUP            (1024 + 1)
#define VK_JOYUPRIGHT       (1024 + 2)
#define VK_JOYLEFT          (1024 + 3)
#define VK_JOYRIGHT         (1024 + 4)
#define VK_JOYDOWNLEFT      (1024 + 5)
#define VK_JOYDOWN          (1024 + 6)
#define VK_JOYDOWNRIGHT     (1024 + 7)
#define VK_JOYBUTTON1       (1024 + 8)
#define VK_JOYBUTTON2       (1024 + 9)
#define VK_JOYBUTTON3       (1024 + 10)
#define VK_JOYBUTTON4       (1024 + 11)
#define VK_JOYBUTTON5       (1024 + 12)
#define VK_JOYBUTTON6       (1024 + 13)
#define VK_JOYBUTTON7       (1024 + 14)
#define VK_JOYBUTTON8       (1024 + 15)
#define VK_JOYBUTTON9       (1024 + 16)
#define VK_JOYBUTTON10      (1024 + 17)
#define VK_JOYBUTTON11      (1024 + 18)
#define VK_JOYBUTTON12      (1024 + 19)
#define VK_JOYBUTTON13      (1024 + 20)
#define VK_JOYBUTTON14      (1024 + 21)
#define VK_JOYBUTTON15      (1024 + 22)
#define VK_JOYBUTTON16      (1024 + 23)
#define VK_JOYBUTTON17      (1024 + 24)
#define VK_JOYBUTTON18      (1024 + 25)
#define VK_JOYBUTTON19      (1024 + 26)
#define VK_JOYBUTTON20      (1024 + 27)
#define VK_JOYBUTTON21      (1024 + 28)
#define VK_JOYBUTTON22      (1024 + 29)
#define VK_JOYBUTTON23      (1024 + 30)
#define VK_JOYBUTTON24      (1024 + 31)

// ORed with VK key codes to indicate double tap
#define VK_DBLTAP           (32)

#define VK_JOYFIRST     VK_JOYUPLEFT
#define VK_JOYLAST      (VK_JOYBUTTON24 | VK_DBLTAP)

#define MAXJOYSTICKS 4

BOOL InitializeJoysticks();
  // Initializes any joysticks and returns TRUE if any found
void CloseJoysticks();
  // Initializes any joysticks and returns TRUE if any found
int NumJoysticks();
  // Returns number of joysticks
void GetJoystickState(int joynum, DWORD *state, DWORD *changed, DWORD *dblstate, DWORD *dblchanged);
  // Returns the current state flags of the joystick buttons in 'state', and
  // flags for which buttons changed since last call in 'changed'
BOOL GetJoystickKeyCode(DWORD state, DWORD changed, DWORD dblstate, DWORD dblchanged, int &code, BOOL &down);
  // Generates VK compatible codes given the values in state, and changed.  The code returned
  // from the previous call must be passed in 'code' in order to get the next code, or code
  // must be -1 when the funciton is first called.  The function returns 'FALSE' when there
  // are no more codes.

#endif
