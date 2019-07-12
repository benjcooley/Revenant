//Alpha Demo Script File for Town and Ruins level

OBJECT "bhstu"
begin

	USE
	BEGIN
		Locke.pos 7248 12224 30 20
	END
end

OBJECT "bhstd"
begin

	USE
	BEGIN
		Locke.pos 7482 11690 30 20
	END
end

OBJECT "bhexit"
begin

	USE
	BEGIN
		Locke.pos 10592 9860 38 20
	END
end

OBJECT "bhenter"
begin

	USE
	BEGIN
		Locke.pos 7624 11653 30 20
	END
end

OBJECT "shopenter"
begin

	USE
	BEGIN
		Locke.pos 8594 11836 16 20
	END
end

OBJECT "shopexit"
begin

	USE
	BEGIN
		Locke.pos 10442 9440 36 20
	END
end

OBJECT "shenter"
begin

	USE
	BEGIN
		Locke.pos 8260 9662 16 20
	END
end

OBJECT "shexit"
begin

	USE
	BEGIN
		Locke.pos 10832 9516 37 20
	END
end

OBJECT "ruinenter"
begin

	USE
	BEGIN
		Locke.pos 5108 5404 65 20
	END
end

OBJECT "ruinexit"
begin

	USE
	BEGIN
		Locke.pos 16119 8373 97 20
	END
end

OBJECT "caventer"
begin

	USE
	BEGIN
		Locke.pos 10547 10467 16 30
	END
end

OBJECT "Rubold"
begin

	DIALOG
	BEGIN
	control off
	Locke.stop
	scrollto Rubold
	wait 24
	say "Welcome to Misthaven."
	say "My name is Rubold"
	Locke.say "Greetings Rubold, I am Locke."
	say "A pleasure to meet you Locke."
	Locke.say "I'm going to be needing a weapon."
	say "I sell only the finest handcrafted weaponry.
	say "Right now I have a sword, a battle axe, and a morning star."
	say "I can only part with one, so choose wisely."
	
	:Start

	Choice sword "I'll take the sword."
	Choice battle axe "I'll take the battle axe."
	Choice morning star "I'll take the morning star."
	wait response

	:sword 
	Locke.say choice
	
	say "A fine choice."
	say "It will serve you well."
	Locke.addinv "sword"
	jump finish
	
	:battle axe
	Locke.say choice

	say "A fine choice."
	say "It will serve you well."
	Locke.addinv "Battle Axe"
	jump finish

	:morning star
	Locke.say choice

	say "A fine choice."
	say "It will serve you well."
	Locke.addinv "MorningStar"
	jump finish

	:Finish
	Locke.say "Now that I'm armed, where can I go for some action?."
	say "If it's adventure you are looking for, you should speak to Rand."
	Locke.say "Thank you for your help."
	say "It was my pleasure!"
	scroll to Locke
	control on 
	toggle pause=1
	END
end

OBJECT "Rand"
begin
	
	DIALOG
	BEGIN
	control off
	Locke.stop	
	scrollto Rand	
	wait 12
	try stand
	say "Welcome to my home."
	say "I am Rand."
	Locke.say "I am Locke."
	say "It appears you have come to Ahkuilon for some adventure."
	say "I think I have just the quest for you."
	Locke.say "Excellent."
	say "To the south lies the Ruins of Tullford."
	say "Inside those ruins is an entrance to a vast network of underground caverns."
	say "It is rumored that a legendary sword by the name of Angsaar lies deep within these caves."
	say "Beware however, for the creatures that roam Ahkuilon are vicious."
	say "And lord knows what vermin live in those caves."
 	say "Make sure you arm yourself and learn the ways of magic before you set out."
	Locke.say "I will take your advice."
	Locke.say "Thank you for your help Rand."
	say "Certainly."
	say "Good luck to you out there."
	try sit
	scrollto Locke
	control on
	toggle pause=1
	END
end

OBJECT "Jong"
begin
	DIALOG
	BEGIN
	control off
	Locke.stop	
	scrollto Jong	
	wait 24
	say "You have come seeking the ways of magic, no?"
	Locke.say "That's right."
	Locke.say "Can you teach me what I need to know?"
	say "But of course!"
	say "The key to all magic is talismans"
	say "By collecting talismans and combining them properly, you can become quite a powerful mage."
	
	:Start

	Choice Talisman "Where can I find these talismans?"
	Choice Combining "How do I combine these talismans into spells?"
	Choice Done "I think I understand."
	wait response	
	
	:Talisman
	Locke.say choice

	say "I have a few I can give you, the rest you'll have to find on your own."
	jump start

	:Combining
	Locke.say choice 
	
	say "You can take the scroll in that chest over there."
	say "It has some basic spell combinations for you to use."
	jump start

	:Done
	Locke.say choice
	
	say "If you have trouble, just refer to the spell scroll."
	say "It has a complete explanation of how to use magic properly."
	jump finish
	
	:Finish
	
	say "Remember to take the scroll and the talismans in that chest by the bed before you leave."
	Locke.say "Thank you for all your help."
	say "It is always a pleasure to pass my wisdom on to others."
	say "Good luck to you!"
	scrollto Locke
	control on
	toggle pause=1
	END
end




//Alpha Demo Script File for Cave level

//Cave exit to Ruins

OBJECT "cavstart"
begin
	USE
	BEGIN
	pos 5088 4928 98 20
	END
end

//Guardian script

OBJECT "Guardian"
begin

	DIALOG
	BEGIN
	control off
	Locke.stop
	scrollto Guardian
	wait 12
	say "Greetings traveller."
	say "Here lies the burial ground of the ancients"
	say "Survive the horrors that lie within and the legendary blade of Angsaar will be yours."
	wait 24
	toggle invisible=1
	gtel.delete

	scrollto Locke
	control on
	toggle pause=1
	END
end

//Cave exit to town

OBJECT "cavexit"
begin
	USE
	BEGIN
	Locke.pos 10607 8101 16 20
	END
end




