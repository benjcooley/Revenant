OBJTYPE FatMan
begin
DIALOG
begin
	Locke.say "Greetings."
	wait char Locke
	
	say "Greetings, traveller, and welcome to the Drunken Boar Tavern!"
	say nowait "My name is Rubold.  What can I get for you?"
	
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
	say nowait "It all started happening after my lucky ring was stolen..."
	jump Start

    :Ring
	
	Locke.say "Someone stole your lucky ring?"
	wait char Locke
	
	say "Yes, I'm quite disturbed about it.  It's a family heirloom, you see."
	say "I'm positive it must have been those filthy Ogrok."
	say "I've posted a reward of 300 gold coins, but no one's even inquired about it."
	say nowait "You look like you can handle yourself - you interested?"
	
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
		say nowait "Well then, anything I can get your before your journey?"
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
	   say nowait "There you go."
	else
	   say nowait "Hey - no money, no service."
	   
	jump Start
	
    :Finish
    	Locke.say "Farewell."
	wait char Locke
	
	say nowait "Thanks for visiting the Drunken Boar.  Come again soon."
	
end

TAKE
begin
	if IsShopObject
	begin
	    say "Never travel with an empty stomach, I always say."
	end
end

DROP
begin
	if IsShopObject
	   say "What, not good enough for you?"
end

end

OBJTYPE SkinnyGuy
begin


DIALOG
begin
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
end

TAKE
begin
	if IsShopObject
	begin
	    say "Such an item would aid you greatly in combat."
	end
end

ALWAYS
begin
	try sit
end

end

OBJTYPE Harowen
begin
DIALOG
begin
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
end

TAKE
begin
	if IsShopObject
	begin
	   if Name = "Vial"
	   begin
	      say "Perfect for quick, clean assassination of any foe."
	      say "Just empty the vial onto any short blade and then place said blade between your victim's ribs."
	   end
	   else
	   begin
	      if Name = "Lockpicks"
	         say "A skilled person can use these to gain entry to locked chests and doors."
	   end	 
	end
end

DROP
begin
	if IsShopObject
	   say "No?  A pitty - you never know when it could come in handy."
end

end

OBJTYPE Ogrok2
begin
DIALOG
begin
	Locke.say "Hey, you look pretty good in blue."
	wait char Locke
	
	say "What are you, queer?"
	say "Sorry pal, I'm straight."
	say nowait "Sorry but homophobia runs pretty rampant down here."

	:start

	Choice bathhouse "Ask about a Bath House"
	Choice leatherpants "Ask about leather pants"
	Choice anallube "Ask about anal lube"
	Choice finish "Been nice talking to you"
	wait response
	
	:bathouse

	Locke.say "You know of any good bathouses around here?"
	wait char Locke

	say "Listen buddy, I already told you I'm NOT GAY!!!"
	jump start

	:leatherpants

	Locke.say "I'm interested in some tight fitting, open seat leather pants."
	wait char Locke
	
	say "Jesus pal, I've heard of flaming homosexuals before but you take the cake."
	jump start

	:anallube

	Locke.say "My ass is killing me, ya got any anal lube?"
	wait char Locke

	say "Sure thing man, me and the wife like a little rear entry from time to time."
	jump start

	:finish

	say "Yeah man, a real pleasure."
  end

end


OBJTYPE PressPlate
begin
ACTIVATE
begin
	ArrowWallS.use
	ArrowWallE.use
	UpBlock.use
end	


end

