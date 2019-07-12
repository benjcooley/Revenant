// ************** Revenant E3 Script File  ************// -------------------------------------------------

// Revenant - Copyright 1998 Cinematix Studios, Inc.

OBJECT "Lv45D8"



OBJECT "Lv45L11"
begin
  USE
  BEGIN
	Lv45D7.use
	Lv45D8.use
  END

END


OBJECT "Lv45D8"
begin

  USE  
  BEGIN
	Rex.goto 7120, 7984, 16
  	Rex.move 7120,7984, 16
  END

END


OBJECT "Lv45D7"
begin

  USE  
  BEGIN
	Karl.goto 7120, 7984, 16
	Karl.move 7120, 7984, 16
  END

END



OBJECT "Horatio"
begin

  CUBE Locke 7216,6448,0 6928,5760,144
  BEGIN
	control off
	Locke.goto 7024, 6064, 16
	wait char Locke
	scrollto Horatio
	//Teleport effect or other creative transition goes here//
	goto 6835, 6087, 145
	move 6835, 6087, 145
	say "Your majesty, this is an unexpected surprise."
	wait char Horatio

	Locke.say "Do I know you spirit?"
	wait char Locke

	say "Have you forgotten the pain and suffering you caused?"
	say "The destruction of an empire caused by your actions?"
	wait char Horatio

	Locke.say "I know nothing of these things."
	wait char Locke

	say "It is a shame that you are not haunted by the screams of the thousands who lost their lives as a result of your ignorance."
	wait char Horatio

	Locke.say "I don't know who you are or what you speak of but I tire of this conversation." 
	wait char Locke
	Locke.say "Either return from where you came or be prepared to be sent back there permanently."
	wait char Locke

	say "It is you who must go back, and you shall burn eternally for the lives you sacrificed for your own selfish desires."
	
  END	

END

//Locke gets the talisman in the chest for defeating Horatio//
//End Crypt room script//


OBJECT "LV42L1" 
begin
  USE
  BEGIN
	LV45UB1.use
  END

END
//END Lever puzzle room script//

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
	Lv45UB2.use
  END

END

OBJECT "Lv45L4"
begin
  USE
  BEGIN
	Lv45Trap2.use
  END

END

//L3 sets UB2 to the up position//
OBJECT "Lv45L5"
begin
   
  USE
  BEGIN
	Lv45UB1.use     
  END

END

//L4triggers trap2//
OBJECT "Lv45L6"
begin

  USE
  BEGIN
	Lv45UB3.use
  END

END

//L5 sets UBUB1 to the up position// 
OBJECT "Lv45L7"
begin

  USE
  BEGIN
	Lv45Trap1.use
  END

END

//L6 sets UB3 to the down position//
OBJECT "Lv45L8"
begin
  
  USE
  BEGIN
	Lv45UB2.use
  END

END

//L7 triggers arrow wall trap?//
OBJECT end

OBJECT "Lv45L9"
begin
  BEGIN	
	Lv45UB1.use
	Lv45UB2.use
	LV45UB3.use
  END

END

//L8 sets UB2 to the down position//
OBJECT end

