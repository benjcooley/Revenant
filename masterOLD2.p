// Revenant - Copyright 1998 Cinematix Studios, Inc.//
//************** Revenant E3 Script File  ************// 


//Script for Self running Demo//
OBJECT "Locke"
BEGIN

  ALWAYS
  BEGIN
	wait 720
    :loop
	pos 10624 8752 270 120
	goto 10864 8768
	goto 11104 8752
	goto 11168 8896
	goto 11024 9072
	goto 10848 9008
	goto 10768 8832
	goto 10528 8848
	goto 10464 9024
	goto 10476 9208
	goto 10192 9552
	goto 10016 9584
	goto 10064 9840
	goto 10144 10128
	goto 9888 10080
	pos 11520 10144 16 121
	wait 1
	goto 11183 10185
	goto 10774 10170
	goto 10411 9847
	goto 9998 9619 
	goto 10433 9373
	goto 10211 9201
	pos 4048 20160 32 1
	wait 1
	goto 3792 20146
	goto 3552 20400
	goto 3792 20146
	goto 4048 20160
	pos 10211 9201 16 121
	wait 1
	goto 10697 8625
	goto 11469 8737
	goto 11737 8875
	pos 10000 10000 16 45
	wait 1
	jump loop
  END

END


END
