// Revenant - Copyright 1998 Cinematix Studios, Inc.
// *********** Revenant RULES DEF File  ************ 

// ***** GENERAL RULES TAGS *****
//
// DAYLENGTH length
//
//			The length of a 24 hour game period in 100ths of a second.
//
// TWILIGHT length, steps
//
//          The length of the twilight (morning/evening) periods where the ambient light
//          changes from day to night in game minutes (not real minutes).  Steps indicates
//			the number of step changes for the ambient light when morning or evening
//			occurs.
//
// FATIGUEDATA perlevel, recovpcnt, recovrate
//
//			Describes the amount of fatigue gained per level (not including class and stat
//			bonus percents), the percentage of fatigue recovered every 'recovrate' 100ths
//			of a second.
//
// HEALTHDATA perlevel, recovpcnt, recovrate
//
//			Describes the amount of health gained per level (not including class and stat
//			bonus percents), the percentage of health recovered every 'recovrate' 100ths
//			of a second.
//
// MANADATA perlevel, recovpcnt, recovrate
//
//			Describes the amount of mana gained per level (not including class and stat
//			bonus percents), the percentage of mana recovered every 'recovrate' 100ths
//			of a second.
//
// STEALTH maxstealth, sneaking, minstealth
//
//	maxstealth, minstealth
//
//				The maximum and minimum stealth values used when calculating stealth in
//				1-100 range.  Usually 100 and 10.
//
//	sneaking	A percentage value for the effect of sneaking around in stealth mode.  This
//				is the base percentage multiplied by the characters stealth modifier to get
//				his stealth value when moving in stealth mode.  A lower value gives characters
//				a much higher bonus when in stealth mode (i.e. 50 is a 50% bonus when sneaking).
//
// CLASS "ClassName"			// Describes a character class
// BEGIN
//
//	 The following tags are used within the block
//
//	 FATIGUEMOD pcnt		
//
//		A percent value to modify the fatigue of the character.  A 10 value
//		would mean that characters of this class get a 10% fatigue bonus per level.
//
//	 HEALTHMOD pcnt		
//
//		A percent value to modify the health of the character.  A 10 value
//		would mean that characters of this class get a 10% health bonus per level.
//
//	 MANAMOD pcnt		
//
//		A percent value to modify the mana of the character.  A 10 value
//		would mean that characters of this class get a 10% mana bonus per level.
//
//	 STATREQS str,con,agl,rflx,luck,mind
//
//		Describes the minimum and maximum reqirements for the stats for a character
//		of this class.  Stats are values from 3-24 based on three rolls of 1-8.  
//		Each stat modifies a number of logical character attributes and skills
//		directly, as well as determining what class a character is qualified for.
//		Positive numbers in the statreq means that the character must have at
//		least this number in that stat to qualify for this class.  Negative
//		numbers mean the character can have no more than this number (positive)
//		for that stat to qualify.  Zero means doesn't care.
//
//
//	SKILLMODS attack,defense,hands,KNIFEs,SWORDs,bludgeons,\
//			  axes,bows,stealth,lockpick
//
//		Describes the base modifiers for each of the player skills.  Each 1
//		level of skill added or subtracted from the base skill level for
//		a player typically adds or subtracts 5% to the skill modifier.  Note
//		that these skill modifiers do not change the actual skill level of 
//		the player (i.e. a +4 stealth skill modifier will give a player
//		'Journeyman' (i.e. level 4) stealth skill.
//
// END // of CLASS
//
// CHARACTER "CharName"			// Describes which character this block is for
// BEGIN
//  
//   The following tags are used within the block.  Note that if tag is 
//   described as a 'monster/NPC' tag, it is not used for the player, and 
//   if described as a 'player' tag, it is not used for monsters/NPC's
//
//	 NOTE: the ATTACK line must be wrapped around to mutiple lines using the 
//   line continue character '\' (backslash).
//
//    ARMOR armorvalue		// The basic armor to hit value (1-25) used for monsters/NPC's
//
//	  ATTACK "name", flags, button, "combo", "impact", "block", "miss", "death",\
//			 blocktime, impacttime, waittime, stunwait, mindist, maxdist, snapdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, offensiveskill,\
//			 weaponmask, weaponskill, attackpcnt
//		
//		Describes a specific character attack which the game uses to determine which
//      attack is played when a character presses a button, or a monster trys to attack.
//		When a monster or player attacks, the game looks through the attack list, and
//		finds the first attack that fits the character's or monsters current status by
//		searching first for attacks with the CA_COMBO flag set, then the CA_SPECIAL flag,
//      then all other ordinary attacks.  Character attacks should be listed from least
//		generic (combo's specials, etc.) to most generic in the file.	
//
//			"name"		Name of attack animation to use for attack.  Use just the attack
//						animation name, WITHOUT transition ("combat to myattack") or
//						animation tags ("myattack_o").  Also remember not to use the _s tag
//						with your attack animation in 3DSMAX.
//
//			flags		Attack flags (CA_xxxx flags, i.e. CA_BLOOD to cause attack to
//					    use blood). More than one flag is specified by using
//						the | character (i.e. CA_BLOOD | CA_COMBO).
//
//			button		The attack button (1=Thrust, 2=Swing, 3=Chop, 4-9=Combos)
//
//			"response"	This attack is combo/counter response move for when opponent in this
//					    state (i.e. "stunned" would make this attack a special attack for when
//						the opponent is in the "stunned" animation state.  The attack system
//						will search for response attacks before special attacks and normal
//						attacks.
//
//			"block"		The special block animation for the opponent to use if he blocks.  
//						If this is "", or the opponent doesn't have this block, the game
//						will use the default "block" animation.  Usually the block animations
//						will need to synchronize their frame numbers with the attackers
//						attack by using the 's' tag in the animaion (i.e. "myblock_s").  Note:
//						do not use the 's' tag on the "attack" animations themselves, only
//						blocks and impacts.  Remember, do not use transition names or animation
//						tags in THIS file.
//
//			"miss"		Miss the attacker uses when he misses the opponent.  This miss
//						animation is played immediatly after 'impacttime' on a miss.  Do not
//						use the synchronize tag 's' for this animation.   Remember, do not 
//						use transition names or animation tags in THIS file.
//
//			"death"		Death animation for opponent to play if this attack kills him.
//						This animation can be the same as the "impact" animation if it ends
//						in a looping root state (i.e. "combat to myimpact" and "myimpact_ol").
//						Remember, do not use transition names or animation tags in THIS file.
//
//			blocktime	Number of frames in which opponent can attempt to block the attack.
//						This MUST be less than 'impacttime'.  If the player or monster
//						attempts to block before this time has expired in the attack, the
//						special 'block' animation will play.  If the block animation is
//						synced to the attack with the _s tag, it will begin at the same frame
//						the attack is currently playing.
//			impacttime	Number of frames after the attack begins to show the impact.  This
//						should be set to the exact point at which the weapon hits the 
//						opponent.  This MUST NOT be greater than the number of frames in
//						the attack animation, or the attack will NEVER HIT.  At impact, the
//						game checks the current blocking, etc. status, and calculates if
//						the attack actually hits, if so, the "impact" or "death" animation
//						begins playing (either one of which can be synced to the attack with
//						the '_s' tag in 3DSMAX.	 However, if the char misses, the 
//						attack animation is interrupted, and the game either plays the "miss"
//						animation if there is one, or it goes directly back (interpolating) to
//						the combat neutral stance ("combat").
//
//			waittime	This is the amount of time to wait (during an attack) before the
//						character can do a new move.  This MUST be AFTER impacttime, or the
//						character will never hit anything!
//
//			stunwait	The number of frames the game will loop in the "impact" looping
//						state (i.e. after it has played "combat to myimpact" it will loop
//						in "myimpact_l" for until frames is expired.  This allows you to set
//						up a stun/combo combination where one attack stuns the opponent, and
//						another attack using the "combo" animation name above is used to 
//						finish him off.
//
//			combowait	This is the amount of time to wait for combo's.  For a button press
//						combo, this is the amount of time the player has to press the button
//						again to get the next move in the combo.  For auto combo's, this is
//						the amount of time before the next move is done.
//
//			mindist		These two values set the minimum and maximum distances at which this
//			maxdist		attack may be used.  If the opponent is beyond these distances, the
//						game will look for another more appropriate attack.
//
//			hitminrange	The game will register a hit ONLY when the character is within this
//			hitmaxrange	range when 'impacttime' expires.  This allows the character to leap
//			hitangle	back, or jump to the side before he gets hit.  'hitangle' is a
//						positive angle from 0-128 where 32 is 45 degrees from either side
//						of the attacker.
//
//			damagemod	When the attacker hits, the amount of damage he does is based on
//						the weapon he is using, and his fatigue.  This percentage modifier 
//						can add to, or subtracts from this basic damage value.  10 = +10%,
//						-30 = -30%.
//
//			fatigue		This is the total fatigue value this attack subtracts from the
//						character's fatigue status.  This limits the super kick ass
//						attacks by causing the character to become more fatigued as he
//						uses the best combos.  When the player is very fatigued, his
//						to hit value goes up for attacks, down for defense, and his
//						damage values are reduced.  If the player does not have enough
//						fatigue, he may not execute this attack.  Normal attacks take
//						a very small amount of fatigue.  This is use for players and
//						monsters/NPC's
//
//			offensiveskill
//
//						The player must have at list this offensive skill level to
//						execute this attack. If he doesn't, the game will search for 
//						another valid attack if there is one.  This is only used for players.
//
//			weaponmask	This describes the weapons that may be used with this attack. If
//						the player is using a different weapon, the game will find another
//						attack which matches it. (i.e. WM_AXE | WM_KNIFE).  Note: use
//						the | character to combine weapon mask flags WM_xxx.  This tag is
//						only used for players.  Animations must have a 'weapon' object
//						for the system to replace if you want to see the weapon a player
//						is using.
//
//			weaponskill	The player has skill levels in each weapon catagory.  To execute
//						this attack, the player must have at least this skill level for
//						the weapon he is using.
//
//			attackpcnt	This is a value from 1-100 which controls the frequency of the
//						attack for monsters.  Note that these values are relative to 
//						each other (i.e. if all attacks have a frequency of either 1, or
//						50, they are all equally likely,  however if one attack has 1, and
//						another has 50, the 50 attack is 50 times more liklely to occur).
//						Please use ranges from 10 to 100 for most attacks and reserve 
//						1-10 for truley unlikely attacks.
//
//	  MAGICATTACK "name", flags, button, attackpcnt, mindist, maxdist, 
//				  "spell", spellx, spelly, spellz
//		
//		Describes a magical attack for a character.  Magical attacks are executed along
//      with ordinary attacks when a character is attacking.  Only monsters or NPC's
//      can use the MAGICATTACK tag, as players use the game spell system and the
//      quickspell keys to execute their magical attacks.
//
//			"name"		Same as ATTACK name
//
//			flags		Same as ATTACK flags (CA_MAGICATTACK is automatically added to
//						all MAGICATTACK flags
//
//			button		Same as ATTACK button
//
//			attackpcnt  Same as ATTACK attackpcnt
//
//			mindist		Same as ATTACK mindist
//
//			maxdist		Same as ATTACK maxdist
//
//			"spell"		Name of spell to cast (see SPELL.DEF for spell list)
//
//			spellx, spelly, spellz	
//
//						Offsets for spell source position so that start of spell cast
//                      matches the hands, etc. of the casting character.  Many spells
//						do not use this.  If these are -1,-1,-1, the spell source is
//						assumed to be the characters "rhand" object (its right hand).
//
//	  IMPACT "impact", flags, "loopname", looptime, mindmgpcnt, maxdmgpcnt, stunpos, stuntime
//	  CHARIMPACT same as above
//
//						The IMPACT tag must immediately follow the ATTACK tag for which it is
//						intended.  IMPACT tags are designed to allow the user to set up specific
//						interactive ATTACK/IMPACT animation sets which may be played together
//
//						The CHARIMPACT tag describes the default impacts for a character when
//						he is hit by an opponents weapons.  Remember IMPACT must follow attack
//						tags, but CHARIMPACT can be placed anywhere in a character block.
//						The CHARIMPACT tag is used for characters who use more than the default
//						"impact", and "dead" animations for their impacts.  If a character only
//						uses these two animations, you don't need to use the CHARIMPACT tag.
//
//			"impact"	The special interactive impact animation for the opponent to use if he is hit.  
//						If this is "", the game will use the default impact animations.
//						If the IMPACT tag is not used immediately following an attack, the
//						default "impact" animation will be used.
//
//						Some impacts may need to have the 's' synchronize frame tag to synchronize
//						the frame number for the attack and impact animation.  Note: do
//						NOT use the 's' tag for the attack animation (NO "myattack_s" BAD!).
//						ALSO: use only the main state name, not the transition name for the
//						name in THIS file.. do NOT use any animation tags here either (i.e.
//						_s or _o etc.)  ("myimpact" GOOD, "combat to myimpact" BAD, 
//						"myimpact_s" BAD)
//
//			flags		CAI_STUN, CAI_KNOCKDOWN, CAI_DEATH, CAI_WHENSTUNNED, CAI_WHENDOWN... indicates
//						state character will be in after the impact during the looping portion
//						of the impact.  The stun, knockdown, and death impacts require a looping
//						impact animation.  This may be the "impact" state if "loopname" is blank,
//						and there is a "combat to impact" state and a looping "impact" state
//						Otherwise "loopname" must be supplied for these flags to work.  When an
//						opponent goes into these looping impact states for 'looptime', his attacker
//						may deliver other more crushing special attacks with the CA_ATTACKDOWN or
//						CA_ATTACKSTUN attack flags.  The CAI_DEATH flag indicates the character will
//						be killed with this impact.  For CAI_DEATH, the opponent must have lost all
//						his health with the attack (see CA_DEATH).  CAI_WHENSTUNNED and CAI_WHENDOWN
//						will be used when the character is in a stunned or knocked down state already.
//
//			"loopname"	This is the name of the animation to loop after the initial impact is played.
//						This can be a stun animation (for CAI_STUN), a knockdown animation (for CAI_KNOCKDOWN),
//						or a final death animation (for CAI_DEATH).  The animation is played for the number
//						of frames specified in 'looptime', during which the attacker has time to do other
//						opportunity attacks on the player.  Looping states need to have a "myloop_l" loop
//                      state, and an optional "myloop to combat" transition state back to combat mode if
//						the state returns (not dead).  NOTE: this "loopname" field is provided to allow
//						multiple impacts to end in the same stun, knockdown, or death state.  Impacts can
//						STILL have their own custom loop states and leav the "loopname" field blank if they use
//						a "to myimpact" impact transition state, and a "myimpact_l" looping state, and
//						another "myimpact to combat" return state.
//
//			looptime	The number of frames to loop in the stun or knockdown animation.  If the CAI_DEATH
//						flag is used, this field is ignored (as death is usually a permanent condition).  
//						NOTE: if a custom "to myimpact", "myimpact_l", and "myimpact to combat" set of
//						animtions are used for the impact, and the "loopname" field is left blank, this
//						field is still used to indicate the duration of the "myimpact_l" stage.
//			
//			mindmgpcnt, maxdmgpcnt
//
//						This impact is used when the amount of damage delivered to an opponent, as a percentage
//						of the opponents total health, is between these values.  These values are always between
//						0-100, and all system damage calculations are cut to 100% as well.  This allows the
//						user to set up series of special impacts which are toggled by the amount of damage done.
//
//			snapdist	This is the distance to which the opponent will be snapped to for
//						synchronized animations.  This is used both to force the opponent into the
//						proper position relative to the attacker for interactive attack/impacts, and
//						to push the opponent forward for regular impacts.  For interactive attack/impacts, 
//						you can get this distance by measuring the distance in units between your characters
//						in 3DSMAX.
//
//			snaptime	This is the number of frames over which the character will be snapped into
//						his new position.  Typically this is 0, meaning the character will immediately
//						be snapped into position.  This may also be used to cause character 'push backs'
//						when used with forward attacks by setting the count to a higher number.
//
//    ARROWPOS x, y, z  Position relative to character's position to start arrow when
//                      character shoots a bow or crossbow.
//
//    ARROWSPEED speed  Units per tick that arrow travels
//
//    ATTACKMOD attackmod	// The attack to hit modifier (1-25) for monsters/NPC's
//
//	  ATTACKFREQ minfreq, maxfreq
//
//		  Will attack between minfreq and maxfreq tenths of a  second.  A 
//		  value of 10,20 means the monster will attack every 1-2 seconds.
//
//	  BLOCK	pcnt, min, max	// The freq (per frame), min and max time monster/NPC will block
//
//	  BODYTYPE "type"	// Players only!
//
//						Determines the body type of the character.  Used to match
//						equipment geometry with character geometry.  Valid values
//						are "normal", "large", "small", "dwarf", "lithe".
//						Players may be designed with other body types, but this will
//						prevent them from using the ordinary replacable geometry armor
//						and clothing that most other characters with standard body
//						types can use.  Special armor/clothing can be made for that
//						player, however.
//						
//						When armor or clothing matches a given body type, it will have
//						a state name with that body type, and geometry which uses the
//						body type name as its prefix (i.e. "normalchest", or "stockyshin").
//
//    BOWWAIT ticks     Amount of ticks before character can fire next arrow.  Controls
//                      the speed at which arrows can be shot.
//
//    BOWAIMSPEED angle Angle to move left, right each frame when aiming bow.  Angles are
//                      in 0-255 format where 255 is 360 degrees.
//
//	  COMBATRANGE range		// Range at which char/monster goes into combat mode (64=1 meter)
//
//	  DAMAGEMODS misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison
//
//		  Damage modifier for the various types of damage a char can sustain
//		  in percentages (i.e. 10 is +10%, -40 is -40%, etc.).
//		  For all monsters players, and NPC's
//
//		  misc		Just damage (no specific type)
//		  hand	    Damage from hand to hand combat
//		  puncture  Puncture damage (arrows, knives and swords thrusts)
//		  cut       Cutting or slashing damage (knive and sword swings)
//        chop      Chopping damage (knive, sword, or axe chops)
//        bludgeon  Bludgeoning damage (warhammers, clubs, maces)
//        magical   Magical damage (other than any that fit below)
//		  burn      Burn damage (i.e. lava, fileball, etc.)
//        freeze    Freeze damage (i.e. frost, etc.)
//        poison    Poison damage
//
//	  DEFENSEMOD defensemod	// The defense to hit modifier (1-25) for monsters/NPC's
//
//	  ENEMIES "groupnames"	// A comma delimited list of enemy groups for this character.
//							// If the group names in this list match any of the groups
//							// in the other character, he is considered an enemy.  A 
//							// typical list for a character might include "monster,sardok"
//							// where all monsters, and the character named sardok are
//							// considered enemies.  NOTE: if the character's 'Aggressive'
//							// stat is '0', he will not be an enemy of ANYONE.  This allows
//							// game scripts to turn aggressiveness on/off during the course
//							// of the game by using 'stat Aggressive=0' or 'stat Aggressive=1'.
//
//	  FATIGUE fatigue		// The maximum fatigue value for monsters/NPC's.
//
//	  GROUPS "groupnames"	// A comma delimited lists of groups this character belongs
//							// too.  The character type name (i.e. "Ogrok") is alread
//							// included as a group, and does not need to be repeated 
//							// here.  A valid group list would be "monster,undead" for
//							// a "skeleton".  If any of the groups in a character's
//							// group list appears in another characters enemies list,
//							// the characters will go immediately into combat mode.
//
//	  HEALTH health			// The maximum health value for monsters/NPC's
//
//	  HEARING hearingmin, hearingmax, range
//
//					hearingmin, hearingmax
//							// Is checked every time character moves when in hearing range
//							// of monster.  The hearing value of the monster will be hearing
//							// max when player is at maximum range and hearingmin when player
//							// is at closest range.  When a player moves, it generates a noise
//							// value from 1-100 based on his stealth, agility, and if he is
//							// in sneak mode.  If this value is above the monsters 1-100
//							// hearing value, the monster hears the character.  Max values
//							// should always be greater than min values.
//
//	  MANA mana				// The maximum spell casting points for a monster/NPC
//
//	  SIGHT sightmin, sightmax, range, angle
//
//					sightmin, sightmax
//							// A monster can always see a player when he is in light above
//							// 50%, however, if a char is in dim light, the character will
//							// generate a 'glimpse' value each time he moves from 1-100 based
//							// on his agility, stealth skill, etc.  If this value is greater
//							// than the monster's sight value, the monster sees the character.
//							// The sight value is based on the range of character to monster,
//							// with min being the value for maximum range, and min being the
//							// value for minimum range.  Max values should always be greater
//							// than min values.
//
//					range	// Range in game units (64 units per meter) within which
//                          // a monster can see a character.
//
//					angle	// Angle 0-255 which specifies the field of view of the monster
//							// A value of 64 means he can see 90 degrees to the left or right
//							// side, or 180 degrees in front of himself.
//
//	  SWIPECOLOR r,g,b		// Sword/weapon swipe color
//	
//	  WALKSPEED speed
//	  SNEAKSPEED speed
//	  RUNSPEED speed
//	  COMBATWALKSPEED speed
//
//							// Sets the speed of the character in units per frame when
//							// moving.  If not specified, uses whatever the speed of the
//							// character is in the animation. 
//
//	  WEAPONDAMAGE damage	// The damage (1-10000) for monster/NPC's weapon
//
//	  WEAPONTYPE weaontype  // The weapon type (i.e. WT_AXE) for monsters/NPC's
//
// END // of CHARACTER


