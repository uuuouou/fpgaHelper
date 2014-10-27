fpgaHelper15: main.o fpga.o audio.o specific.o serial_port.o aDIO.o
	g++ -o fpgaHelper15 main.o fpga.o audio.o specific.o serial_port.o aDIO.o -lreadline -lpthread -L. -lrtd-aDIO
serial_port.o: serial_port.c serial_port.h user_type.h
	gcc -c serial_port.c
specific.o: specific.c specific.h user_type.h
	gcc -c specific.c
audio.o: audio.c audio.h fpga.h specific.h serial_port.h
	gcc -c audio.c
fpga.o: fpga.c fpga.h specific.h audio.h
	gcc -c fpga.c
aDIO.o: aDIO.c aDIO.h user_type.h aDIO_library.h
	gcc -c aDIO.c
main.o: main.cpp serial_port.h fpga.h aDIO.h user_type.h
	g++ -c main.cpp