OBJECT "Lv45L10"
begin
  BEGIN

	if Lv45L10.state <> 1			
      jump Finish

	if Lv45OgrokRoom = 1
	  jump FirstSwitch

	if Lv45OgrokRoom = 2
	  jump SecondSwitch
	
	if Lv45OgrokRoom = 3
	  jump ThirdSwitch
	
	if Lv45OgrokRoom = 4
	  jump Finish	

	jump Finish

    :FirstSwitch
	control off
	scrollto 8960, 6304, 16
	group1.light off
	Urgg.say "Whose messing with the lights?"
	say "The switch is down the hall"
	Slagg.say "What's going on here?"
	Urgg.say "Someone check the switch."
	say "Slagg, go check the switch."
	Slagg.say "Uh.... Urgg, go check the switch."
	Urgg.say "Uh O.K., watch my back all right?"
	Urgg.goto 9328,6256,16
	Urgg.move 9328,6256,16
	Lv45L10.state 3
	group1.light on
	control on
	centeron Locke
	set Lv45OgrokRoom = 2
	goto Finish

  

    :SecondSwitch      	
	control off
	Scrollto 8960, 6304, 16
	say "There go the lights again."   
	Slagg.say "Where's Urgh?"
	say "He's probably messing with us."
	Slagg.say "You think he's all right?"
	say "I don't know, why don't you check it out?"
	Slagg: "Right."
	Slagg.goto 9328,6256,16
	Slagg.move 9328,6256,16
	Lv45L10.state 3
	group1.light on
	control on
	centeron Locke
	set Lv45OgrokRoom = 3
	goto Finish

	scrollto Locke

  

    :ThirdSwitch
	control off

	scrollto 8960 6304 16

	say "Uh oh Lights again."
	say "URGH???? SLAGG????"
	say "I'm going to kill those guys for this."
	say "I guess I better go and check things out."
	goto 9328,6256,16
	move 9328,6256,16
	Lv45L10.state 3
	group1.light on
	control on
	centeron Locke
	set Lv45OgrokRoom = 4
	goto Finish
	
	scrollto Locke

    :Finish	
  END

END




OBJECT "Mort"
begin
Locke 9328,6352,0 9232,6352,144
  BEGIN

	if Lv45OgrokRoom <> 0
	  jump Finish

	control off
	scrollto 8960,6304,16
	Urgg.say "Someone go get food."
	Slagg.say "Mort, go get us something to eat."
	say "Go get it yourselves."
	Urgg.say "Don't make me hurt you Mort."
	say "Bring it on."
	nowait try //hit animation//
	Urgg.try //hit animation//
	Urgg.say "Shhh. I think I smell something."
	say "I don't smell anything."
	wait 40
	play //"fart SFX"//
	say "I smell it now!"
	nowait Slagg.say "HAHAHAHA!!!!"
	nowait Urgg.say  "HAHAHAHA!!!!"
	say "HAHAHAHA!!!!"
	Urgg.say "Seriously guys, I smell human. Shhhh.."
	Slagg.say "Wait, I smell it too."
	control on
	scrollto locke
	
	set Lv45OgrokRoom=1

	:Finish
  END 

END





OBJECT "StoneGiant2"
begin
  BEGIN
    if StoneGiant1.Health <= 0
      Sabu.trigger SavedByLocke
  END

END

//Script for OgrokRoom puzzle area//
OBJECT end

OBJECT "StoneGiant1"
begin
  BEGIN
    if StoneGiant2.Health <= 0
      Sabu.trigger SavedByLocke
  END

END



OBJECT "Sabu5"
begin
Locke 7216,8656,0 7216,6752,144
  BEGIN
	control off
	scrollto Sabu5
	say "You again?"
	say "I've had just about enough of this."
	say "My Stone Golems will destroy you for good!"
	nowait StoneGuy1.try attack1
	nowait StoneGuy2.try attack1
	try //High Priest hit animation//
	try dying
	say "What?"
	say "This is impossible!"
	say "That talisman, it's reversed my control spell!"
	say "HELP ME!!!!!!!!"

	scrollto Locke
	Locke. face 76
	Locke.say "I can't let them kill him...."
	wait char Locke
	Locke.say "He knows where the statue is."
	Locke.try combat
	control on
  END

  TRIGGER SavedByLocke
  BEGIN 
	control off
	Locke.goto Sabu
	Locke.say "Where is the statue?"
	say I'll tell you nothing....(cough)....(choke)
	Locke.say "Talk or it's your life!"
	say "I'll die regardless"
	say "Your threats are useless."
	Locke.say "I can make it quick for you, if you tell me where the statue is."
	wait char Locke
	say "You may have beaten me, but know this...(cough)
	say "You're quest is futile"
	say "To face Yaghoro you must first recover the Nahkranoth"
	say "But to wear the Nahkranoth means the destruction of your soul"
	Locke.say "You forget one small detail....."
	Locke.wait 24
	Locke.say "I have no soul to destroy."
	say "You will fail...(choke)...(cough)...."
	say "To face Yaghoro...is (hack)....(wheeze)..to...face... a GOD!"
	try death
	Locke.wait 48
	Locke.face 76
	Locke.say "He's dead.."
	wait char Locke
	Locke.say "May his soul rot in hell."
	wait char Locke
	Locke.say "Now I've got to go find that statue!"
        control on
  END