// Character attack flags (use with ATTACK -,flags,-,-,-,-,-,-... tag)
#define CA_SPECIAL	     0x0001	// Special attack, set when this attack may override a normal attack
#define CA_RESPONSE      0x0002	// Set when 'response' non-empty (response enabled by opponent state)
#define CA_SPLATTER      0x0004	// Causes opponent to splatter if successful
#define CA_DEATH         0x0008	// Used when damage value is high enough to kill enemy
#define CA_HAND	         0x0010	// Is a thrust type attack (can't do with axe)
#define CA_THRUST        0x0020	// Is a thrust type attack (can't do with axe)
#define CA_SLASH		 0x0040 // This is a slashing attack (if axe, uses CHOP damage type)
#define CA_CHOP		     0x0080	// Is a chop type attack
#define CA_SPARKS		 0x0100 // Show sparks on block
#define CA_BLOOD		 0x0200 // Show blood on impact
//#define CA_SNAPIMPACT	 0x0400 // Snap the impact (THIS FLAG IS NO LONGER USED!, obsoleted by IMPACT tags)
//#define CA_SNAPBLOCK	 0x0800	// Snap the block (THIS FLAG IS NO LONGER USED!, obsoleted by IMPACT tags)
#define CA_NOMISS        0x1000 // Will play entire attack ani even if misses
#define CA_SNEAK		 0x2000 // This attack only available in sneak mode
#define CA_CHAIN		 0x4000 // Will automatically play next attack when next button pressed
#define CA_AUTOCOMBO	 0x8000 // Will play this attack, then the next attack after waiting (see "chainname")
#define CA_ATTACKDOWN	 0x40000 // Attack for when opponent is on ground only! (kick a man when he's down)
#define CA_ATTACKSTUN	 0x80000 // Attack for when opponent is stunned only! (kick a man when he's.. stunned?)
#define CA_MOVING		 0x100000 // This attack is only available when attacker is moving towards opponent
#define CA_ONETARGET	 0x200000 // This attack is applied to only one character
#define CA_NOPUSH		 0x400000 // This attack will not push the other character
#define CA_MAGICATTACK   0x800000 // This is a magical attack (uses the MAGICATTACK tag)

