CLASS "WEAPON"
begin


	STATS
	begin
		"EqSlot" 0x3 0x0 0x8
		"Combining" 0x0 0x0 0x10
	end


	TYPES
	begin
		"Club" 395
		"Stone Axe" 145
		"Bone Pickaxe" 389
		"Sword" 174
		"Dagger" 177
		"Spiked Handaxe" 398
		"Flanged Mace" 386
		"Battle-Axe" 277
		"Morning Star" 247
		"Shortsword" 384
		"Broadsword" 391
		"Hand-and-a-Half Sword" 396
		"Horseman's Flail" 280
		"Maul" 143
		"Tulwar" 390
		"Katana" 314
		"Flamberge" 316
		"Dwarven Axe" 248
		"Valkyrie's Sword" 249
		"Golden Morningstar" 278
		"Jeweled Dagger" 383
		"Writhing Kris" 393
		"Charred Claymore" 392
		"Berserker Sword" 394
		"Blackened Axe" 388
		"Blackened Morningstar" 279
		"Zikastar" 144
		"Serpentine Dagger" 318
		"Charred Battlebow" 387
		"Axe of Wrath" 381
		"Mercurous" 245
		"Deathwreaker" 317
		"Pommel" 894 Combining = 0x1
		"Crosspiece" 895 Combining = 0x2
		"Hilt" 896 Combining = 0x1
		"Blade" 897 Combining = 0x2
		"Angsaar" 175
	end

end

CLASS "ARMOR"
begin


	STATS
	begin
		"EqSlot" 0x4 0x0 0x8
		"Combining" 0x0 0x0 0x10
	end


	TYPES
	begin
		"Sigil Ring" 146
		"Serpent Ring" 148
		"Gloves of Thieving" 149
		"Cestus of Slicing" 176
		"Wyrmtooth Necklace" 376 EqSlot = 0x0
		"Emerald Ring" 320
		"Emerald Bracelet" 321
		"Emerald Collar" 322 EqSlot = 0x0
		"Amulet of Thunderbolts" 323 EqSlot = 0x0
		"Wolfsbane Pendant" 324 EqSlot = 0x0
		"Leather Vest" 325 EqSlot = 0x1
		"Shadowsilk Shirt" 326 EqSlot = 0x1
		"Chainmail" 363 EqSlot = 0x1
		"Scale Mail" 364 EqSlot = 0x1
		"Studded Vest" 365 EqSlot = 0x1
		"Hauberk" 366 EqSlot = 0x1
		"Breastplate" 367 EqSlot = 0x1
		"Field Plate" 271 EqSlot = 0x1
		"Spiked Garde" 368 EqSlot = 0x1
		"Ancient Armor of Wizardry" 369 EqSlot = 0x1
		"Icen Band" 375
		"Fireruby Ring" 377
		"Blacken Ring" 380
		"Twindragon Bracelet" 382
		"Shadowsilk Boots" 179 EqSlot = 0x8
		"Ancient Boots of Wizardry" 180 EqSlot = 0x8
		"Leather Boots" 181 EqSlot = 0x8
		"Climbing Boots" 182 EqSlot = 0x8
		"High Boots" 183 EqSlot = 0x8
		"Spiked-Toed Boots" 184 EqSlot = 0x8
		"Amulet of Channeling" 147 EqSlot = 0x0 Combining = 0x1
		"Staff" 849 EqSlot = 0x2 Combining = 0x1
		"Staff of Channeling" 850 EqSlot = 0x2
	end

end

CLASS "TALISMAN"
begin


	TYPES
	begin
		"Law" 150
		"Sun" 151
		"Soul" 152
		"Life" 153
		"Ocean" 154
		"Stars" 155
		"Death" 352
		"Chaos" 353
		"Sky" 354
		"Earth" 355
		"Ward" 356
		"Moon" 357
		"Rubert" 254
		"Gilmor" 255
		"Barry" 256
	end

end

CLASS "FOOD"
begin


	TYPES
	begin
		"Meat" 242
		"Cheese" 243
		"Apple" 327
		"Bread" 328
		"Grapes" 329
		"Bottle" 359
		"Glass" 360
		"Fish" 361
		"Mug" 372
		"Watermelon" 373
		"Celery" 374
	end

end

CLASS "CONTAINER"
begin


	TYPES
	begin
		"Chest" 244
		"Pouch" 358
		"Vial" 281
		"Spell Pouch" 583
	end

end

CLASS "LIGHTSOURCE"
begin


	STATS
	begin
		"EqSlot" 0x4 0x0 0x6
	end


	TYPES
	begin
		"Lantern" 319 EqSlot = 0x2
		"Torch" 178 EqSlot = 0x2
	end

end

CLASS "TOOL"
begin


	TYPES
	begin
		"Lockpicks" 370
		"Galvorn Lockpicks" 371
	end

end

CLASS "MONEY"
begin


	TYPES
	begin
		"Coin" 362
	end

end