END



OBJECT "Sabu4"
begin
Locke 7312,7600,0 7216,7696,144
  BEGIN
	control off
	scrollto Sabu4
	try //lever pull animation//
	use Lv45L10
	goto 7024, 8464, 16
	move 7024, 8464, 16
	scroll to Locke
	toggle invisible
	toggle pause
	control on
  END

END




OBJECT "Sabu3"
begin
Locke 8368,7408,0 8752,7408,144
  BEGIN
	control off
	scrollto Sabu3
	goto 8368,7216,16
	move 8368,7216,16
	try //open animation//
	use Lv45D5 
	//this door will be locked so Sabu must have all level keys//
	goto 8560, 6640, 16
	move 8560, 6640, 16
	centeron Locke
	toggle invisible
	toggle pause
	control on      
  END

END



OBJECT "Sabu2"
begin
Locke 8656,9040,0 8752,9040,144
  BEGIN	
	control off
	Locke.stop
	scrollto Sabu2
	goto 8848,7984, 16
	move 8848,7984, 16
	try //pull lever animation//
	use Lv45L9
	goto 8464, 7120, 16
	move 8464, 7120, 16
	centeron Locke
	toggle invisible
	toggle pause
    control on
  END

END



OBJECT "Sabu1"
begin
Locke 8272,9520,0 8080,9712,144
  BEGIN
	
	control off
	Locke.stop
	
	scrollto 8080 9600 16

	wait 64

	say "Ahh, a visitor."
	say "We rarely get company down here, to what do I owe this pleasure?"
	
    Locke.say "I am Locke. What would you have of me, priest?"
	
	say "Greetings Locke, I am Sabu."
	say "I am the overseer of these dungeons and high priest of the order."
	say "It has come to my attention that you might know something about a statue that was stolen."

    :Start

	Choice Admission "Are you referring to this gold statue?"
	Choice Denial "I don't know what you are talking about."
	wait response

    :Admission

	Locke.say "Are you referring to this gold statue?"
	
	say "My statue! You will return it to me immediately!"
	jump Finish

    :Denial

	Locke.say "I don't know what you are talking about."
	

	say "Please do not insult my intelligence."
	say "My vision reaches to all parts of these dungeons."
	say "You will return that statue to me at once!"
	jump Finish

    :Finish

	Locke.say "I take orders from no man!" 
	Locke.say "You want your precious statue?"
	try combat
	wait 24
	Locke.say "Then come and get it."

	say "As much as I would love to stay and play, I have pressing business elsewhere."
	say "I assure you that you will not leave this dungeon alive."
	add heal 8048,9616,16
	wait 24
	toggle invisible
	toggle pause

	Lv45Barrier1.delete
	Locke.delinv Statue
	scrollto Locke
	
	Locke.say "That bastard stole the statue."
	wait 32
	Locke.face 79
	Locke.say "He's going down"
	
	control on

  END

END


OBJECT "Harowen"
begin
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
	
	say nowait "Thanks for visiting the Drunken Boar.  Come again soon."

  END

END


OBJECT "Locke"
begin
9950,9840,0 10028,9770,100
  BEGIN
    control off
	goto 9990 9805
	face 80
	say "Hey, isn't this supposed to go somewhere?"
	wait 48
	say "Well, I guess not."
	control on
  END

END