// Character attack impact flags (use with IMPACT tag)
// No flags	(0)				// For an ordinary impact (other than the default "impact" state)
#define CAI_STUN		0x0001	// Results in a looping stun using 'loopname' and 'loopwait'
#define CAI_KNOCKDOWN	0x0002	// Results in a looping knockdown using 'loopname' and 'loopwait'
#define CAI_DEATH		0x0004	// This describes a death impact (other than default "death" state)
#define CAI_WHENSTUNNED	0x0008	// This impact will only be used when the character is stunned
#define CAI_WHENDOWN	0x0010	// This impact will only be used when the character is knocked down
#define CAI_FLYBACK		0x0020	// This impact causes character to fly back (use with CAI_KNOCKDOWN!)
#define CAI_BLOOD		0x0040	// This impact spouts blood

// Character flags (use with FLAGS tag)
#define CF_UNDEAD		0x0001	// This character can only be damaged by magical/silver weapons
#define CF_MAGICAL		0x0002	// This character can only be damaged by magical weapons
#define CF_INFRAVISION	0x0004	// This character can see in the dark
#define CF_LIGHTBLIND	0x0008	// This character is blinded by light (reverse dark/light sight)

// Weapon type defines
#define WT_HAND		    0			// Hand, claw, tail, etc.	
#define WT_KNIFE		1			// Daggers, knives
#define WT_SWORD		2			// Swords
#define WT_BLUDGEON		3			// Clubs, maces, hammers
#define WT_AXE			4			// Axes
#define WT_STAFF		5			// Staffs, polearms, spears, etc.
#define WT_BOW			6			// Bow
#define WT_CROSSBOW		7			// Crossbow
#define WT_LAST			7			// Last weapon type

