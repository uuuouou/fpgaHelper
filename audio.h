/*
 * audio.h
 *
 *  Created on: May 5, 2014
 *      Author: root
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include "user_type.h"

//音频芯片参数配置地址及对应数据
#define	Left_Line_Input_Volume_Control 			(0x00 + 0x00)	//8'b0000000_0, 左线形输入通道音频控制
//#define Left_Line_Input_Volume_Control_Data 	(0x17)			//8'b00010111

#define Right_Line_Input_Volume_Control 		(0x02 + 0x00)	//8'b0000001_0, 右线形输入通道音频控制
//#define Right_Line_Input_Volume_Control_Data	(0x97)			//8'b10010111

#define	Left_Headphone_Volume_Control			(0x04 + 0x00)   //8'b0000010_0, 左通道耳机音频控制
//#define	Left_Headphone_Volume_Control_Data		(0xF9)			//8'b11111001

#define	Right_Headphone_Volume_Control			(0x06 + 0x00)   //8'b0000011_0, 右通道耳机音频控制
//#define	Right_Headphone_Volume_Control_Data		(0xF9)			//8'b11111001

#define	Analog_Audio_Path_Control	    		(0x08 + 0x00)   //8'b0000100_0, 模拟音频控制
//#define	Analog_Audio_Path_Control_Data			(0x12)			//8'b00010010

#define	Digital_Audio_Path_Control	    		(0x0A + 0x00)   //8'b0000101_0, 数字音频控制
//#define	Digital_Audio_Path_Control_Data			(0x00)			//8'b00000000

#define	Power_Down_Control						(0x0C + 0x00)	//8'b0000110_0, 功耗控制
//#define	Power_Down_Control_Data					(0x02)			//8'b00000010

#define Digital_Audio_Inteface_Format			(0x0E + 0x00)   //8'b0000111_0, 数字音频接口格式
//#define Digital_Audio_Inteface_Format_Data		(0x0A)			//8'b00001010

#define	Sample_Rate_Control						(0x10 + 0x00)	//8'b0001000_0, 采样率控制
//#define Sample_Rate_Control_Data				(0x01)			//8'b00000001

#define	Digital_Interface_Activation			(0x12 + 0x00)	//8'b0001001_0, 数字接口激活
//#define	Digital_Interface_Activation_Data		(0x01)			//8'b00000001

#define	Reset_Register							(0x1E + 0x00)	//8'b0001111_0, 复位寄存器
//#define Reset_Register_Data						(0x00)			//8'b00000000




//配置音频芯片参数的函数
int configurateAudioChip(int fd, UCHAR audioConfig[11]);

//将音频芯片要发出去的24bit数据依次发给FPGA
int configurateAudioSignal(int fd, int data[], int n);

//read audio signal at certain index
int readAudioSignal(int fd, UCHAR index, UCHAR data[3]);


#endif /* AUDIO_H_ */
