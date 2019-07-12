CONTEXT TestObj
begin
    TEST
        say "Hello, I'm a tree."
end

RACE DRUHG
begin
    PROXIMETY Locke
    begin
        if group.size > 4
        begin
            say "A human!  Get 'im!"
            group.attack Locke
        end
        else
        begin
            say "Run away, run away!"
            group.flee
        end
    end

    TIME 2pm
    begin
        goto Dinertable
        pickup [type=food]
        if success
        begin
            eat
            say "Mmmm!  Me likums them foods!"
        end
        else
            say "Me hungry.  Needs them foods!"
    end
end

CONTEXT Gordo
begin
    proximety locke
    begin
        random
        begin
            say "Argh!  Me gonna kill you."
            say "You dead meat now."
            say "I say, old chap, ready to get the shit beat out of you?"
            begin
                say "Grrrrr!"
                playanim beat chest
                playsound snarl
            end
        end
        attack locke
    end
end
