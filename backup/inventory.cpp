// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  Inventory.cpp - TInvetory object                     *
// *************************************************************************

#include <stdlib.h>

#include "revenant.h"
#include "inventory.h"
#include "object.h"
#include "mappane.h"
#include "display.h"
#include "cursor.h"
#include "player.h"
#include "equip.h"
#include "font.h"
#include "money.h"
#include "animation.h"
#include "textbar.h"

BOOL TInventory::Initialize()
{
    grabslot = heldslot = -1;
    isdragging = FALSE;

    SetDirty(TRUE);

    return TRUE;
}

void TInventory::Close()
{
}

void TInventory::DrawBackground()
{
    if (container && IsDirty())
    {
        Display->Box(INVENTORYCONTX, INVENTORYCONTY,
            INVITEMREALHEIGHT, INVITEMREALWIDTH, 0, 0xffff, 0, DM_BACKGROUND);
        container->DrawInvItem(INVENTORYCONTX, INVENTORYCONTY);

        for (int y = 0; y < INVITEMSY; y++)
            for (int x = 0; x < INVITEMSX; x++)
                Display->Box((x * INVITEMHEIGHT) + INVENTORYSTARTX,
                    (y * INVITEMWIDTH) + INVENTORYSTARTY, INVITEMREALHEIGHT,
                    INVITEMREALWIDTH, 0, 0xffff, 0, DM_BACKGROUND);

        for (TInventoryIterator i(container); i; i++)
        {
            PTObjectInstance oi = i.Item();

            if ((!isdragging || oi->InventNum() != grabslot) && oi->InventNum() < 256)
            {
                if (oi->InventNum() < 0)
                {
                    oi->SetInventNum(container->FindFreeInventorySlot());
                    TextBar.Print("Bad inventory slot for %s", oi->GetName());
                }

                int x = (oi->InventNum() % INVITEMSX) * INVITEMWIDTH;
                int y = (oi->InventNum() / INVITEMSX) * INVITEMHEIGHT;
                x += INVENTORYSTARTX;
                y += INVENTORYSTARTY;

                oi->DrawInvItem(x, y);

                if (oi->Amount() > 1)
                {
                    char buf[80];
                    itoa(oi->Amount(), buf, 10);
                    Display->WriteText(buf, x, y-1, 1, GameData->Font("numbers"));
                }
            }
        }

        if (grabslot < 0)
            heldslot = -1;
        SetDirty(FALSE);
    }
}

void TInventory::Animate(BOOL draw)
{
    if (draw && container && !mousebutton)
    {
        PTObjectInstance inst = container->GetInventorySlot(OnSlot(cursorx - GetPosX(), cursory - GetPosY()));
        CursorOverObject(inst);
    }
}

