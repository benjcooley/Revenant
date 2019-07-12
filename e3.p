// Revenant - Copyright 1998 Cinematix Studios, Inc.//
//************** Revenant E3 Script File  ************// 

// Script for Dungeon playable level

// Crypt Room

OBJECT "Horatio"
begin

  CUBE Locke 6928,6448,144 7216,5776,0
  BEGIN
	control off
	Locke.stop
	Locke.goto 7024 6064 
	wait 64
	scrollto Horatio
	wait 36
	say "Your majesty, this is an unexpected surprise."
	wait char Horatio

	Locke.say "Do I know you spirit?"
	wait char Locke

	say "Have you forgotten the pain and suffering you caused?"
	say "The destruction of an empire caused by your actions?"
	

     :Start

	Choice Who "Who the hell are you anyway?"
	Choice Answer "I know nothing of these things."	
	wait response

	:Answer
	Locke.say "I know nothing of these things."
	
	say "I see... It is a shame that you are not haunted by the screams of the thousands.."
      say "who lost their lives as a result of your ignorance."
	jump Finish

     :Who
	Locke.say "Who the hell are you anyway?"
	
	say "I am Horatio, Archbishop and servant of the great empire.... and it's King."
	jump Finish

     :Finish
	wait 24
	Locke.say "I don't know who you are or what you speak of but I tire of this conversation." 
	wait 24
	Locke.say "Either return from where you came or prepare to be sent back there permanently."
	wait char Locke

	say "I spent my entire mortal existence serving you as my king."
	say "I absolve myself of this servitude with one final act."
	say "In front of you is a chest."
	say "Contained within is a powerful talisman."
	say "Take it with you if you hope to escape from here alive."
	say "If you succeed in this quest you may yet free me from this undead hell."
	say "I help you for that reason alone."
	say "Farewell your majesty."
	wait 24
	toggle invisible=1
	nowait teleporter.delete

	scrollto Locke
	control on
	toggle pause=1  
     END	

END

//script for lever puzzle room

OBJECT "LV42L1" 
begin

  USE
  BEGIN
	LV45UB1.use
  END

END


OBJECT "Lv45L2"
begin

  USE
  BEGIN
	Lv45UB3.use
  END

END

OBJECT "Lv45L3"
begin

  USE
  BEGIN
	Lv45Trap2.activate
  END

END

OBJECT "Lv45L4"
begin

  USE
  BEGIN
	Lv45UB2.use
  END

END


OBJECT "Lv45L5"
begin
   
  USE
  BEGIN
	Lv45UB1.use     
  END

END


OBJECT "Lv45L6"
begin

  USE
  BEGIN
	Lv45UB3.use
  END

END


OBJECT "Lv45L7"
begin

  USE
  BEGIN
	Lv45Trap1.use
  END

END

OBJECT "Lv45L8"
begin

  USE
  BEGIN
	Lv45UB2.use
  END

END

OBJECT "Lv45L9"
begin

  USE
  BEGIN	
	Lv45UB1.use
	Lv45UB2.use
	LV45UB3.use
  END

END

//script for Ogrok room

OBJECT "Mort"
begin

  CUBE Locke 9296,6480,0 9456,6400,144
  BEGIN

	if Lv45OgrokRoom <> 0
	  jump Finish

	control off
	Locke.stop
	wait 12
	Locke.say "I think I hear some voices over there."
	wait 24
	Locke.goto 9232 6352 16
	scrollto 8960 6288 16
	Locke.stop
	wait 24
	Urgg.say "I'm getting kind of hungry."
	Urgg.try bore5
	Slagg.face 140
	Slagg.say "Mort, go get us something to eat."
	face 20
	say "Go get it yourselves."
	Urgg.face 180
	Urgg.say "Don't make me hurt you Mort."
	face 50
	say "Bring it on."
	Urgg.say "Shhh. Quiet you guys, I think I smell something."
	Urgg.say "I know that smell.... human."
	Slagg.face 100
	Slagg.say "Yeah, I think your right, definitely a human."
	Slagg.try bore6
	face 20
	say "Let's get him!"
	Urgg.face 230
	Urgg.say "Yeah, let's get him!"
	wait 12
	Slagg.say "Now hold on just a second..."
	say "What is it?"
	Slagg.face 120
	Slagg.say "Well, we're reasonably intelligent monsters."
	try bore6
	Slagg.say "That human might have a nasty sword and powerful armor."
	Slagg.say "We don't have to mindlessly rush at every human that comes through here."
	wait 12
	Slagg.say "This isn't Diablo after all."
	say "Good point."
	Urgg.face 230
	Urgg.say "Makes sense to me."
	wait 12
	Slagg.say "If we hide in here maybe he won't notice us."
	say "Sounds like a plan."
	Urgg.try bore5
	Urgg.say "Count me in."
	wait 12
	Slagg.say "Shhhh.. everyone be real quiet."
	scrollto Locke
	wait 24
	Locke.face 96
	Locke.say "I can't believe what I just heard."
	Locke.say "I should kill them on principle alone."
	control on
	
	
	set Lv45OgrokRoom=1

	:Finish
  END 

END

//Sabu script, Room1

