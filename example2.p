CONTEXT Ryu
begin
    TEST
    begin
        goto 720 500

        if RyuIntroduced
           say "You already know my name, so I won't repeat it."
        else
        begin
           say "Hi, my name is Ryu."
           set RyuIntroduced 1
        end

        set i 0
        while i < 3
        begin
           say "!"
           set i i+1
        end

        say "I'm one bad dude."
        say "I think I'm going to stand over here, now."
        goto 900 400
        say "Hmmm, funny, this isn't all that different from that other spot."
        goto 800 600
        say "Ah, fuck it.  Time to repeat my script, I guess."
    end
end