// Weapon mask defines (Note: make sure these don't match Windows WM_ messages)
#define WM_HAND			0x0001
#define WM_KNIFE		0x0002
#define WM_SWORD		0x0004
#define WM_BLUDGEON		0x0008
#define WM_AXE			0x0010
#define WM_STAFF		0x0020
#define WM_BOW			0x0040
#define WM_CROSSBOW		0x0080

// Define for most two handed weapons
#define WM_TWOHAND		0x000e

DAYLENGTH 80000				// Day length is two REAL minutes
TWILIGHT 60, 8				// 30 GAME minutes for twilight, 15 light change steps
HEALTHDATA 25, 1, 6000		// 25 per level (before stat/class mods), recov 1% per 1 minutes
FATIGUEDATA 25, 1, 100		// 25 per level (before stat/class mods), recov 1% per 1 second
MANADATA 25, 1, 6000		// 25 per level (before stat/class mods), recov 1% per 1 minutes
STEALTH 100, 50, 10			// 100 max stealth, 50 when sneaking, 10 is minimum

CLASS "Revenant"
BEGIN

	FATIGUEMOD 0			// Revenants have no fatigue modifiers
	HEALTHMOD 0				// Revenants have no health modifiers
	MANAMOD 5				// Revenants have a +5% mana modifier

// Revenants have above average strength and above average mind but below average
// constitution as they are actually living undead beings.
//	STATREQS str,con,agl,rflx,luck,mind
	STATREQS 16, 0, 0, 0, 16, -12

