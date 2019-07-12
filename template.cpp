// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           template.cpp - FORSAKEN terrain template routines           *
// *************************************************************************

#include <stdio.h>

#include "revenant.h"
#include "template.h"
#include "parse.h"
#include "object.h"
#include "sector.h"
#include "mappane.h"

extern TObjectClass TileClass;

TTerrainTemplate::TTerrainTemplate()
{
    memset(templates, 0, sizeof(STemplate) * MAXTEMPLATES);
    numtemplates = 0;
    Load();
}

TTerrainTemplate::~TTerrainTemplate()
{
    Save();
}

BOOL TTerrainTemplate::Load()
{
    FILE *fp = popen(TEMPLATEFILENAME, "rb");
    if (!fp)
        return FALSE;

    TFileParseStream s(fp, TEMPLATEFILENAME);
    TToken t(s);
    t.Get();

    while (t.Type() != TKN_EOF)
    {
        if (t.Type() != TKN_IDENT || !t.Is("template"))
            FatalError("'template' identifier expected");
        else
        {
            if (numtemplates >= MAXTEMPLATES)
                break;

            // get the tile names
            t.WhiteGet();
            if (t.Type() != TKN_TEXT)
                FatalError("expected tile name following template identifier");

            do
            {
                if (templates[numtemplates].numnames >= MAXTERRAINNAMES)
                    break;

                strcpy(templates[numtemplates].names[templates[numtemplates].numnames++], t.Text());
                t.WhiteGet();
                if (t.Type() == TKN_SYMBOL && t.Code() == ',')
                    t.WhiteGet();
            } while (t.Type() == TKN_TEXT);

            t.LineGet();
            t.DoBegin();

            // get the objects for the template
            while ((t.Type() != TKN_KEYWORD || t.Code() != KEY_END) && t.Type() != TKN_EOF)
            {
                if (templates[numtemplates].numobjects >= MAXOBJECTREFS)
                    break;

                if (t.Type() != TKN_TEXT)
                    FatalError("expected tile name in template object list");

                strcpy(templates[numtemplates].objects[templates[numtemplates].numobjects].name, t.Text());

                t.WhiteGet();
                if (t.Type() != TKN_NUMBER)
                    FatalError("expected tile x offset in template object list");
                templates[numtemplates].objects[templates[numtemplates].numobjects].offset.x = t.Index();

                t.WhiteGet();
                if (t.Type() == TKN_SYMBOL && t.Code() == ',')
                    t.WhiteGet();
                if (t.Type() != TKN_NUMBER)
                    FatalError("expected tile y offset in template object list");
                templates[numtemplates].objects[templates[numtemplates].numobjects].offset.y = t.Index();

                t.WhiteGet();
                if (t.Type() == TKN_SYMBOL && t.Code() == ',')
                    t.WhiteGet();
                if (t.Type() != TKN_NUMBER)
                    FatalError("expected tile z offset in template object list");
                templates[numtemplates].objects[templates[numtemplates].numobjects].offset.z = t.Index();

                templates[numtemplates].numobjects++;
                t.LineGet();
            }

            numtemplates++;

            t.DoEnd();
        }
    }

    fclose(fp);
    return TRUE;
}

BOOL TTerrainTemplate::Save()
{
    FILE *fp = popen(TEMPLATEFILENAME, "wb");
    if (!fp)
        return FALSE;

    for (int t = 0; t < numtemplates; t++)
    {
        fprintf(fp, "template ");
        for (int n = 0; n < templates[t].numnames; n++)
        {
            if (n > 0)
                fprintf(fp, ", ");

            fprintf(fp, "\"%s\"", templates[t].names[n]);
        }

        fprintf(fp, "\r\nbegin\r\n");

        for (int o = 0; o < templates[t].numobjects; o++)
        {
            fprintf(fp, "\t\"%s\" %d, %d, %d\r\n", templates[t].objects[o].name,
                    templates[t].objects[o].offset.x, templates[t].objects[o].offset.y,
                    templates[t].objects[o].offset.z);
        }

        fprintf(fp, "end\r\n\r\n");
    }

    fclose(fp);

    return TRUE;
}

int TTerrainTemplate::NewTemplate(char *tilename)
{
    if (numtemplates >= MAXTEMPLATES)
        return -1;

    strcpy(templates[numtemplates].names[0], tilename);
    templates[numtemplates].numnames = 1;
    templates[numtemplates].numobjects = 0;

    return numtemplates++;
}

int TTerrainTemplate::AddObject(int index, PTObjectInstance inst, S3DPoint offset)
{
    if ((DWORD)index >= MAXTEMPLATES ||
        templates[index].numobjects >= MAXOBJECTREFS)
        return -1;

    strcpy(templates[index].objects[templates[index].numobjects].name, inst->GetTypeName());
    templates[index].objects[templates[index].numobjects].offset = offset;

    return templates[index].numobjects++;
}

#define MAXFOUND    32

void TTerrainTemplate::ApplyTemplate(PTObjectInstance inst, PTSector sect, int density)
{
    if (!inst || !sect)
        return;

    char *name = inst->GetTypeName();
    int foundlist[MAXFOUND];
    int numfound = 0;

    BOOL found = FALSE;
    for (int t = 0; t < numtemplates && numfound < MAXFOUND; t++)
        for (int n = 0; n < templates[t].numnames && numfound < MAXFOUND; n++)
            if (stricmp(templates[t].names[n], name) == 0)
                foundlist[numfound++] = t;

    if (numfound < 1)
        return;

    t = foundlist[random(0, numfound - 1)];

    S3DPoint pos;
    inst->GetPos(pos);

    for (int i = 0; i < templates[t].numobjects; i++)
    {
        if (random(1, MAXDENSITY) > density)
            continue;

        SObjectDef def;
        memset(&def, 0, sizeof(SObjectDef));

        def.objclass = OBJCLASS_TILE;
        def.objtype = TileClass.FindObjType(templates[t].objects[i].name);
        def.flags = OF_GENERATED;
        def.pos.x = pos.x + templates[t].objects[i].offset.x;
        def.pos.y = pos.y + templates[t].objects[i].offset.y;
        def.pos.z = pos.z + templates[t].objects[i].offset.z;
        def.level = MapPane.GetMapLevel();

        inst = TileClass.NewObject(&def);
        if (inst)
            sect->AddObject(inst);
    }
}
