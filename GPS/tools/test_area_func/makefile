all: mainfunc.o area.o test_uart.o
	gcc -lm -g -o gps mainfunc.o area.o test_uart.o
mainfunc.o: mainfunc.c area.h test_uart.h
	gcc -g -c mainfunc.c
area.o: area.c
	gcc -g -lm -c area.c
test.o: test_uart.c
	gcc -g -c test_uart.c