// Need to figure this out eventually
//	SKILLMODS attack,defense,invoke,hands,KNIFEs,SWORDs,bludgeons,\
//			  axes,bows,stealth,lockpick
	SKILLMODS 0,0,0,0,0,0,0,0,0,0,0

END

CHARACTER "Default"	// For any character that doesn't have a special definition
BEGIN

END

CHARACTER "Locke"	// Our main man!
BEGIN
 
 // Damage Mods: misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison
  DAMAGEMODS 0, 0, 0, 0, 0, 0, -10, 0, 0, 0  	
  GROUPS "player"				// This is a player
  ENEMIES "monster,priest"		// Hates monsters and priests
  CLASS "Revenant"				// This guy is a Revenant
  COMBATRANGE 320				// About 4 meters away 64 per meter
  WEAPONDAMAGE 20			// Total base damage points for weapon
  SIGHT 30, 100, 640, 64		// Sight percentage, range, and angle
  HEARING 10, 50, 384			// Hearing percentage and range

  SWIPECOLOR 0, 0, 10

// Attack:"name", flags, button, "response", "block", "miss", "death",\
//			 blocktime, impacttime, nextwait, chainexptime, mindist, maxdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, attackskill,\
//			 weaponmask, weaponskill, attackpcnt
//
//  Impact: "name", flags, "loopname", looptime, damagemin, damagemax, snapdist, snaptime
 

// CHARIMPACT "impact", flags, "loopname", looptime, mindmgpcnt, maxdmgpcnt, pushpos, pushtime
   CHARIMPACT "highimpact", 0, "", 0, 0, 5, 0, 0
   CHARIMPACT "lowimpact", 0, "", 0, 6, 10, 0, 0
   CHARIMPACT "heavylowimpact", 0, "", 0, 11, 15, 0, 0
   CHARIMPACT "heavyhighimpact", 0, "", 0, 16, 20, 0, 0      
   CHARIMPACT "knockbackimpact", 0, "", 0, 20, 99, 0, 0


 // Attack 2 counter head throw
//	ATTACK "countert", CA_RESPONSE | CA_ONETARGET | CA_SLASH | CA_SPARKS | CA_BLOOD, 3, "atkthrow", "", "", "",\
//		   1, 8, 164, 0, 1, 50,\
//		   1, 50, 32, 0, 0, 0,\
//		   WM_TWOHAND, 0, 20
//	IMPACT "countertn", 0, "", 0, 1, 999, 15, 0 

// Attack 1 thrust chain attack 3
//	ATTACK "thrustchain3", CA_CHAIN | CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "thrustchain2",\
//		   9, 11, 30, 0, 5, 150,\
//		   5, 80, 32, 0, 0, 0,\
//             WM_TWOHAND, 0, 0

//	IMPACT "knockbackimpact", 0, "", 0, 1, 999, 80, 5
//	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0


// Attack 1 thrust chain attack 2
//	ATTACK "thrustchain2", CA_CHAIN | CA_CHOP | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "thrustchain1",\
//		   9, 11, 12, 24, 5, 150,\
//		   5, 80, 32, 0, 0, 0,\
//		   WM_TWOHAND, 0, 0

//	IMPACT "heavyhighimpact", 0, "", 0, 1, 999, 80, 5
//	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0


// Attack 1 thrust chain attack 1
//	ATTACK "thrustchain1", CA_MOVING | CA_CHAIN | CA_THRUST | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
//		   5, 7, 8, 24, 5, 150,\
//		   5, 80, 32, 0, 0, 0,\
//		   WM_TWOHAND, 0, 0
//
//	IMPACT "heavylowimpact", 0, "", 0, 1, 999, 80, 5
//	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0
 





// Attack 2 swing chain attack 3
	ATTACK "swingchain3", CA_CHAIN | CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 2, "", "", "", "swingchain2",\
		   9, 11, 30, 0, 5, 150,\
		   5, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "knockbackimpact", 0, "", 0, 1, 999, 80, 5
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0


// Attack 2 swing chain attack 2
	ATTACK "swingchain2", CA_CHAIN | CA_CHOP | CA_SPARKS | CA_BLOOD | CA_NOMISS, 2, "", "", "", "swingchain1",\
		   11, 13, 14, 24, 5, 150,\
		   5, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "heavyhighimpact", 0, "", 0, 1, 999, 80, 5
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0


// Attack 2 swing chain attack 1
	ATTACK "swingchain1", CA_MOVING | CA_THRUST | CA_SPARKS | CA_BLOOD | CA_NOMISS, 2, "", "", "", "",\
		   13, 15, 16, 24, 5, 150,\
		   5, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "heavylowimpact", 0, "", 0, 1, 999, 80, 5
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0







// Attack 3 chop chain attack 3
	ATTACK "chopchain3", CA_CHAIN | CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 3, "", "", "", "chopchain2",\
		   0, 8, 30, 0, 5, 150,\
		   5, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "knockbackimpact", 0, "", 1, 1, 999, 0, 0
	IMPACT "combat to dead2", CAI_DEATH, "dead2", 0, 1, 999, 0, 0

// Attack 3 chop chain attack 2
	ATTACK "chopchain2", CA_CHAIN | CA_CHOP | CA_SPARKS | CA_BLOOD | CA_NOMISS, 3, "", "", "", "chopchain1",\
		   0, 8, 9, 24, 5, 150,\
		   5, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "heavyhighimpact", 0, "", 1, 1, 999, 80, 20
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0

// Attack 3 chop chain attack 1
	ATTACK "chopchain1", CA_MOVING | CA_CHOP | CA_SPARKS | CA_BLOOD | CA_NOMISS, 3, "", "", "", "",\
		   13, 15, 16, 24, 5, 300,\
		   5, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "heavylowimpact", 0, "", 1, 1, 999, 80, 20
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0






// Attack 2 cut ogrok's head off
	ATTACK "atkdecap", CA_SPECIAL | CA_ONETARGET | CA_DEATH | CA_THRUST | CA_SPARKS | CA_BLOOD | CA_NOMISS, 2, "", "", "atkdecapmiss", "",\
		   4, 23, 10, 0, 40, 80,\
		   0, 80, 32, 10, 5, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "impdecap", CAI_DEATH, "", 0, 1, 999, 48, 0





// Attack 1 standard thrust for swords
	ATTACK "attackthrust", CA_THRUST | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   5, 9, 10, 0, 0, 0,\
		   0, 80, 32, 0, 5, 0,\
		   WM_TWOHAND, 0, 0


// Attack 2 standard slash for swords, axes, hammers, maces
	ATTACK "attackswing", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 2, "", "", "", "",\
		   6, 10, 13, 0, 0, 0,\
		   0, 80, 32, 0, 5, 0,\
		   WM_TWOHAND, 0, 0