void TInventory::MouseClick(int button, int x, int y)
{
    if (!container)
        return;

    if (button == MB_LEFTDOWN && InPane(x, y))
    {
        heldslot = grabslot = OnSlot(x, y);
        startposx = x;
        startposy = y;
        isdragging = FALSE;
    }
    else if (button == MB_LEFTUP)
    {
        // check to see if they are on the far left (ie, chest/bag/pack icon)
        if (x >= 0 && x <= INVENTORYSTARTX && y >= 0 && y < INVENTORYPANEHEIGHT)
        {
            if (container->GetOwner())
            {
                if (grabslot < 0)
                    SetContainer(container->GetOwner());
                else
                {
                    PTObjectInstance inst = container->GetInventorySlot(grabslot);
                    if (inst && container->GetOwner()->FindFreeInventorySlot() < MAXINVITEMS)
                    {
                        inst->RemoveFromInventory();
                        container->GetOwner()->AddToInventory(inst);
                    }
                }
            }
        }
        else if (grabslot >= 0)
        {
            // handle swaping and using objects
            int newslot = OnSlot(x, y);

            if (newslot >= 0)
            {
                PTObjectInstance inst = container->GetInventorySlot(grabslot);
                PTObjectInstance oi = container->GetInventorySlot(newslot);

                BOOL used = FALSE;

                if (inst && oi && ((isdragging && oi != inst) || (!isdragging && oi == inst)))
                {
                    if (oi->Use(GetTopContainer(), oi != inst ? inst->GetMapIndex() : -1))
                    {
                        used = TRUE;
                        Update();
                    }
                }

                if (!used)
                    SwapSlots(grabslot, newslot);
            }
        }
        else if (InPane(x, y))
        {
            // handle transfers from other panes
            if (EquipPane.GetHeldSlot() >= 0)
            {
                PTObjectInstance inst = ((PTPlayer)GetTopContainer())->GetEquip(EquipPane.GetHeldSlot());
                int newslot = OnSlot(x, y);
                if (inst && newslot >= 0)
                {
                    if (GetTopContainer()->GetInventorySlot(newslot) == NULL)
                    {
                        ((PTPlayer)GetTopContainer())->Equip(NULL, EquipPane.GetHeldSlot());    // clear from eq list
                        inst->SetInventNum(newslot);                    // add to inventory
                        Update();
                    }
                }
            }
        }

        grabslot = -1;
        SetDragBitmap(NULL);
        SetDragObj(NULL);
        isdragging = FALSE;

        if (InPane(x, y))
            Update();
    }
}

void TInventory::MouseMove(int button, int x, int y)
{
    if (!container)
        return;

    if (!isdragging && InPane(x, y))
        if (button == MB_LEFTDOWN || button == MB_RIGHTDOWN)
        {
            // don't bother switching to drag mode until they actually move it
            if (absval(startposx - x) >= 2 || absval(startposy - y) >= 2)
            {
                PTObjectInstance inst = container->GetInventorySlot(grabslot);

                if (inst && inst->InventoryImage())
                {
                    int grabx = (startposx - INVENTORYSTARTX) % INVITEMWIDTH;
                    int graby = (startposy - INVENTORYSTARTY) % INVITEMHEIGHT;

                    if (!inst->InventoryImage()->OnPixel(grabx, graby))
                        grabx = graby = 20;

                    SetDragBitmap(inst->InventoryImage(), grabx, graby);
                    SetDragObj(inst);
                }

                isdragging = TRUE;
                Update();
            }
        }
}

void TInventory::DrawAnim(PTObjectInstance inst, PTBitmap bm)
{
    if (!container || inst->GetOwner() != container)
        return;

    if (inst->InventNum() >= 256)
        EquipPane.DrawAnim(inst, bm);
    else
    {
        int x = (inst->InventNum() % INVITEMSX) * INVITEMWIDTH;
        int y = (inst->InventNum() / INVITEMSX) * INVITEMHEIGHT;
        x += INVENTORYSTARTX;
        y += INVENTORYSTARTY;

        int sx, sy, sw, sh;
        Display->GetClipRect(sx, sy, sw, sh);
        SetClipRect();
        Display->Put(x, y, bm);
        Display->SetClipRect(sx, sy, sw, sh);
    }
}

int TInventory::OnSlot(int x, int y)
{
    x -= INVENTORYSTARTX;
    y -= INVENTORYSTARTY;

    if (x < 0 || y < 0 ||
        (x / INVITEMWIDTH) >= INVITEMSX || (y / INVITEMHEIGHT) >= INVITEMSY ||
        (x % INVITEMWIDTH) >= INVITEMREALWIDTH || (y % INVITEMHEIGHT) >= INVITEMREALHEIGHT)
        return -1;
        
    return ((y / INVITEMHEIGHT) * INVITEMSX) + (x / INVITEMWIDTH);
}

void TInventory::SwapSlots(int oldslot, int newslot)
{
    PTObjectInstance inst0 = container->GetInventorySlot(oldslot);
    PTObjectInstance inst1 = container->GetInventorySlot(newslot);

    if (inst0)
        inst0->SetInventNum(newslot);

    if (inst1)
        inst1->SetInventNum(oldslot);
}
