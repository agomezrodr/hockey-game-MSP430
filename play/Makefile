# makfile configuration

CPU             	= msp430g2553
CFLAGS          	= -mmcu=${CPU} -Os -I../h
LDFLAGS		= -L../lib -L/opt/ti/msp430_gcc/include/ 

#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-gcc -mmcu=${CPU} -c


all: hockey.elf

#additional rules for files

hockey.elf: ${COMMON_OBJECTS}  hockey.o wdt_handler.o buzzer.o stateMachine.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ -lTimer -lLcd -lShape -lCircle -lp2sw

load: hockey.elf
	mspdebug rf2500 "prog $^"

clean:
	rm -f *.o *.elf