// Attack 3 standard chop for swords, axes, hammers, maxes
//	ATTACK "attackchop", CA_CHOP | CA_SPARKS | CA_BLOOD | CA_NOMISS, 3, "", "", "", "",\
//		   8, 14, 19, 0, 0, 0,\
//		   0, 80, 32, 0, 5, 0,\
//		   WM_TWOHAND, 0, 0

// Attack 3 standard chop for swords, axes, hammers, maxes
	ATTACK "punch", CA_HAND | CA_BLOOD | CA_NOMISS, 3, "", "", "", "",\
		   8, 14, 19, 0, 0, 0,\
		   0, 80, 32, 0, 5, 0,\
		   WM_HAND, 0, 0


 // Attack 2 hip throw
//	ATTACK "atkhipthrow", CA_SPECIAL | CA_ONETARGET | CA_SLASH | CA_SPARKS | CA_BLOOD, 2, "", "", "", "",\
//		   1, 8, 164, 0, 1, 50,\
//		   1, 50, 32, 0, 0, 0,\
//		   WM_TWOHAND, 0, 20
//	IMPACT "imphipthrow", 0, "", 0, 1, 999, 48, 0 

// Attack 1 thrust chain attack 3
	ATTACK "punchchain3", CA_CHAIN | CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 2, "", "", "", "punchchain2",\
		   5, 9, 30, 0, 5, 150,\
		   0, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "impact", 0, "", 0, 1, 999, 80, 5
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0


// Attack 1 thrust chain attack 2
	ATTACK "punchchain2", CA_CHAIN | CA_CHOP | CA_SPARKS | CA_BLOOD | CA_NOMISS, 2, "", "", "", "punchchain1",\
		   5, 9, 15, 20, 5, 150,\
		   0, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "impact", 0, "", 0, 1, 999, 80, 5
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0

// Attack:"name", flags, button, "response", "block", "miss", "death",\
//			 blocktime, impacttime, nextwait, chainexptime, mindist, maxdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, attackskill,\
//			 weaponmask, weaponskill, attackpcnt
//
// Attack 1 thrust chain attack 1
	ATTACK "punchchain1", CA_CHAIN | CA_THRUST | CA_SPARKS | CA_BLOOD | CA_NOMISS, 2, "", "", "", "",\
		   5, 12, 20, 25, 5, 300,\
		   0, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "impact", 0, "", 0, 1, 999, 80, 5
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0


// Attack 1 thrust chain attack 3
	ATTACK "kickchain3", CA_CHAIN | CA_SLASH | CA_BLOOD | CA_NOMISS, 1, "", "", "", "kickchain2",\
		   5, 9, 30, 0, 5, 150,\
		   0, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "impact", 0, "", 0, 1, 999, 80, 5
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0


// Attack 1 thrust chain attack 2
	ATTACK "kickchain2", CA_CHAIN | CA_CHOP | CA_BLOOD | CA_NOMISS, 1, "", "", "", "kickchain1",\
		   5, 9, 15, 20, 5, 150,\
		   0, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "impact", 0, "", 0, 1, 999, 80, 5
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0

// Attack:"name", flags, button, "response", "block", "miss", "death",\
//			 blocktime, impacttime, nextwait, chainexptime, mindist, maxdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, attackskill,\
//			 weaponmask, weaponskill, attackpcnt
//
// Attack 1 thrust chain attack 1
	ATTACK "kickchain1", CA_CHAIN | CA_THRUST | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   5, 12, 20, 25, 5, 300,\
		   0, 80, 32, 0, 0, 0,\
		   WM_TWOHAND, 0, 0

	IMPACT "impact", 0, "", 0, 1, 999, 80, 5
	IMPACT "combat to dead", CAI_DEATH, "dead", 0, 1, 999, 0, 0




 
END


CHARACTER "Ogrok"	// The ogrok dude!
BEGIN
 
 // Damage Mods: misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison
  DAMAGEMODS 0, 0, 0, 0, 0, -10, 10, 0, 0, 0
   	// Ogroks have hard heads, and are resistant to bludgeon damage.
	// They are more suceptible to magic damage though.

  HEALTH 35					// What is this monsters health value
  FATIGUE 35				// His fatigue value?
  MANA 0					// Mana (none for Ogroks)
  GROUPS "monster,natural"	// This is a natural monster
  ENEMIES "player,undead"	// Hates the undead and players!
  ARMOR 3					// Armor value (tohit 1-25 > armor + defensemod - attackmod)
  BLOCK 3,10,20
  ATTACKFREQ 100, 250		// Min and max in 100ths of second
  ATTACKMOD 0				// Attack to hit modifier in pcnt (10=+10%, -20=-20%) 
  DEFENSEMOD 0				// Defense to hit modifier in pcnt (10=+10%, -20=-20%) 
  WEAPONDAMAGE 5			// Total base damage points for weapon
  WEAPONTYPE WT_AXE			// Type of weapon (for damage modifiers)
  SIGHT 30, 100, 640, 64	// Sight percentage, range, and angle
  HEARING 10, 50, 384		// Hearing percentage and range
  COMBATRANGE 320			// About 4 meters away 64 per meter

// Attack:"name", flags, button, "response", "block", "miss", "chain",\
//			 blocktime, impacttime, nextwait, chainexptime, mindist, maxdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, attackskill,\
//			 weaponmask, weaponskill, attackpcnt
//
//  Impact: "name", flags, "loopname", looptime, damagemin, damagemax, snapdist, snaptime

  // Attack 3 axe throw
	ATTACK "atkaxethrow", CA_SPECIAL | CA_SLASH | CA_SPARKS | CA_BLOOD | CA_ONETARGET, 3, "", "", "atkaxemiss", "",\
		   10, 10, 10, 0, 40, 80,\
		   0, 80, 32, 0, 0, 0,\
		   WM_SWORD | WM_BLUDGEON | WM_AXE, 0, 30
	IMPACT "impaxethrow", 0, "", 0, 0, 100, 43, 0 

 // Attack 2 head throw
	ATTACK "atkthrow", CA_SPECIAL | CA_SLASH | CA_SPARKS | CA_BLOOD | CA_ONETARGET, 2, "", "", "atkthrowmiss", "",\
		   10, 17, 164, 0, 40, 80,\
		   0, 80, 32, 0, 0, 0,\
		   WM_SWORD | WM_BLUDGEON | WM_AXE, 0, 30
	IMPACT "impthrow", 0, "", 0, 0, 100, 48, 0 
  
  // Attack 1 thrust for swords
	ATTACK "attackthrust", CA_THRUST | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   4, 10, 10, 0, 40, 80,\
		   0, 80, 32, 0, 0, 0,\
		   WM_SWORD, 0, 100

  // Attack 1 hit for axes, hammers, maces
	ATTACK "attackhit", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   4, 10, 10, 0, 40, 80,\
		   0, 80, 32, 0, 0, 0,\
		   WM_BLUDGEON | WM_AXE, 0, 100

  // Attack 2 slash for swords, axes, hammers, maces
	ATTACK "attackswing", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 2, "", "", "", "",\
		   6, 9, 13, 0, 40, 80,\
		   0, 80, 32, 0, 0, 0,\
		   WM_SWORD | WM_BLUDGEON | WM_AXE, 0, 100

  // Attack 3 chop for swords, axes, hammers, maxes
	ATTACK "attackchop", CA_CHOP | CA_SPARKS | CA_BLOOD | CA_NOMISS, 3, "", "", "", "",\
		   8, 14, 19, 0, 40, 80,\
		   0, 80, 32, 0, 0, 0,\
		   WM_SWORD | WM_BLUDGEON | WM_AXE, 0, 100