CLASS "TILE"
begin


	STATS
	begin
		"Code" 0xffffffff 0x0 0xffffffff
		"Extra" 0xffffffff 0x0 0xffffffff
		"Supertile" 0x0 0x0 0x1
		"Width" 0x0 0x0 0x10
		"Height" 0x0 0x0 0x10
	end


	TYPES
	begin
		"Light" 125
		"Brazier" 227
		"Street" 134
		"sidewalkSWin" 162
		"sidewalkSEin" 163
		"sidewalkNWin" 164
		"sidewalkNEin" 165
		"sidewalkSWinT" 166
		"sidewalkSEinT" 167
		"sidewalkNWinT" 168
		"sidewalkNEinT" 169
		"sidewalkE2" 190
		"sidewalkE1" 191
		"sidewalkN2" 192
		"sidewalkN1" 193
		"sidewalkS2" 194
		"sidewalkS1" 195
		"sidewalkW2" 196
		"sidewalkW1" 197
		"sidewalkNE" 198
		"sidewalkNW" 199
		"sidewalkSE" 200
		"sidewalkSW" 201
		"sidewalkW2" 202
		"sidewalkN1" 203
		"sidewalkN2" 204
		"sidewalkW1" 205
		"sidewalkNs" 206
		"sidewalkEs" 207
		"sidewalkSs" 208
		"sidewalkWs" 209
		"sidewalkNw" 210
		"sidewalkEw" 211
		"sidewalkSw" 212
		"sidewalkWw" 213
		"tierSE" 101
		"tierS2" 102
		"tierS1" 103
		"tierSW" 104
		"tierE2" 105
		"tierE1" 106
		"tierNE" 107
		"tierW1" 108
		"tierW2" 109
		"tierN1" 110
		"tierN2" 111
		"tierNW" 112
		"tier" 113
		"blankwallE" 401
		"blankwallS" 423
		"chimney" 402
		"doorwayE" 403
		"doorwayS" 404
		"pillarSW" 405
		"pillarSE" 406
		"pillarNE" 424
		"pillartopS" 407
		"pillartopE" 408
		"stonewallS" 409
		"stonewallS2" 410
		"stonewallE" 411
		"winwallS" 412
		"winwallS2" 413
		"winwallS3" 414
		"winwallE" 415
		"winwallE2" 416
		"winwallE3" 417
		"winopenwallS" 418
		"winopenwallS2" 419
		"winopenwallE" 420
		"winopenwallE2" 421
		"gableroofE" 422
		"gableroofS" 425
		"blankwallEp" 401
		"blankwallSp" 453
		"doorwayEp" 403
		"doorwaySp" 434
		"pillarSWp" 405
		"pillarSEp" 406
		"pillarNEp" 424
		"pillartopSp" 407
		"pillartopEp" 408
		"stonewallSp" 439
		"stonewallS2p" 440
		"stonewallEp" 411
		"winwallSp" 442
		"winwallS2p" 443
		"winwallS3p" 444
		"winwallEp" 415
		"winwallE2p" 416
		"winwallE3p" 417
		"winopenwallSp" 448
		"winopenwallS2p" 449
		"winopenwallEp" 420
		"winopenwallE2p" 421
		"gableroofEp" 422
		"gableroofSp" 455
		"insidewallW" 274
		"insidewallN" 275
		"floor" 509
		"cutawayS" 234
		"cutawayE" 235
		"cutawaySE" 236
		"cutawaySW" 332
		"cutawayNE" 333
		"cutawayDoorW" 283
		"cutawayDoorN" 284
		"weaponwallN" 237
		"weaponwallW" 238
		"barrelwallW" 239
		"barrelwallN" 240
		"bakerwallW" 298
		"bakerwallN" 299
		"arrowwallW" 300
		"arrowwallN" 301
		"doorframeW" 302
		"doorframeN" 303
		"toolwallW" 307
		"toolwallN" 308
		"potwallW" 309
		"potwallN" 310
		"armorwallW" 506
		"armorwallN" 505
		"windowwallW" 311
		"windowwallN" 312
		"windowwall2W" 334
		"windowwall2N" 335
		"stairsN" 287
		"stairsW" 288
		"postW" 345
		"postN" 346
		"postSW" 612
		"postNW" 613
		"postNE" 614
		"fireplaceW" 347
		"fireplaceN" 348
		"stovewallW" 349
		"stovewallN" 350
		"stoveW" 232
		"stoveN" 233
		"ovenW" 230
		"ovenN" 231
		"tableW" 241
		"tableN" 276
		"tableRound" 286
		"nighttableW" 503
		"nighttableN" 504
		"counterW" 313
		"counterN" 344
		"dresserW" 272
		"dresserN" 273
		"stool" 304
		"chairW" 336
		"chairN" 339
		"chairE" 338
		"chairS" 337
		"bookcaseN" 282
		"bedN" 289
		"bedW" 290
		"chestW" 292
		"chestN" 293
		"candle" 291
		"sconceW" 294
		"sconceN" 295
		"lampW" 296
		"lampN" 297
		"barrel" 507
		"barrel2" 508
		"crate" 285
		"crate2" 270
		"pictureN" 500
		"pictureW" 502
		"picture2N" 501
		"counterEndS" 600
		"counterEndE" 601
		"counterEndN" 602
		"counterEndW" 603
		"counterNE" 604
		"counterNW" 605
		"counterSW" 606
		"counterSE" 607
		"counterN" 608
		"counterW" 609
		"counterS" 610
		"counterE" 611
		"banisterS" 615
		"banisterE" 616
		"banisterN" 617
		"banister4" 618
		"banisterPost" 619
		"Pat" 510
		"Fred" 511
		"Sue" 512
		"gtgt" 513
		"ggtt" 514
		"ggtt2" 515
		"gtgt2" 516
		"tgtt" 517
		"tgtg" 518
		"tgtg2" 519
		"ttgt" 520
		"ttgg" 521
		"ttgg2" 522
		"tttg" 523
		"templefloor" 540
		"templerailS" 524
		"templerailE" 525
		"templerailN" 526
		"templerailW" 527
		"templerailJoin" 559
		"templerailJoinV" 542
		"templestepsSr" 528
		"templestepsS" 529
		"templestepsSl" 533
		"templestepsSrV" 541
		"templestepsSlV" 538
		"templestepsE" 530
		"templestepsEl" 436
		"templestepsN" 531
		"templestepsNr" 536
		"templestepsW" 532
		"templestepsWl" 535
		"templedoorwayE" 558
		"templedoorwayS" 537
		"templewallSr" 561
		"templewallEr" 562
		"templewallSl" 563
		"templewallEl" 564
		"templeheadboardS" 543
		"templeheadboardE" 544
		"templestatueSW" 547
		"templestatueSE" 548
		"templestatueNE" 549
		"templeroof" 553
		"templeroofS" 551
		"templeroofE" 552
		"templeroofSW" 554
		"templeroofSE" 555
		"templeroofNE" 556
		"templeroofNW" 557
		"templeHorsieSW" 565
		"templeHorsieSE" 566
		"templeHorsieNE" 567
		"templeHorsieNW" 568
		"towergate1" 700
		"towergate2" 701
		"keepPillarS" 702
		"keepPillarSE" 703
		"keepPillarE" 704
		"keepTowerRoof" 705
		"keepTowerWallS" 706
		"keepTowerWallE" 707
		"keepRoofWindowS" 708
		"keepRoofWindowE" 709
		"keepRoofWindowN" 710
		"keepRoofWindowW" 711
		"keepRoofSE" 712
		"keepRoofNE" 713
		"keepRoofNW" 714
		"keepRoofSW" 715
		"keepRoofS" 716
		"keepRoofE" 717
		"keepRoofN" 718
		"keepRoofW" 719
		"keepChimneyE" 720
		"keepChimneyW" 721
		"keepPillar2" 722
		"keepWallS2" 723
		"keepWallE2" 724
		"keepWallS" 725
		"keepWallE" 726
		"keepWallN" 727
		"keepwallW" 728
		"keepBanner1" 729
		"keepBanner2" 730
		"keepBanner3" 731
		"keepTowerDoorwayS" 732
		"towersidedoor2" 733
		"keepMiniWallS" 734
		"keepMiniWallE" 735
		"keepMiniTowerRoof" 736
		"keepParpetS" 737
		"keepParpetE" 738
		"keepParpetN" 739
		"keepParpetW" 740
		"roofS" 486
		"roofE" 487
		"roofN" 488
		"roofW" 489
		"roofSE" 490
		"roofNE" 491
		"roofNW" 492
		"roofSW" 493
		"roof" 494
		"kiFloor" 773
		"kiWallN" 752
		"kiWallW" 753
		"kiWindowWallN" 746
		"kiWindowWallW" 747
		"kiSGWindowWallN" 771
		"kiSGWindowWallW" 772
		"kiGrateWallN" 748
		"kiGrateWallW" 749
		"kiPillarN" 761
		"kiPillarW" 762
		"kiDoorwayN" 780
		"kiDoorwayW" 781
		"kiDoorframeN" 782
		"kiDoorframeW" 783
		"kiHeadboardN" 750
		"kiHeadBoardW" 751
		"kiRug" 760
		"kiRugN" 756
		"kiRugW" 757
		"kiLongRugN" 758
		"kiLongRugW" 759
		"kiPictureN" 763
		"kiPictureW" 764
		"kiPictureN2" 765
		"kiPictureW2" 766
		"kiPictureN3" 767
		"kiPictureW3" 768
		"kiFireplaceN" 774
		"kiFireplaceW" 775
		"kiFireplaceTopN" 776
		"kiFireplaceTopW" 777
		"kiBannerN" 778
		"kiBannerW" 779
		"kiTableN" 754
		"kiTableW" 755
		"kiChairN" 784
		"kiChairE" 787
		"kiChairS" 786
		"kiChairW" 785
		"kiBenchN" 790
		"kiBenchE" 789
		"kiBenchS" 788
		"kiBenchW" 791
		"kiBanisterN" 792
		"kiBanisterW" 793
		"kiBanisterPost" 794
		"templeCollumnS" 539
		"kiStairsN" 770
		"kiStairsE" 769
		"kiStairsS" 799
		"kiStairsW" 798
		"kiStairsN2" 742
		"kiStairsE2" 741
		"kiStairsS2" 744
		"kiStairsW2" 743
		"kiLandingNE" 697
		"kiLandingSE" 745
		"kiLandingSW" 699
		"kiLandingNW" 698
		"armorW" 620
		"armorN" 621
		"armorcaseW" 622
		"armorcaseN" 623
		"armorshelfW" 624
		"armorshelfN" 625
		"kiPlant" 692
		"kiHeadboardN2" 693
		"kiHeadboardW2" 694
		"kiHalfwallN" 695
		"kiHalfwallW" 696
		"kipillar" 689
		"something" 690
		"something" 691
		"Forgggg" 173 Code = 0x10101010
		"Fordddd" 628 Code = 0x20202020
		"Forggdd" 131 Code = 0x10102020
		"Forddgg" 126 Code = 0x20201010
		"Forgdgd" 136 Code = 0x10201020
		"Fordgdg" 127 Code = 0x20102010
		"Forgggd" 226 Code = 0x10101020
		"Forggdg" 221 Code = 0x10102010
		"Forgdgg" 269 Code = 0x10201010
		"Fordggg" 217 Code = 0x20101010
		"Fordddg" 137 Code = 0x20202010
		"Forddgd" 159 Code = 0x20201020
		"Fordgdd" 139 Code = 0x20102020
		"Forgddd" 141 Code = 0x10202020
		"Forgddg" 341 Code = 0x10202010
		"Forgddg2" 585 Code = 0x10202010
		"Fordggd" 351 Code = 0x20101020
		"Fordggd2" 990 Code = 0x20101020
		"Forddkk" 432 Code = 0x20201111
		"Forkkdd" 459 Code = 0x11112020
		"Fordkdk" 457 Code = 0x20112011
		"Forkdkd" 462 Code = 0x11201120
		"Fordddk" 496 Code = 0x20202011
		"Forddkd" 560 Code = 0x20201120
		"Fordkdd" 534 Code = 0x20112020
		"Forkddd" 546 Code = 0x11202020
		"Forkkkd" 574 Code = 0x11111120
		"Forkkdk" 573 Code = 0x11112011
		"Forkdkk" 575 Code = 0x11201111
		"Fordkkk" 571 Code = 0x20111111
		"Fordkkd" 577 Code = 0x20111120
		"Forkddk" 576 Code = 0x11202011
		"Forggkk" 173 Code = 0x10101111 Extra = 0x20201111
		"Forkkgg" 173 Code = 0x11111010 Extra = 0x11112020
		"Forgkgk" 173 Code = 0x10111011 Extra = 0x20112011
		"Forkgkg" 173 Code = 0x11101110 Extra = 0x11201120
		"Forgggk" 173 Code = 0x10101011 Extra = 0x20202011
		"Forggkg" 173 Code = 0x10101110 Extra = 0x20201120
		"Forgkgg" 173 Code = 0x10111010 Extra = 0x20112020
		"Forkggg" 173 Code = 0x11101010 Extra = 0x11202020
		"Forkkkg" 173 Code = 0x11111110 Extra = 0x11111120
		"Forkkgk" 173 Code = 0x11111011 Extra = 0x11112011
		"Forkgkk" 173 Code = 0x11101111 Extra = 0x11201111
		"Forgkkk" 173 Code = 0x10111111 Extra = 0x20111111
		"Forgkkg" 173 Code = 0x10111110 Extra = 0x20111120
		"Forkggk" 173 Code = 0x11101011 Extra = 0x11202011
		"Forkrgr" 598 Code = 0x11511050 Extra = 0x11512050
		"Forrkrg" 599 Code = 0x51115010 Extra = 0x51115020
		"Forkgrr" 626 Code = 0x11105150 Extra = 0x11205150
		"Forrrkg" 627 Code = 0x51501110 Extra = 0x51501120
		"Forrrrr" 428 Code = 0x50505050
		"Forddrr" 456 Code = 0x20205050
		"Forrrdd" 429 Code = 0x50502020
		"Fordrdr" 458 Code = 0x20502050
		"Forrdrd" 430 Code = 0x50205020
		"Fordddr" 550 Code = 0x20202050
		"Forddrd" 545 Code = 0x20205020
		"Fordrdd" 569 Code = 0x20502020
		"Forrddd" 499 Code = 0x50202020
		"Forrrrd" 460 Code = 0x50505020
		"Forrrdr" 498 Code = 0x50502050
		"Forrdrr" 495 Code = 0x50205050
		"Fordrrr" 497 Code = 0x20505050
		"Fordrrd" 570 Code = 0x20505020
		"Forrddr" 572 Code = 0x50202050
		"Forgdrr" 580 Code = 0x10205050 Extra = 0x20205050
		"Forgrdr" 580 Code = 0x10502050 Extra = 0x20502050
		"Fordgrr" 582 Code = 0x20105050 Extra = 0x20205050
		"Forrgrd" 582 Code = 0x50105020 Extra = 0x50205020
		"Fordrgr" 584 Code = 0x20501050 Extra = 0x20502050
		"Forrrgd" 584 Code = 0x50501020 Extra = 0x50502020
		"Forrdrg" 587 Code = 0x50205010 Extra = 0x50205020
		"Forrrdg" 587 Code = 0x50502010 Extra = 0x50502020
		"Forgrgr" 588 Code = 0x10501050 Extra = 0x20502050
		"Forrgrg" 589 Code = 0x50105010 Extra = 0x50205020
		"Forggrr" 590 Code = 0x10105050 Extra = 0x20205050
		"Forrrgg" 591 Code = 0x50501010 Extra = 0x50502020
		"Forrrrg" 587 Code = 0x50505010 Extra = 0x50505020
		"Forrrgr" 584 Code = 0x50501050 Extra = 0x50502050
		"Forrgrr" 582 Code = 0x50105050 Extra = 0x50205050
		"Forgrrr" 580 Code = 0x10505050 Extra = 0x20505050
		"Forrggg" 592 Code = 0x50101010 Extra = 0x50202020
		"Forgrgg" 593 Code = 0x10501010 Extra = 0x20502020
		"Forggrg" 594 Code = 0x10105010 Extra = 0x20205020
		"Forgggr" 595 Code = 0x10101050 Extra = 0x20202050
		"Forgrrg" 596 Code = 0x10505010 Extra = 0x20505020
		"Forrggr" 597 Code = 0x50101050 Extra = 0x50202050
		"Forrrkd" 128 Code = 0x51501120
		"Forrkrd" 130 Code = 0x51115020
		"Forkdrr" 132 Code = 0x11205150
		"Forkrdr" 578 Code = 0x11512050
		"Forirdr" 156 Code = 0x14542050
		"Forrird" 160 Code = 0x54145020
		"Foridrr" 170 Code = 0x14205450
		"Forrrid" 158 Code = 0x54501420
		"Forirgr" 598 Code = 0x14541050 Extra = 0x14542050
		"Forrirg" 599 Code = 0x54145010 Extra = 0x54145020
		"Forigrr" 626 Code = 0x14105450 Extra = 0x14205450
		"Forrrig" 627 Code = 0x54501410 Extra = 0x54501420
		"Foroooo" 129 Code = 0x60606060
		"Forggoo" 138 Code = 0x10106060
		"Foroogg" 157 Code = 0x60601010
		"Forgogo" 140 Code = 0x10601060
		"Forogog" 161 Code = 0x60106010
		"Forgggo" 331 Code = 0x10101060
		"Forggog" 427 Code = 0x10106010
		"Forgogg" 343 Code = 0x10601010
		"Foroggg" 426 Code = 0x60101010
		"Forooog" 224 Code = 0x60606010
		"Foroogo" 219 Code = 0x60601060
		"Forogoo" 252 Code = 0x60106060
		"Forgooo" 215 Code = 0x10606060
		"Forgoog" 135 Code = 0x10606010
		"Foroggo" 133 Code = 0x60101060
		"Forgrrggrrggrrgoooo" 258 Supertile = 0x1 Width = 0x4 Height = 0x4
		"Forgggorrrorrrogggo" 259 Supertile = 0x1 Width = 0x4 Height = 0x4
		"Foroooogrrggrrggrrg" 260 Supertile = 0x1 Width = 0x4 Height = 0x4
		"Forogggorrrorrroggg" 261 Supertile = 0x1 Width = 0x4 Height = 0x4
		"Foriiix" 330 Code = 0x14141400
		"Foriixi" 268 Code = 0x14140014
		"Forixii" 340 Code = 0x14001414
		"Forxiii" 342 Code = 0x141414
		"Forxxii" 225 Code = 0x1414
		"Foriixx" 220 Code = 0x14140000
		"Forxixi" 228 Code = 0x140014
		"Forixix" 223 Code = 0x14001400
		"Forxxxi" 218 Code = 0x14
		"Forxxix" 216 Code = 0x1400
		"Forxixx" 579 Code = 0x140000
		"Forixxx" 214 Code = 0x14000000
		"Formdrr" 654 Code = 0x30205050
		"Formdrr2" 629
		"Forrrmd" 652 Code = 0x50503020
		"Forrrmd2" 630
		"Formrdr" 651 Code = 0x30502050
		"Formrdr2" 631
		"Forrmrd" 653 Code = 0x50305020
		"Forrmrd2" 645
		"Formgrr" 626 Code = 0x30105050 Extra = 0x30205050
		"Formgrr2" 629
		"Forrrmg" 627 Code = 0x50503010 Extra = 0x50503020
		"Forrrmg2" 630
		"Formrgr" 598 Code = 0x30501050 Extra = 0x30502050
		"Formrgr2" 631
		"Forrmrg" 599 Code = 0x50305010 Extra = 0x50305020
		"Forrmrg2" 645
		"Formmxx" 650 Code = 0x30300000
		"Formmxx2" 643
		"Formxmx" 644 Code = 0x30003000
		"Formxmx2" 641
		"Forxmxm" 658 Code = 0x300030
		"Forxxmm" 656 Code = 0x3030
		"Formmmx" 637 Code = 0x30303000
		"Formmmx2" 633
		"Forxmmm" 659 Code = 0x303030
		"Formxmm" 635 Code = 0x30003030
		"Formxmm2" 632
		"Formmxm" 636 Code = 0x30300030
		"Formmxm2" 634
		"Forxmxx" 640 Code = 0x300000
		"Forxmxx2" 638
		"Formxxx" 642 Code = 0x30000000
		"Formxxx2" 639
		"Forxxmx" 655 Code = 0x3000
		"Forxxxm" 657 Code = 0x30
		"Forcxcx" 648
		"Forcxcx2" 646
		"Forccxx" 647
		"Forccxx2" 649
		"ForpathNES" 185
		"ForpathNS" 186
		"ForpathEW" 187
		"ForpathNW" 188
		"ForpathES" 189
		"ForpathSW" 229
		"ForpathNE" 262
		"ForpathN" 263
		"ForpathS" 264
		"ForpathW" 265
		"ForpathE" 266
		"ForpathBN" 267
		"ForpathBS" 305
		"ForpathBW" 306
		"ForpathBE" 400
		"ForpathNSW" 431
		"ForpathNEW" 433
		"ForpathESW" 435
		"ForRock1" 998
		"ForRock2" 999
		"ForRock3" 995
		"ForRock4" 994
		"ForTree1" 115
		"ForTree2" 116
		"ForTree3" 117
		"ForTree4" 118
		"ForTree5" 119
		"ForTree6" 171
		"ForTree7" 172
		"ForTree8" 399
		"ForTree9" 996
		"ForBirch1" 120
		"ForBirch2" 121
		"ForBirch3" 122
		"ForBirch4" 123
		"ForBirch5" 124
		"ForFerns1" 805
		"ForFerns2" 821
		"ForFerns3" 822
		"ForFerns4" 823
		"ForPlants1" 826
		"ForPlants2" 827
		"ForPlants3" 829
		"ForPlants4" 851
		"ForFerns5" 852
		"ForFerns6" 853
		"ForFerns7" 854
		"ForFerns8" 855
		"ForStalk1" 856
		"ForStalk2" 857
		"ForStalk3" 859
		"ForGrassTuft1" 865
		"ForGrassTuft2" 866
		"ForGrassTuft3" 867
		"ForGrassTuft4" 868
		"ForFern1" 869
		"ForFern2" 876
		"ForFern3" 877
		"ForFern4" 878
		"ForPlant1" 870
		"ForPlant2" 871
		"ForPlant3" 879
		"ForPlant4" 880
		"ForPlant5" 881
		"ForPlant6" 882
		"ForPlant7" 883
		"ForPlant8" 884
		"ForPlant9" 885
		"ForPlant10" 886
		"ForPlant11" 887
		"ForPlant12" 888
		"ForPlant13" 889
		"ForPlant14" 890
		"ForPlant15" 891
		"ForPlant16" 892
		"ForMushrooms1" 872
		"ForMushrooms2" 873
		"ForMushrooms3" 874
		"ForMushrooms4" 875
		"ForBridge" 142
		"DunFloor" 451
		"DunFloorDesign" 450
		"DunWater" 848
		"Dungggw" 463
		"Dungwgg" 464
		"Dunwggg" 465
		"Dunggwg" 466
		"Dungwww" 467
		"Dunwwwg" 468
		"Dunwwgw" 469
		"Dunwgww" 470
		"Dunwgwg" 471
		"Dunwwgg" 472
		"Dungwgw" 473
		"dunggww" 474
		"dunFountain" 479
		"dunWallN" 830
		"dunWallW" 831
		"dunPillarN" 839
		"dunPillarW" 840
		"dunPillar" 847
		"dunPillarNW" 835
		"dunPillarN2" 836
		"dunPillarW2" 858
		"dunPillarTopN" 959
		"dunPillarTopW" 960
		"dunHeadboardN" 837
		"dunHeadboardW" 838
		"dunArchJoinN" 976
		"dunArchJoinW" 977
		"dunHalfwallN" 978
		"dunHalfwallW" 979
		"dunRuneWallN" 437
		"dunRuneWallW" 438
		"dunRuneWallN2" 832
		"dunRuneWallW2" 833
		"dunGrateWallN" 446
		"dunGrateWallW" 447
		"dunWordWallN" 452
		"dunWordWallW" 454
		"dunWinWallN" 475
		"dunWinWallW" 477
		"dunCauldronWallN" 665
		"dunCauldronWallW" 666
		"dunStatueN" 661
		"dunStatueW" 662
		"dunDoorwayN" 476
		"dunDoorwayW" 478
		"dunPortcullisN" 667
		"dunPortcullisW" 676
		"dunAlcoveN" 955
		"dunAlcoveTopN" 957
		"dunAlcoveTopN2" 974
		"dunAlcoveW" 956
		"dunAlcoveTopW" 958
		"dunAlcoveTopW2" 975
		"dunBreakawayWallN" 670
		"dunBreakawayWallW" 671
		"dunEyeWallN" 681
		"dunEyeWallW" 682
		"dunFireballWallN" 683
		"dunFireballWallW" 684
		"dunRevolvingWallN" 824
		"dunRevolvingWallW" 825
		"dunFloorPresPlate" 795
		"dunFloorPresPlate2" 797
		"dunFloorTrapdoor" 828
		"dunFloorSpikePit" 834
		"dunBanisterPost" 860
		"dunBanisterN" 861
		"dunBanisterN2" 862
		"dunBanisterW" 863
		"dunBanisterW2" 864
		"dunBridgeFloorEW" 480
		"dunBridgeFloorNS" 481
		"dunBridgeMatNS" 482
		"dunBridgeMatEW" 483
		"dunBridgeArchEW" 484
		"dunBridgeArchNS" 485
		"dunBridgePost" 660
		"dunStepN" 972
		"dunStepE" 971
		"dunStepS" 970
		"dunStepW" 973
		"dunStepNW" 964
		"dunStepSE" 962
		"dunStepSW" 965
		"dunStepNE" 963
		"dunStepNWin" 966
		"dunStepSEin" 968
		"dunStepSWin" 967
		"dunStepNEin" 969
		"dunStairsN" 679
		"dunStairsE" 672
		"dunStairsS" 674
		"dunStairsW" 675
		"dunStairsN2" 680
		"dunStairsE2" 673
		"dunStairsS2" 688
		"dunStairsW2" 685
		"dunLandingNE" 687
		"dunLandingSE" 686
		"dunLandingSW" 804
		"dunLandingNW" 796
		"dunPool1" 441
		"dunPool2" 445
		"dunBrazier" 461
		"dunCrankN" 663
		"dunCrankW" 664
		"dunSwitchN" 668
		"dunSwitchW" 669
		"dunWardN" 677
		"dunWardW" 678
		"dunThroneDias" 841
		"dunDaisBack" 842
		"dunCoffinN" 843
		"dunCoffinW" 844
		"dunCoffinS" 845
		"dunCoffinE" 846
		"dunBedN" 222
		"dunBedW" 250
		"dunCampefire1" 251
		"dunCampfire2" 961
		"dunTransporter" 893
		"cavdunTransN" 927
		"cavdunTransW" 928
		"cavdunTransW2" 929
		"cavdunTransN2" 930
		"cavFloor" 913
		"caviigg" 898
		"caviigg2" 902
		"cavigig" 899
		"cavigig2" 903
		"caviggg" 900
		"cavgigg" 909
		"cavggig" 901
		"cavigii" 908
		"caviigi" 906
		"caviiig" 907
		"cavccgg" 904
		"cavcgcg" 905
		"cavgwgw" 910
		"cavwgwg" 924
		"cavggww" 925
		"cavwwgg" 926
		"cavgwww" 919
		"cavwgww" 914
		"cavwwgw" 920
		"cavwwwg" 921
		"cavwggg" 918
		"cavgwgg" 917
		"cavggwg" 915
		"cavgggw" 916
		"cavgwwg" 923
		"cavwggw" 922
		"cavBones1" 912
		"cavBones2" 931
		"cavBones3" 933
		"cavBones4" 935
		"cavAltar" 911
		"cavWallN" 951
		"cavWallW" 952
		"cavTallWallN" 953
		"cavTallWallW" 954
		"cavWallTopN" 938
		"cavWallTopW" 936
		"cavWallTopN2" 941
		"cavWallTopW2" 942
		"cavFountainW" 943
		"cavFountainN" 944
		"cavDoorN" 939
		"cavDoorW" 940
		"cavBrazier" 937
		"cavBrazier2" 945
		"cavBridgeFloorEW" 946
		"cavBridgeFloorNS" 947
		"cavBridgeArchEW" 948
		"cavBridgeArchNS" 949
		"cavBridgePost" 950
		"Cavgggl" 001
		"Cavglgg" 002
		"Cavlggg" 003
		"Cavgglg" 004
		"Cavlggl" 005
		"Cavgllg" 006
		"Cavllgg" 007
		"Cavlglg" 008
		"Cavggll" 009
		"Cavglgl" 010
		"Cavlllg" 011
		"Cavlgll" 012
		"Cavglll" 013
		"Cavllgl" 014
		"Cavllll" 015
		"Cavlllo" 016
		"Cavloll" 017
		"Cavolll" 018
		"Cavllol" 019
		"Cavllls" 020
		"Cavlsll" 021
		"Cavslll" 022
		"Cavllsl" 023
		"Cavlllu" 024
		"Cavlull" 025
		"Cavulll" 026
		"Cavllul" 027
		"Cavlloo" 028
		"Cavlolo" 029
		"Cavooll" 030
		"Cavolol" 031
		"Cavllss" 032
		"Cavlsls" 033
		"Cavssll" 034
		"Cavslsl" 035
		"Cavlluu" 036
		"Cavlulu" 037
		"Cavuull" 038
		"Cavulul" 039
		"Cavlool" 040
		"Cavollo" 041
		"Cavlooo" 042
		"Cavoolo" 043
		"Cavoool" 044
		"Cavoloo" 045
		"Cavlssl" 046
		"Cavslls" 047
		"Cavlsss" 048
		"Cavssls" 049
		"Cavsssl" 050
		"Cavslss" 051
		"Cavluul" 052
		"Cavullu" 053
		"Cavluuu" 054
		"Cavuulu" 055
		"Cavuuul" 056
		"Cavuluu" 057
		"Cavnnnn" 058
		"Cavnnnu" 059
		"Cavnunn" 060
		"Cavunnn" 061
		"Cavnnun" 062
		"Cavnnuu" 063
		"Cavnunu" 064
		"Cavuunn" 065
		"Cavunun" 066
		"Cavnuun" 067
		"Cavnuuu" 068
		"Cavuunu" 069
		"Cavuuun" 070
		"Cavunuu" 071
		"Cavssss" 073
		"Cavuuuu" 074
		"labFloorSE" 000
		"labFloorNE" 072
		"labFloorNW" 075
		"labFloorSW" 076
		"labPillar" 077
		"labFloorEW" 078
		"labFloorNS" 079
		"labStairsW" 080
		"labStairsS" 081
		"labStairsE" 082
		"labStairsN" 083
		"labFloorENS" 084
		"labFloorESW" 085
		"labFloorNES" 086
		"labFloorNEW" 087
		"labFloorNESW" 088
		"labFloorPylon" 089
		"labFloorEw" 090
		"labFloorNs" 091
		"labFloorWe" 092
		"labFloorSn" 093
		"labDoorwayN" 094
		"labDoorwayW" 095
		"labFloorNESW2" 096
		"labBrazierW" 097
		"labBrazierE" 098
		"labPyramidDoor" 099
		"labPyramidPillar" 980
		"labPyramidSW" 981
		"labPyramidSE" 982
		"labPyramidS" 983
		"Labpyrdoorwall" 984
		"Labcontrol1" 985
		"Labcontrol2" 986
		"Labplatform" 987
		"lightning" 581
		"tuft2" 991
		"tree2" 992
		"stump" 993
		"DeadTree" 997
	end

