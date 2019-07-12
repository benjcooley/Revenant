// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                CharStats.h - Defines character stats                  *
// *************************************************************************

#ifndef _CHARSTATS_H
#define _CHARSTATS_H

// Character flag defines  (indexes into player OBJSTATS stats in CLASS.DEF)
#define CHRFLAG_FIRST       0
#define NUM_CHRFLAGS        3

#define CHRFLAG_AGGRESSIVE  0
#define CHRFLAG_POISONED    1
#define CHRFLAG_SLEEPING    2


// Character stat defines  (indexes into player OBJSTATS stats in CLASS.DEF)
#define NUM_CHRSTATS        3
#define CHRSTAT_FIRST       (CHRFLAG_FIRST + NUM_CHRFLAGS)

#define CHRSTAT_HEALTH      0           // Current health
#define CHRSTAT_FATIGUE     1           // Current fatigue
#define CHRSTAT_MANA        2           // Current mana

// Player value defines (anything that's not a stat or a skill)
#define NUM_PLRVALS         2
#define PLRVAL_FIRST        (CHRSTAT_FIRST + NUM_CHRSTATS)

#define PLRVAL_LEVEL        0
#define PLRVAL_EXP          1

// Player stat defines (indexes into player OBJSTATS stats in CLASS.DEF)
#define NUM_PLRSTATS        6
#define PLRSTAT_FIRST       (PLRVAL_FIRST + NUM_PLRVALS)

#define PLRSTAT_STRN        0           // Strength
#define PLRSTAT_CONS        1           // Constitution
#define PLRSTAT_AGIL        2           // Agility
#define PLRSTAT_RFLX        3           // Reflexes
#define PLRSTAT_MIND        4           // Mind
#define PLRSTAT_LUCK        5           // Luck

// Skill defines (indexes into player OBJSTATS stats in CLASS.DEF)
#define NUM_SKILLS          11
#define SK_FIRST            (PLRSTAT_FIRST + NUM_PLRSTATS)
#define SKE_FIRST           (SK_FIRST + NUM_SKILLS)

#define SK_ATTACK           0           // Offensive combat
#define SK_DEFENSE          1           // Defensive combat
#define SK_INVOKE           2           // Invocation ability
#define SK_HANDS            3           // Hand attacks          (WT_HAND)
#define SK_KNIFE            4           // Daggers, knives       (WT_SHORTBLADE)
#define SK_SWORD            5           // Swords                (WT_LONGBLADE)
#define SK_BLUDGEONS        6           // Clubs, maces, hammers (WT_BLUDGEON)
#define SK_AXES             7           // Axes                  (WT_AXE)
#define SK_BOWS             8           // Bows                  (WT_BOW)
#define SK_STEALTH          9           // Sneaking around
#define SK_LOCKPICK         10          // Picking locks

#define SK_WEAPONSKILLS (SK_HANDS)      // Where are the weapon skills

#endif