END

CHARACTER "Issathi"	// The lizard guy
BEGIN
 
 // Damage Mods: misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison
  DAMAGEMODS 0, 0, 0, 0, 0, -10, 10, 0, 0, 0
   	// Ogroks have hard heads, and are resistant to bludgeon damage.
	// They are more suceptible to magic damage though.

  HEALTH 35					// What is this monsters health value
  FATIGUE 35				// His fatigue value?
  MANA 0					// Mana (none for Ogroks)
  GROUPS "monster,natural"	// This is a natural monster
  ENEMIES "player,undead"	// Hates the undead and players!
  ARMOR 5					// Armor value (tohit 1-25 > armor + defensemod - attackmod)
  ATTACKFREQ 100, 250		// Min and max in 100ths of second
  ATTACKMOD 0				// Attack to hit modifier in pcnt (10=+10%, -20=-20%) 
  DEFENSEMOD 0				// Defense to hit modifier in pcnt (10=+10%, -20=-20%) 
  WEAPONDAMAGE 5			// Total base damage points for weapon
  WEAPONTYPE WT_AXE			// Type of weapon (for damage modifiers)
  SIGHT 30, 100, 640, 64	// Sight percentage, range, and angle
  HEARING 10, 50, 384		// Hearing percentage and range
  COMBATRANGE 320			// About 4 meters away 64 per meter

// Attack:"name", flags, button, "response", "block", "miss", "chain",\
//			 blocktime, impacttime, nextwait, chainexptime, mindist, maxdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, attackskill,\
//			 weaponmask, weaponskill, attackpcnt
//
//  Impact: "name", flags, "loopname", looptime, damagemin, damagemax, snapdist, snaptime
 
  // Attack 1 biting attack
	ATTACK "attack1", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   4, 10, 10, 0, 0, 0,\
		   40, 80, 32, 0, 0, 0,\
		   0, 0, 100
 
  // Attack 2 tail attack
	ATTACK "attack2", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 2, "", "", "", "",\
		   6, 9, 13, 0, 0, 0,\
		   40, 80, 32, 0, 0, 0,\
		   0, 0, 100
  
END


CHARACTER "Hopper"	// The frog thing
BEGIN
 
 // Damage Mods: misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison
  DAMAGEMODS 0, 0, 0, 0, 0, -10, 10, 0, 0, 0
   	// Ogroks have hard heads, and are resistant to bludgeon damage.
	// They are more suceptible to magic damage though.

  HEALTH 35					// What is this monsters health value
  FATIGUE 35				// His fatigue value?
  MANA 0					// Mana (none for Ogroks)
  GROUPS "monster,natural"	// This is a natural monster
  ENEMIES "player,undead"	// Hates the undead and players!
  ARMOR 5					// Armor value (tohit 1-25 > armor + defensemod - attackmod)
  ATTACKFREQ 100, 250		// Min and max in 100ths of second
  ATTACKMOD 0				// Attack to hit modifier in pcnt (10=+10%, -20=-20%) 
  DEFENSEMOD 0				// Defense to hit modifier in pcnt (10=+10%, -20=-20%) 
  WEAPONDAMAGE 5			// Total base damage points for weapon
  WEAPONTYPE WT_AXE			// Type of weapon (for damage modifiers)
  SIGHT 30, 100, 640, 64	// Sight percentage, range, and angle
  HEARING 10, 50, 384		// Hearing percentage and range
  COMBATRANGE 320			// About 4 meters away 64 per meter

 // Attack:"name", flags, button, "response", "impact", "block", "miss", "chain",\
//			 blocktime, impacttime, waittime, stunwait, combowait, mindist, maxdist, snapdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, offensiveskill,\
//			 weaponmask, weaponskill, attackpcnt
 
  // Attack 1 biting attack
	ATTACK "attack1", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   4, 10, 10, 0, 0, 0,\
		   40, 80, 32, 0, 0, 0,\
		   0, 0, 100
     
END

CHARACTER "Arakna"	// The big spider
BEGIN
 
 // Damage Mods: misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison
  DAMAGEMODS 0, 0, 0, 0, 0, -10, 10, 0, 0, 0
   	// Ogroks have hard heads, and are resistant to bludgeon damage.
	// They are more suceptible to magic damage though.

  HEALTH 35					// What is this monsters health value
  FATIGUE 35				// His fatigue value?
  MANA 0					// Mana (none for Ogroks)
  GROUPS "monster,natural"	// This is a natural monster
  ENEMIES "player,undead"	// Hates the undead and players!
  ARMOR 5					// Armor value (tohit 1-25 > armor + defensemod - attackmod)
  ATTACKFREQ 100, 250		// Min and max in 100ths of second
  ATTACKMOD 0				// Attack to hit modifier in pcnt (10=+10%, -20=-20%) 
  DEFENSEMOD 0				// Defense to hit modifier in pcnt (10=+10%, -20=-20%) 
  WEAPONDAMAGE 5			// Total base damage points for weapon
  WEAPONTYPE WT_AXE			// Type of weapon (for damage modifiers)
  SIGHT 30, 100, 640, 64	// Sight percentage, range, and angle
  HEARING 10, 50, 384		// Hearing percentage and range
  COMBATRANGE 320			// About 4 meters away 64 per meter

 // Attack:"name", flags, button, "response", "impact", "block", "miss", "death",\
//			 blocktime, impacttime, waittime, stunwait, combowait, mindist, maxdist, snapdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, offensiveskill,\
//			 weaponmask, weaponskill, attackpcnt
 
  // Attack 1 biting attack
	ATTACK "attack1", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   4, 10, 10, 0, 0, 0,\
		   40, 80, 32, 0, 0, 0,\
		   0, 0, 100
     
END

