// Revenant - Copyright 1998 Cinematix Studios, Inc.//
//************** Revenant E3 Script File  ************// 


//Script for Self running Demo//
OBJECT "UrLocke"
BEGIN

  ALWAYS
  BEGIN
	wait 48
	pos 10699 8713 269 120
	wait 1
	ambcolor 50 60 200
	ambient 30
	wait 48
	goto 10432 9040
	goto 10144 9584
	goto 9952 9808
	goto 9984 9840
	pos 11520 10144 16 121
	wait 1
	ambcolor 0 0 255
	ambient 50
	goto 11183 10185
	goto 10774 10170
	goto 10411 9847
	goto 9998 9619 
	goto 10433 9373
	goto 10211 9201
	pos 4048 20160 32 1
	wait 1
	ambcolor 255 255 255
	ambient 30
	goto 3792 20146
	goto 3552 20400
	goto 3792 20146
	goto 4048 20160
	pos 10211 9201 16 121
	wait 1
	ambcolor 0 0 255
 	ambient 50
	goto 10697 8625
	goto 11445 8725
	pos 6144 2107 32 1
  END
END


//Script for Dungeon playable level//


//Crypt Room//

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



//script for lever puzzle room//

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




//script for Ogrok room//

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
	Slagg.face 140
	Slagg.say "Mort, go get us something to eat."
	face 20
	say "Go get it yourselves."
	Urgg.face 180
	Urgg.say "Don't make me hurt you Mort."
	wait 24
	face 50
	say "Bring it on."
	wait 24
	Urgg.say "Shhh. Quiet you guys, I think I smell something."
	wait 24
	Urgg.say "I know that smell.... human."
	Slagg.face 100
	Slagg.say "Yeah, I think your right, definitely a human."
	face 20
	say "Let's get him!"
	Urgg.face 180
	Urgg.say "Yeah, let's get him!"
	wait 30
	Slagg.say "Now hold on just a second..."
	say "What is it?"
	Slagg.face 120
	Slagg.say "Well, we're reasonably intelligent monsters."
	Slagg.say "That human might have a nasty sword and powerful armor."
	Slagg.say "We don't have to mindlessly rush at every human that comes through here."
	wait 24
	Slagg.say "This isn't Diablo after all."
	say "Good point."
	Urgg.face 230
	Urgg.say "Makes sense to me."
	wait 24
	Slagg.say "If we hide in here maybe he won't notice us."
	say "Sounds like a plan."
	Urgg.say "Count me in."
	wait 24
	Slagg.say "Shhhh.. everyone be real quiet."
	scrollto Locke
	wait 36
	Locke.face 96
	Locke.say "I can't believe what I just heard."
	Locke.say "I should kill them on principle alone."
	control on
	
	
	set Lv45OgrokRoom=1

	:Finish
  END 

END



//Sabu script, Room1//

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
	try spell1
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




//Sabu script, lever puzzle room//

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
	say "This should hold our young friend for a while"
	face 60
	try lever1
	Lv45L9.use
	Lv45UB1.use
	Lv45UB2.use
	Lv45UB3.use
	wait 24
	goto 8816 8048
	wait 12
	face 120
	try spell1
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




//Sabu script, crossroads room//

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
	toggle pause
	control on      
  END

END




//Sabu script, monster trap room//

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
	try spell1
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





//Sabu script, Death of Sabu//

OBJECT "Sabu5"
begin

  CUBE Locke 7280,8624,0 7376,8784,144
  BEGIN
	control off
	Locke.stop
	scrollto Sabu5
	say "You again?"
	say "I've had just about enough of this."
	say "My Elemental attack will destroy you for good!"
	try surprise
	say "What?"
	say "This is impossible!"
	say "That talisman, it's reversed my elemental spell!"
	say "Where did you...."
	say "Noooooooo!!!!!"
	//add effect//
	try lay
	wait 36
	scrollto Locke
	Locke. face 76
	Locke.say "I can't let him die..."
	wait 24
	Locke.say "He knows where the statue is."
	Locke.goto 7059 8565
	Locke.say "Where is the statue?"
	say dying "I'll tell you nothing....(cough)....(choke)
	Locke.say "Talk or it's your life!"
	say dying "I'll die regardless"
	say dying "Your threats are useless."
	Locke.say "I can make it quick for you, if you tell me where the statue is."
	wait char Locke
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






//Scripts for Townspeople//

