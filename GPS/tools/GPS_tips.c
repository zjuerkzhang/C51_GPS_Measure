$Rev$
$Author$
$ID$
$URL$
$Date$
$Header$

KeyPressValue:
 1: Left
 2: Up
 3: Right
 4: down
 5: OK
 6: menu
 0: none key

danwei_zouchang_sel:
	0: mi
	1: kilo mi


TEST1: indicate the current project
 0:main menu
 1:celiang
 2:danjia
 3:danwei
 4:jilu

danjiasel: [0-9]
	0: area
	5: length

TEST2[5]: the price per area, only index 0-3 valid
	
TEST3[5]: the price per length, only index 0-3 valid


TEST_0[20]: jiner value

TEST_1[5]: Beijing Time in ascii
 	
TEST_2: the count of history data

TEST_5: the time in dec HHMM
 
TEST_6: index of 4 menu
 0:celiang
 1:danjia
 2:danwei
 3:jilu

TEST_7: to fresh the battle
	
TEST_8: jiner
 
TEST_9[20]: mianji value
 
FLAG1: indicate the state of celiang
	0: idle in celiang
	1: celiang ing
	2: celiang save 
 
FLAG3: lcd need fresh

FLAG4: flag to indicate the danwei of area or length
	0: length
	1: area
 
 
 