CHARACTER "Cobra"	// The big snake
BEGIN
 
 // Damage Mods: misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison
  DAMAGEMODS 0, 0, 0, 0, 0, -10, 10, 0, 0, 0
   	// Ogroks have hard heads, and are resistant to bludgeon damage.
	// They are more suceptible to magic damage though.

  HEALTH 35					// What is this monsters health value
  FATIGUE 35				// His fatigue value?
  MANA 0					// Mana (none for Ogroks)
  GROUPS "monster,natural"	// This is a natural monster
  ENEMIES "player,undead"	// Hates the undead and players!
  ARMOR 5					// Armor value (tohit 1-25 > armor + defensemod - attackmod)
  ATTACKFREQ 100, 250		// Min and max in 100ths of second
  ATTACKMOD 0				// Attack to hit modifier in pcnt (10=+10%, -20=-20%) 
  DEFENSEMOD 0				// Defense to hit modifier in pcnt (10=+10%, -20=-20%) 
  WEAPONDAMAGE 5			// Total base damage points for weapon
  WEAPONTYPE WT_AXE			// Type of weapon (for damage modifiers)
  SIGHT 30, 100, 640, 64	// Sight percentage, range, and angle
  HEARING 10, 50, 384		// Hearing percentage and range
  COMBATRANGE 320			// About 4 meters away 64 per meter

 // Attack:"name", flags, button, "response", "impact", "block", "miss", "death",\
//			 blocktime, impacttime, waittime, stunwait, combowait, mindist, maxdist, snapdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, offensiveskill,\
//			 weaponmask, weaponskill, attackpcnt
 
  // Attack 1 biting attack
	ATTACK "attack1", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   4, 10, 10, 0, 0, 0,\
		   40, 80, 32, 0, 0, 0,\
		   0, 0, 100
     
END

CHARACTER "Hooded Priest"	// The Priest
BEGIN
 
 // Damage Mods: misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison
  DAMAGEMODS 0, 0, 0, 0, 0, -10, 10, 0, 0, 0
   	// Ogroks have hard heads, and are resistant to bludgeon damage.
	// They are more suceptible to magic damage though.

  HEALTH 75					// What is this monsters health value
  FATIGUE 75				// His fatigue value?
  MANA 100					// Mana (hooded priests have 100)
  GROUPS "priest,natural"	// This is a natural monster
  ENEMIES "player,undead,monster"	// Hates the undead and players!
  ARMOR 5					// Armor value (tohit 1-25 > armor + defensemod - attackmod)
  ATTACKFREQ 0,200 		// Min and max in 100ths of second
  ATTACKMOD 0			// Attack to hit modifier in pcnt (10=+10%, -20=-20%) 
  BLOCK 50,24,36
  DEFENSEMOD 0				// Defense to hit modifier in pcnt (10=+10%, -20=-20%) 
  WEAPONDAMAGE 5			// Total base damage points for weapon
  WEAPONTYPE WT_AXE			// Type of weapon (for damage modifiers)
  SIGHT 30, 100, 640, 64	// Sight percentage, range, and angle
  HEARING 10, 50, 384		// Hearing percentage and range
  COMBATRANGE 320			// About 4 meters away 64 per meter
  COMBATWALKSPEED 1
  SWIPECOLOR 255,0,0

// Attack:"name", flags, button, "response", "block", "miss", "chain",\
//			 blocktime, impacttime, nextwait, chainexptime, mindist, maxdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, attackskill,\
//			 weaponmask, weaponskill, attackpcnt
//
//  Impact: "name", flags, "loopname", looptime, damagemin, damagemax, snapdist, snaptime
//
//  Magic Attack: "name", flags, button, attackpcnt, mindist, maxdist, 
//                "spell", spellx, spelly, spellz
 
// Attack 1 fire ball
	MAGICATTACK "Fireball", CA_MAGICATTACK, 0, 100, 128, 640,\
                "Fireball", -1, -1, -1

// Attack 1 fire ball
	MAGICATTACK "attack01", CA_MAGICATTACK, 0, 100, 128, 640,\
                "VortexM", -1, -1, -1
   
// Attack 1 strike attack 1 
	ATTACK "strike", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   7,7, 10, 0, 0, 80,\
		   0, 80, 32, 0, 0, 0,\
		   0, 0, 100
      

END

CHARACTER "Zombie"	// The Zombie
BEGIN
 
 // Damage Mods: misc, hand, puncture, cut, chop, bludgeon, magical, burn, freeze, poison
  DAMAGEMODS 0, 0, 0, 0, 0, -10, 10, 0, 0, 0
   	// Ogroks have hard heads, and are resistant to bludgeon damage.
	// They are more suceptible to magic damage though.

  HEALTH 75					// What is this monsters health value
  FATIGUE 75				// His fatigue value?
  MANA 100					// Mana (hooded priests have 100)
  GROUPS "priest,natural"	// This is a natural monster
  ENEMIES "player,undead,monster"	// Hates the undead and players!
  ARMOR 5					// Armor value (tohit 1-25 > armor + defensemod - attackmod)
  ATTACKFREQ 0,200 		// Min and max in 100ths of second
  ATTACKMOD 0			// Attack to hit modifier in pcnt (10=+10%, -20=-20%) 
  BLOCK 50,24,36
  DEFENSEMOD 0				// Defense to hit modifier in pcnt (10=+10%, -20=-20%) 
  WEAPONDAMAGE 5			// Total base damage points for weapon
  WEAPONTYPE WT_AXE			// Type of weapon (for damage modifiers)
  SIGHT 30, 100, 640, 64	// Sight percentage, range, and angle
  HEARING 10, 50, 384		// Hearing percentage and range
  COMBATRANGE 320			// About 4 meters away 64 per meter
  COMBATWALKSPEED 1
  SWIPECOLOR 255,0,0

// Attack:"name", flags, button, "response", "block", "miss", "chain",\
//			 blocktime, impacttime, nextwait, chainexptime, mindist, maxdist,\
//			 hitminrange, hitmaxrange, hitangle, damagemod, fatigue, attackskill,\
//			 weaponmask, weaponskill, attackpcnt
//
//  Impact: "name", flags, "loopname", looptime, damagemin, damagemax, snapdist, snaptime
//
//  Magic Attack: "name", flags, button, attackpcnt, mindist, maxdist, 
//                "spell", spellx, spelly, spellz
 
  	ATTACK "doublepunch1", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
               4, 12, 10, 0, 30, 70,\
      	   10, 55, 120, 3, 0, 0,\
       	   0, 0, 50

	ATTACK "doublepunch2", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   4, 21, 10, 0, 15, 50,\
		   10, 55, 120, 3, 0, 0,\
		   0, 0, 75

      ATTACK "axehandle", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   4, 21, 10, 0, 40, 120,\
		   10, 50, 120, 1, 0, 0,\
		   0, 0, 25
      
      ATTACK "overhand", CA_SLASH | CA_SPARKS | CA_BLOOD | CA_NOMISS, 1, "", "", "", "",\
		   4, 18, 10, 0, 40, 80,\
		   5, 60, 120, 1, 0, 0,\
		   0, 0, 25

// Attack 1 fire ball
	MAGICATTACK "Vomit", CA_MAGICATTACK, 0, 100, 128, 640,\
                "Puke", -1, -1, -1

      

END