OBJECT "Sabu1"
begin

  CUBE Locke 8272,9520,0 8080,9712,144
  BEGIN
	
	control off
	Locke.stop
	
	scrollto 8040 9560 16
	wait 32

	say "Ahh, a visitor."
	say "We rarely get company down here, to what do I owe this pleasure?"
	
   	Locke.say "I am Locke. What would you have of me, priest?"
	
	say "Greetings Locke, I am Sabu."
	say "I am the overseer of these dungeons and high priest of the order."
	say "It has come to my attention that you might know something about a golden statue that was stolen from these dungeons."

    :Start

	Choice Admission "Are you referring to this gold statue?"
	Choice Denial "I don't know what you are talking about."
	wait response

    :Admission

	Locke.say choice
	
	say "My statue! You will return it to me immediately!"
	jump Finish

    :Denial

	Locke.say choice
	

	say "Please do not insult my intelligence."
	say "My vision reaches to all parts of these dungeons."
	say "You will return that statue to me at once!"
	jump Finish

    :Finish

	Locke.say "I take orders from no man!" 
	wait 15
	Locke.say "You want your precious statue?"
	wait 24
	Locke.say "Then come and get it."
	Locke.try combat
	
	say "As much as I would love to stay and play, I have pressing business elsewhere."
	say "But I assure you that you will not leave this dungeon alive."
	delinv Locke. statue
	scrollto Sabu
	play call_vortex
	try spell2
	add vortex 
	wait 36
	toggle invisible=1

	Lv45Barrier1.delete
	Lv45Barrier1.delete
	wait 40

	scrollto Locke
	Locke.try walk
	
	wait 36
	Locke.say "That bastard stole the statue."
	wait 24
	Locke.face 96
	Locke.say "He's going down."
	wait 32

	vortex.delete

	control on

	toggle pause=1

  END

END

//Sabu script, lever puzzle room

OBJECT "Sabu2"
begin

  CUBE Locke 8608,9088,144 8790,9060,0
  BEGIN	
	control off
	Locke.stop
	scrollto 8848 7984 16
	wait 96
	centeron Sabu2
	goto 8800 7952
	wait 36
	pivot 64
	say "This should hold our young friend for a while."
	face 60
	try lever2
	Lv45L9.use
	Lv45UB1.use
	Lv45UB2.use
	Lv45UB3.use
	wait 24
	goto 8816 8048
	wait 12
	face 120
	play call_vortex
	try spell2
	add vortex
	wait 36
	toggle invisible=1
	scrollto Locke
	wait 96
	centeron Locke
  	control on
	vortex.delete
	toggle pause=1
  END

END

//Sabu script, crossroads room

OBJECT "Sabu3"
begin

  CUBE Locke 8368,7120,144 8752,7408,0
  BEGIN
	control off
	scrollto Sabu3
	goto 8300 7208
	try lever1
	Lv45D5.use 
	//this door will be locked so Sabu must have all level keys//
	goto 8176 7216
	Lv45D5.use
	centeron Locke
	wait 48
	toggle invisible
	toggle pause=1
	control on      
  END

END

//Sabu script, monster trap room

OBJECT "Sabu4"
begin

  CUBE Locke 7312,7600,0 7216,7696,144
  BEGIN
	control off
	Locke.stop
	scrollto Sabu4
	say "This has been quite entertaining....."
	say "but I'm afraid this is the end of line."
	face 80
	wait 24
	try lever2
	Lv45L11.use
	wait 36
	play call_vortex
	try spell2
	add vortex
	wait 36
	toggle invisible=1

	scrollto Issathi1
	wait 24
	Lv45D8.use
	wait 24
	Issathi1.goto 7072 7952
  	Issathi1.pos 7072 7952 	
	wait 24

	scrollto Issathi2
	wait 24
	Lv45D7.use
	wait 24
	Issathi2.goto 7072 7696
	Issathi2.pos 7072 7696
	wait 24

	scrollto Locke
	wait 24
	vortex.delete
	toggle pause
	control on
  END

END

//Sabu script, Death of Sabu

OBJECT "Sabu5"
begin

  CUBE Locke 7344,8624,0 7440,8784,144
  BEGIN
	control off
	Locke.stop
	scrollto Sabu5
	face 60
	say "You again?"
	say "I've had just about enough of this."
	say "My Elemental attack will destroy you for good!"
	centeron Sabu5
	try cast
	wait 24
	say surprise "What? This is impossible!"
	say surprise "That talisman, it's rendered me powerless!"
	Locke.goto 7184 8640
	Locke.stop
	Locke.face 200
	say backpedal "Where did you...."
	Locke.say "Something the matter?"
	wait 24
	Locke.try combat
	say backpedal "Noooooooo!!!!!"
	Locke.try attack1
	try lay
	//add effect//
	Locke.try walk
	wait 72
	scrollto Locke
	Locke. face 76
	Locke.say "I can't let him die..."
	wait 24
	Locke.say "He knows where the statue is."
	Locke.goto 6848 8688
	wait 36
	Locke.say "Where is the statue?"
	wait 24
	say dying "I'll tell you nothing....(cough)....(choke)"
	wait 24
	Locke.say "Talk or it's your life!"
	wait 24
	say dying "I'll die regardless"
	say dying "Your threats are useless."
	wait 24
	Locke.say "I can make it quick for you, if you tell me where the statue is."
	wait 24
	say dying "You may have beaten me, but know this...(cough)
	say dying "You're quest is futile"
	say dying "To face Yaghoro you must first recover the Nahkranoth"
	say dying "But to wear the Nahkranoth means the destruction of your soul"
	Locke.say "You forget one small detail....."
	wait 24
	Locke.say "I have no soul to destroy."
	wait 36
	say dying "You will fail...."
	say dying "To face Yaghoro...is...to...face... a GOD!"
	try die
	Locke.wait 48
	Locke.face 76
	Locke.say "He's dead...."
	wait 24
	Locke.say "May his soul rot in hell."
	Locke.say "Now I've got to go find that statue!"
    control on
  END

END