OBJECT "Harowen"
begin

  DIALOG
  BEGIN
	Locke.say "Greetings."
	wait char Locke

	say "Greetings, stranger, and welcome to Illicit Goods."	
	say "My name is Harowen, the Black Hand."
	say nowait "What can I do for you?"
	
    :Start
	
	Choice Shop "Ask about the shop"
	Choice Tower "Ask about the tower"
	if Tab > 0
	   Choice Pay "Pay your bill"
	Choice Finish "Peruse the shop"
	wait response
	
    :Shop
    
	Locke.say "Tell me about your shop."
	wait char Locke
	
	say "I sell objects useful to those employed in stealthy professions."
	say nowait "If you'd like to know about a specific object, just point it out to me."
	jump Start
	
    :Tower
    
	Locke.say "Tell me about this tower."
	wait char Locke
	
	say "This abandoned tower is a good location for my shop, which would not be appreciated in the town."
	say "I don't know who built it, exactly, but it is quite secure."
	say nowait "This seems to be the only room, which seems strange - it looks much larger from the outside."
	jump Start
	
    :Pay
    
    	Locke.say "I'd like to buy this."
	wait char Locke
	Locke.PayTab
	
	if Tab < 1
	   say nowait "Thanks for your patronage.  Be sure to not tell your friends about us."
	else
	   say nowait "Sorry pal - no money, no goods."
	jump Start
	
    :Finish
    	Locke.say "I'm just going to look around for a bit."
	wait char Locke
	
	say nowait "Very well."
  END

END



OBJECT "SkinnyGuy"
begin
 
  DIALOG
  BEGIN
	Locke.say "Greetings."
	try stand	
	wait char Locke
	
	say "Well met.  What can I do for you?"
	
    :Start
	
	Choice Shop "Ask about the shop"
	Choice Ogrok "Ask about the Ogrok"
	if Tab > 0
	   Choice Pay "Pay your bill"
	Choice Finish "Peruse the shop"
	wait response
	
    :Shop
    
   	Locke.say "Tell me about your shop."
	wait char Locke
	 
    	say "I sell weapons and armor of all types."
	jump Start
	
    :Pay
    
    	Locke.say "I'd like to buy this."
	wait char Locke
	Locke.PayTab
	
	if Tab < 1
	   say nowait "I hope it serves you well."
	else
	   say nowait "I'm afraid you seem to be short on funds."
	   
	jump Start
	
    :Ogrok

	Locke.say "Rubold said you know something about the Ogrok."
	wait char Locke
	
	say "Yes, I ran into one once."
	say "It's a good thing they are so slow, else I doubt we would be having this conversation."
	say "Stay clear of the dungeon and you should be safe."
	
	jump Start 
   		
    :Finish
    	Locke.say "I'm just going to look around for a bit."
	wait char Locke
	
	say nowait "Very well."
  END

END

OBJECT "FatMan"
begin

  DIALOG
  BEGIN
	Locke.say "Greetings."
	wait char Locke
	
	say "Greetings, traveller, and welcome to the Drunken Whore Tavern!"
	say "My name is Rubold.  What can I get for you?"
	
    :Start

	Choice Tavern "Ask about the tavern"
	Choice Ring "Ask about his stolen ring"
	if Tab > 0
	   Choice Pay "Pay your bill"
	Choice Finish "Peruse the shop"
	wait response
	
    :Tavern
	
	Locke.say "Tell me about this tavern."
	wait char Locke
	
	say "Well, I do fairly well here - normally."
	say "But lately things have been rather slow."
	say "It all started happening after my lucky ring was stolen..."
	jump Start

    :Ring
	
	Locke.say "Someone stole your lucky ring?"
	wait char Locke
	
	say "Yes, I'm quite disturbed about it.  It's a family heirloom, you see."
	say "I'm positive it must have been those filthy Ogrok."
	say "I've posted a reward of 300 gold coins, but no one's even inquired about it."
	say "You look like you can handle yourself - you interested?"
	
	Choice Yes "Yes"
	Choice No "No"
	wait response
	
	    :Yes
	    	Locke.say "For 300 coins, it's worth a try.  What does the ring look like?"
		wait char Locke
		say "It is a gold band shaped like flame, set with a single fire-ruby."
		Locke.say "I see.  And what about these Ogrok?"
		wait char Locke
		say "I don't know much, I'm afraid.  Most folk steer well clear of them."
		say "Hrothfuurd the Smith claims to know something about them, ask him."
		Locke.say "Very well."
		say "Well then, anything I can get your before your journey?"
		jump Start
		
	    :No
	    	Locke.say "Find it yourself, fat boy."
		wait char Locke
		say nowait "It was just an offer, sir.  Forget I mentioned it."
		jump Start
	
    :Pay
    
    	Locke.say "I'd like to buy this."
	wait char Locke
	Locke.PayTab
	
	if Tab < 1
	   say "There you go."
	else
	   say "Hey - no money, no service."
	   
	jump Start
	
    :Finish
    	Locke.say "Farewell."
	wait char Locke
	
	say nowait "Thanks for visiting the Drunken Whore.  Come again soon."

  END

END