end

CLASS "EXIT"
begin


	TYPES
	begin
		"Door" 740
		"Halfdoor" 740
	end

end

CLASS "PLAYER"
begin


	TYPES
	begin
		"Locke" "locke.i3d"
		"Hog" 740
	end

end

CLASS "CHARACTER"
begin


	TYPES
	begin
		"Ollihoot" 740
		"Fatman" 740
		"Gordo" "gordo.i3d"
		"FatGuard" 740
		"SkinnyGuard" 740
		"Sorcerer" 740
		"Thickman" 740
		"Lizardman" 740
		"SkinnyGuy" 740
		"Warrior1" 740
		"Warrior2" 740
		"Warrior3" 740
		"ThickGuard" 740
		"Hopper" 740
		"Dragon" 740
		"HighPriest" 740
	end

end

CLASS "SHADOW"
begin

end

CLASS "HELPER"
begin


	TYPES
	begin
		"Box" "box.i3d"
		"Axis" "axis.i3d"
		"Plane" 100
		"Plane3D" 100
	end

end

CLASS "Lightning"
begin


	TYPES
	begin
		"Lightning" 932
	end

end

CLASS "Hole"
begin


	TYPES
	begin
		"Hole" 934
	end

end

CLASS "AMMO"
begin


	STATS
	begin
		"EqSlot" 0x7 0x0 0x8
	end


	TYPES
	begin
		"Arrow" 257
		"Lightning Bolt" 114
	end

end

CLASS "SCROLL"
begin


	TYPES
	begin
		"Scroll" 253
	end

end

CLASS "RANGEDWEAPON"
begin


	STATS
	begin
		"EqSlot" 0x6 0x0 0x8
	end


	TYPES
	begin
		"Bow" 397
		"Crossbow" 315
		"Steel Bow" 385
		"Heavy Crossbow" 379
		"Golden Bow" 246
		"Delvagul" 378
	end

end

CLASS "EFFECT"
begin


	TYPES
	begin
		"Blood" 586
	end

end

