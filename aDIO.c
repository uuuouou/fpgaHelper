/*
 * aDIO.c
 *
 *  Created on: July 17, 2014
 *      Author: root
 */

#include "aDIO.h"
#include "user_type.h"
#include "aDIO_library.h"

#include <error.h>
#include <errno.h>
#include <stdio.h>

static DeviceHandle aDIO_Device;
static UCHAR PortValue[2];
static UCHAR P0Direction = 0;
static char *P1Direction = "Output";
static UCHAR Strobe[2];


void open_aDIO()
{
	if(OpenDIO_aDIO(&aDIO_Device, 0))
		fprintf(stderr, "error@open_aDIO: OpenDIO_aDIO() failed\n");
}

void close_aDIO()
{
	if(CloseDIO_aDIO(aDIO_Device))
		fprintf(stderr, "error@close_aDIO: CloseDIO_aDIO() failed\n");
}

void print_aDIO()
{
	int i, aDIO_ReturnVal;
	UCHAR ControlReg;

//Read the values of Port 0 and Port 1 
	for (i = 0; i < 2; i++) {
		aDIO_ReturnVal = ReadPort_aDIO(aDIO_Device, i, &PortValue[i]);
		if(aDIO_ReturnVal) error(EXIT_FAILURE, errno, "ERROR: ReadPort_aDIO() FAILED");
	}

//Get the direction of Port 1
	aDIO_ReturnVal = ReadControlRegister_aDIO(aDIO_Device, &ControlReg);
	if(aDIO_ReturnVal) error(EXIT_FAILURE, errno, "ERROR: ReadControlRegister_aDIO() FAILED");
	if ((ControlReg & 0x04) == 0) P1Direction = "Input ";
	else P1Direction = "Output";

//Read Strobe 0 and Strobe 1
	aDIO_ReturnVal = ReadStrobe0_aDIO(aDIO_Device, &Strobe[0]);
	if(aDIO_ReturnVal) error(EXIT_FAILURE, errno, "ERROR: ReadStrobe0_aDIO() FAILED");
	aDIO_ReturnVal = ReadStrobe1_aDIO(aDIO_Device, &Strobe[1]);
	if(aDIO_ReturnVal) error(EXIT_FAILURE, errno, "ERROR: ReadStrobe1_aDIO() FAILED");

//print the result to the screen 
	puts
	    ("\n               PORT 0        PORT 1        STROBE 0        STROBE 1");
	puts
	    ("             --------------------------------------------------------");
	printf
	    ("Direction      0x%02x          %s                                   \n",
	     P0Direction, P1Direction);
	printf
	    ("Value Read     0x%02x          0x%02x            %s            %s    \n",
	     PortValue[0], PortValue[1], Strobe[0] ? "High" : "Low", Strobe[1] ? "High" : "Low");
}

void setPort0Direction(const char* val)
{
	int res;
	if(1 != sscanf(val, "%x", &res) || res < 0 || res > 255){
		fprintf(stderr, "Invalid val, must be in range [0x00, 0xFF]\n");
		return;
	}
	if(LoadPort0BitDir_aDIO(aDIO_Device, 
		  					(res >> 7) & 0x01, (res >> 6) & 0x01, 
		  					(res >> 5) & 0x01, (res >> 4) & 0x01, 
		  					(res >> 3) & 0x01, (res >> 2) & 0x01, 
		  					(res >> 1) & 0x01,  res & 0x01
		  					)
		) fprintf(stderr, "error@setPort0Direction: LoadPort0bitDir_aDIO() failed\n");
	else P0Direction = res;
	print_aDIO();
}
void setPort0Value(const char* val)
{
	int res;
	if(1 != sscanf(val, "%x", &res) || res < 0 || res > 255){
		fprintf(stderr, "Invalid val, must be in range [0x00, 0xFF]\n");
		return;
	}
	if(WritePort_aDIO(aDIO_Device, 0, res))
		fprintf(stderr, "error@setPort0Value: WritePort_aDIO() failed\n");
	print_aDIO();
}

void setPort1Direction(const char* val)
{
	int res;
	if(1 != sscanf(val, "%d", &res) || res < 0 || res > 1){
		fprintf(stderr, "Invalid val, must be 0(for input) or 1(for output)\n");
		return;
	}
	if(LoadPort1PortDir_aDIO(aDIO_Device, res))
		fprintf(stderr, "error@setPort0Direction: LoadPort1bitDir_aDIO() failed\n");
	print_aDIO();
}
void setPort1Value(const char* val)
{
	int res;
	if(1 != sscanf(val, "%x", &res) || res < 0 || res > 255){
		fprintf(stderr, "Invalid val, must be in range [0x00, 0xFF]\n");
		return;
	}
	if(WritePort_aDIO(aDIO_Device, 1, res))
		fprintf(stderr, "error@setPort0Value: WritePort_aDIO() failed\n");
	print_aDIO();
}
