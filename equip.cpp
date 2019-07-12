// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     equip.cpp - Equipment Pane                        *
// *************************************************************************

#include "revenant.h"
#include "equip.h"
#include "bitmap.h"
#include "display.h"
#include "playscreen.h"
#include "inventory.h"
#include "mappane.h"
#include "player.h"
#include "cursor.h"
#include "multi.h"

// Location of equipment slots on eq pane
struct { int x, y; } EquipLoc[NUM_SECTS][NUM_EQ_SLOTS] =
{ { { 22, 83 }, { 64, 24 }, { 64, 66 }, { -1, -1 }, { 106, 83 }, { -1, -1 }, { -1, -1 }, { 106, 5 }, { 22, 5 }, { -1, -1 }, { -1, -1 } },
  { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { 106, 5 }, { 106, 47 }, { 22, 47 }, { -1, -1 }, { -1, -1 }, { 22, 5 }, { 64, 83 } }
};

void BtnEquipDown()
{
    EquipPane.Scroll(1);
}

void BtnEquipUp()
{
    EquipPane.Scroll(-1);
}

BOOL TEquipPane::Initialize()
{
    TButtonPane::Initialize();

    NewButton("\\/", 64, 108, 40, 15, -1, BtnEquipDown, GameData->Bitmap("equipdowndown"), GameData->Bitmap("equipdownup"));
    NewButton("/\\", 64, 5, 40, 15, -1, BtnEquipUp, GameData->Bitmap("equipupdown"), GameData->Bitmap("equipupup"));

    equipdata = TMulti::LoadMulti("equipscr.dat");

    top = equipdata->Bitmap("equipscrtop");
    bottom = equipdata->Bitmap("equipscrbottom");
    section = SECT_TOP;

    heldslot = grabslot = -1;
    SetDirty(TRUE);

    return (top != NULL) && (bottom != NULL);
}

void TEquipPane::MouseClick(int button, int x, int y)
{
    TButtonPane::MouseClick(button, x, y);

    if (!Player)
        return;

    if (button == MB_LEFTDOWN)
    {
        int slot = OnSlot(x, y);
        if (slot >= 0)
        {
            if (Player->GetEquip(slot) && Player->GetEquip(slot)->InventoryImage())
            {
                heldslot = grabslot = slot;
                SetDragBitmap(Player->GetEquip(slot)->InventoryImage(),
                                x - EquipLoc[section][slot].x, y - EquipLoc[section][slot].y);
                SetDragObj(Player->GetEquip(slot));
                Update();
            }
        }
    }
    else if (button == MB_LEFTUP)
    {
        if (grabslot >= 0)
        {
            // dragging from the equipment pane to someplace else
            grabslot = -1;
            SetDragBitmap(NULL);
            SetDragObj(NULL);
            Update();
        }
        else if (InPane(x, y))
        {
            // dragging from the inventory pane to here
            PTObjectInstance inst = Inventory.GetContainer()->GetInventorySlot(Inventory.GetHeldSlot());
            if (inst)
            {
                int slot = inst->GetStat("eqslot");
                if ((DWORD)slot < NUM_EQ_SLOTS && Player->CanEquip(inst, slot))
                {
                    if (Player->GetEquip(slot))
                    {
                        // special case: accessories can go in either left or right slot
                        if (slot == EQ_R_ACCESSORY && Player->GetEquip(EQ_L_ACCESSORY) == NULL)
                            slot = EQ_L_ACCESSORY;
                        else if (slot == EQ_L_ACCESSORY && Player->GetEquip(EQ_R_ACCESSORY) == NULL)
                                slot = EQ_R_ACCESSORY;
                    }

                    if (inst->GetOwner() == Player)
                    {
                        // something already in that slot?
                        if (Player->GetEquip(slot))
                            Player->GetEquip(slot)->SetInventNum(inst->InventNum());

                        inst->SetInventNum(256 + slot);
                    }
                    else
                    {
                        // it's a container, so can't just swap inventory positions
                        inst->RemoveFromInventory();

                        if (Player->GetEquip(slot))
                        {
                            Player->GetEquip(slot)->RemoveFromInventory();
                            Inventory.GetContainer()->AddToInventory(Player->GetEquip(slot), Inventory.GetHeldSlot());
                        }

                        Player->AddToInventory(inst, 256 + slot);
                    }

                    Player->Equip(inst, slot);
                }
            }
        }
        Inventory.SetDirty(TRUE);
        SetDirty(TRUE);
    }
}

void TEquipPane::DrawBackground()
{
    if (!IsDirty())
        TButtonPane::DrawBackground();
    else
    {
        Display->Put(0, 0, section == SECT_TOP ? top : bottom, DM_BACKGROUND);

        if (Player)
        {
            for (int i = 0; i < NUM_EQ_SLOTS; i++)
                if (grabslot != i && Player->GetEquip(i))
                    if (EquipLoc[section][i].x >= 0)
                        Player->GetEquip(i)->DrawInvItem(EquipLoc[section][i].x, EquipLoc[section][i].y);

            if (grabslot < 0)
                heldslot = -1;
        }

        if (section == SECT_TOP)
        {
            Button(0)->Show();
            Button(1)->Hide();
        }
        else
        {
            Button(1)->Show();
            Button(0)->Hide();
        }

        TButtonPane::DrawBackground();

        PlayScreen.MultiUpdate();
        SetDirty(FALSE);
    }
}

void TEquipPane::Animate(BOOL draw)
{
    if (draw && !mousebutton && Player)
    {
        PTObjectInstance inst = Player->GetInventorySlot(OnSlot(cursorx - GetPosX(), cursory - GetPosY()));
        CursorOverObject(inst);
    }
}

void TEquipPane::Scroll(int amount)
{
    if (amount > 0)
        section = SECT_BOTTOM;
    else if (amount < 0)
        section = SECT_TOP;
    else
        return;

    SetDirty(TRUE);
}
 
void TEquipPane::DrawAnim(PTObjectInstance inst, PTBitmap bm)
{
    if (inst->InventNum() < 256)
        return;

    if (EquipLoc[section][inst->InventNum() - 256].x >= 0)
        Display->Put(EquipLoc[section][inst->InventNum() - 256].x, EquipLoc[section][inst->InventNum() - 256].y, bm);
}

int TEquipPane::OnSlot(int x, int y)
{
    for (int slot = 0; slot < NUM_EQ_SLOTS; slot++)
        if (EquipLoc[section][slot].x >= 0 && EquipLoc[section][slot].y >= 0 &&
            x >= EquipLoc[section][slot].x && x < (EquipLoc[section][slot].x + INVITEMREALWIDTH) &&
            y >= EquipLoc[section][slot].y && y < (EquipLoc[section][slot].y + INVITEMREALHEIGHT))
        return slot;

    return -1;
}
