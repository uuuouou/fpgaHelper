/*
 * audio.c
 *
 *  Created on: May 5, 2014
 *      Author: root
 */

//local headers
#include "audio.h"
#include "fpga.h"
#include "specific.h"
#include "serial_port.h"

//standard C headers
#include <stdio.h>


static const UCHAR audioConfigAddr[] = {
	Reset_Register					,
	Power_Down_Control				,
	Digital_Audio_Inteface_Format	,
	Analog_Audio_Path_Control		,
	Digital_Audio_Path_Control 		,
	Sample_Rate_Control				,
	Left_Headphone_Volume_Control	,
	Left_Line_Input_Volume_Control	,
	Right_Headphone_Volume_Control	,
	Right_Line_Input_Volume_Control	,
	Digital_Interface_Activation,
};
static const char* audioConfigName[] = {
	STRING(Reset_Register) 					,
	STRING(Power_Down_Control) 				,
	STRING(Digital_Audio_Inteface_Format)	,
	STRING(Analog_Audio_Path_Control) 		,
	STRING(Digital_Audio_Path_Control) 		,
	STRING(Sample_Rate_Control) 			,
	STRING(Left_Headphone_Volume_Control) 	,
	STRING(Left_Line_Input_Volume_Control) 	,
	STRING(Right_Headphone_Volume_Control) 	,
	STRING(Right_Line_Input_Volume_Control) ,
	STRING(Digital_Interface_Activation)
};


static int sendAudioConfig(int fd, UCHAR addr, UCHAR data)
{
	UCHAR state;

	//step 1: send addr
	if(SUCCESS != writeRegister(fd, AIC_CFG_ADDR, addr)) return FAILURE;

	//step 2: send data
	if(SUCCESS != writeRegister(fd, AIC_CFG_DATA, data)) return FAILURE;

	//step 3: read state of changing audio config
	if(SUCCESS != readRegister(fd, AIC_CFG_STATE, &state)) return FAILURE;

	//step 4: check if state is good
	if((state & 0xC0) == 0x80 && 				//state[7:6] = 2'b10
	   (state & 0x3F) == ((addr & 0x7F) >> 1)  //state[5:0] = addr[6:1]
	) return SUCCESS;

	return FAILURE;
}
int configurateAudioChip(int fd, UCHAR audioConfig[])
{
	int i = 0;
	for(; i < 11; ++i){
		if(SUCCESS != sendAudioConfig(fd, audioConfigAddr[i], audioConfig[i])){
			fprintf(stderr, "error@configurateAudioChip: fail to set %s\n", audioConfigName[i]);
			return FAILURE;
		}
	}
	return SUCCESS;
}



static int sendAudioSignal(int fd, UCHAR index, int data)
{
	const static int maskHig = 0x00FF0000;
	const static int maskMid = 0x0000FF00;
	const static int maskLow = 0x000000FF;

	//step 1: send addr
	if(SUCCESS != writeRegister(fd, AUDIO_DATA_UPDATE_ADDR, index)) return FAILURE;
	//step 2: send data[23:16]
	if(SUCCESS != writeRegister(fd, AUDIO_DATA_UPDATE_HIG, (data & maskHig) >> 16)) return FAILURE;
	//step 3: send data[15:8]
	if(SUCCESS != writeRegister(fd, AUDIO_DATA_UPDATE_MID, (data & maskMid) >> 8)) return FAILURE;
	//step 4: send data[7:0]
	if(SUCCESS != writeRegister(fd, AUDIO_DATA_UPDATE_LOW, (data & maskLow) >> 0)) return FAILURE;
	//step 5: enable write
	if(SUCCESS != writeRegister(fd, AUDIO_DATA_UPDATE_EN, 0x01)) return FAILURE;

	return SUCCESS;
}
int configurateAudioSignal(int fd, int signal[], int n)
{
	if(n < 1 || n > 256){
		fprintf(stderr, "error@configurateAudioSignal: illegal signal length = %d\n", n);
		return FAILURE;
	}
	//send each signal value
	int i = 0;
	for(; i < n; ++i){
		if(SUCCESS != sendAudioSignal(fd, i & 0xFF, signal[i])){
			fprintf(stderr, "error@configurateAudioSignal: fail to set signal[%d] = %06X\n", i, signal[i]);
			return FAILURE;
		}
	}
	//send signal max index to transmit
	if(SUCCESS != writeRegister(fd, AUDIO_DATA_MAX_ADDR, (n - 1) & 0xFF)){
		fprintf(stderr, "error@configurateAudioSignal: fail to set AUDIO_DATA_MAX_ADDR\n");
		return FAILURE;
	}

	return SUCCESS;
}



static const UCHAR audioDataAddr[] = {
		AUDIO_DATA_RD_HIG,
		AUDIO_DATA_RD_MID,
		AUDIO_DATA_RD_LOW
};
int readAudioSignal(int fd, UCHAR index, UCHAR data[3])
{
//step 1: write index
	if(SUCCESS != writeRegister(fd, AUDIO_DATA_RD_ADDR, index)) return FAILURE;
//step 2: read high, mid, low
	if(SUCCESS != readRegisters(fd, audioDataAddr, data, 3)) return FAILURE;
	return SUCCESS;
}

