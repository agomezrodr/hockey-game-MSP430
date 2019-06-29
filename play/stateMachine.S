; Adrian Gomez Rodriguez
; Computer Architecture 1
; T-Th 7:30-8:50
; Prof. Dr. Freudenthal
; TA. Daniel Cervantes

		.data
count:	
		.word 0
	
		.text
jumpTable:
    .word case0
    .word case1
	.word case2
	.word case3
	.word case4
	.word case5
	.word case6
	.word case7
	.word case8
	.word case9
	.word case10
	.word case11
	.word case12
    .global Music

Music:	mov &count, r12
		add r12, r12
		mov jumpTable(r12), r0

;; start the cases for the music
case0:  mov #300, r12
		call #buzzer_set_period
		add #5, &count
		JMP break
case1:
case2:
case3:  mov #1020, r12
		call #buzzer_set_period
		add #5, &count
		JMP break
case4:
case5:
case6:  mov #700, r12
		call #buzzer_set_period
		cmp #12, &count
		JGE else
		mov #0, &count
		JMP break
	else:
		add #5, &count
		JMP break

case7:
case8:
case9:  mov #3800, r12
		 call #buzzer_set_period
		 add #5, &count
		 JMP break
case10:
case11:
case12: mov #2000, r12
		call #buzzer_set_period
		add #5, &count
		JMP break
break:	
        ret
 
