/*
 * fpga.c
 *
 *  Created on: May 8, 2014
 *      Author: root
 */

#include "fpga.h"
#include "specific.h"
#include "audio.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

int parseTime(UCHAR arr[7], const char* time)
{
	int year, mon, day, hour, min, sec;
	if(6 != sscanf(time, " %d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &min, &sec)){
		return FAILURE;
	}
	arr[0] = year / 100;
	arr[1] = year % 100;
	arr[2] = mon;
	arr[3] = day;
	arr[4] = hour;
	arr[5] = min;
	arr[6] = sec;
	return SUCCESS;
}

/****************************** read write ********************************/
void readAddr(int fd, const char* addr)
{
	int n;
	UINT start, end;
	UCHAR value;
	n = sscanf(addr, " %x %x", &start, &end);
	if(n < 1 ||
	  (n == 1 && start > 255) ||
	  (n == 2 && (start > end || end > 255))
	  ){
		fprintf(stderr, "Invalid address\n");
		return;
	}
	if(n == 1) end = start;
	for(; start <= end; ++start){
		if(SUCCESS != readRegister(fd, start, &value))
			fprintf(stderr, "Failed to read fpga[0x%02X]\n", start);
		else
			printf("fpga[0x%02X] = 0x%02X\n", start, value);
	}
}
void writeAddr(int fd, const char* addrValue)
{
	UINT address, value;
	if(2 != sscanf(addrValue, " %x %x", &address, &value)){
		fprintf(stderr, "Invalid address or value\n");
		return;
	}
	if(SUCCESS != writeRegister(fd, address, value)){
		fprintf(stderr, "Failed to write 0x%02X to fpga[0x%02X]\n", value, address);
		return;
	}
	printf("Write 0x%02X to fpga[0x%02X] done\n", value, address);
}

/****************************** configure audio chip ********************************/
void configure(int fd, const char* file)
{
	int i;
	char line[256], *p;
	UINT tmp;
	UCHAR arr[11];
//step 1: open file
	while(isspace(*file)) ++file;
	FILE* fp = fopen(file, "r");
	if(fp == NULL){
		fprintf(stderr, "Invalid file path");
		return;
	}
//step 2: read out parameters
	for(i = 0; i < 11; ++i){
		if(NULL == fgets(line, sizeof(line), fp)){
			fprintf(stderr, "Invalid line %d\n", i + 1);
			fclose(fp);
			return;
		}
		p = strchr(line, '#');
		if(p != NULL) *p = '\0';
		if(1 != sscanf(line, " %x", &tmp)){
			fprintf(stderr, "Invalid line %d\n", i + 1);
			fclose(fp);
			return;
		}
		arr[i] = tmp;
		//printf("%02X\n", arr[i]);
	}
	fclose(fp);
//step 3: configure audio chip
	if(SUCCESS != configurateAudioChip(fd, arr))
		fprintf(stderr, "Failed to configure audio chip\n");
	else
		printf("Configure audio chip done\n");
}

/****************************** update audio signal ********************************/
void update(int fd, const char* s)
{
    char file[512];
    double num = 0;
//step 0: get parameters
    int res = sscanf(s, " %s %lf", file, &num);
    if(res < 1){
        fprintf(stderr, "Invalid parameters!\n");
        return;
    }
    if(res == 1) num = 1.0;
//step 1: open file
	FILE* fp = fopen(file, "r");
	if(fp == NULL){
		fprintf(stderr, "Invalid file path\n");
		return;
	}
//step 2: read number to update
	int length, i;
	int signal[256];
	if(1 != fscanf(fp, " %d", &length) || length < 1 || length > 256){
		fprintf(stderr, "Invalid signal length\n");
		fclose(fp);
		return;
	}
	const double magnitude = pow(2, 23);
    double tmp, positiveLimit = (magnitude - 1)/magnitude, negativeLimit = -1.0;
	for(i = 0; i < length; ++i){
		if(1 != fscanf(fp, " %lf", &tmp)){
			fprintf(stderr, "Invalid line %d\n", i + 1);
			fclose(fp);
			return;
		}
		tmp *= num;
		if(tmp > positiveLimit) tmp = positiveLimit;
		else if(tmp < negativeLimit) tmp = negativeLimit;
		signal[i] = tmp * magnitude;
		printf("signal[%03d] = %08X\n", i, signal[i]);
	}
	fclose(fp);
//step 3: update audio date
	if(SUCCESS != configurateAudioSignal(fd, signal, length))
		fprintf(stderr, "Failed to fully update audio data\n");
	else
		printf("Successfully update %d data\n", length);
}

/****************************** read audio signal ********************************/
int stricmp(const char* a, const char* b)
{
	for(; *a && *b; ++a, ++b){
		if(toupper(*a) != toupper(*b)) break;
	}
	return toupper(*a) - toupper(*b);
}
void readSignal(int fd, const char* fileFormat)
{
//step 1: check if command is ok
	char file[80], format[80];
	int n = sscanf(fileFormat, " %s %s", file, format);
	if(n == 1 ||
	  (n == 2 && stricmp(format, "hex") != 0 && stricmp(format, "dec") != 0)
	  ){
		fprintf(stderr, "Invalid command\n");
		return;
	}
//step 2: check if to write info file and in what format
	FILE* fp = NULL;
	int hex = -1;
	if(n == 2){
		fp = fopen(file, "w");
		if(fp == NULL){
			fprintf(stderr, "Invalid file path");
			return;
		}
		hex = stricmp(format, "hex") == 0;
	}

	UCHAR maxIndex, data[3];
	int i, count;
	if(SUCCESS != readRegister(fd, AUDIO_DATA_MAX_ADDR, &maxIndex)){
		fprintf(stderr, "Failed to read AUDIO_DATA_MAX_ADDR\n");
		if(n == 2) fclose(fp);
		return;
	}
	printf("Signal length = %u\n", maxIndex);
	const double magnitude = pow(2, 23) - 1;
	for(i = 0, count = maxIndex; i <= count; ++i){
		if(SUCCESS != readAudioSignal(fd, i & 0xFF, data))
			fprintf(stderr, "Failed to read signal[%d]\n", i);
		else if(hex == -1){
			printf("signal[%d] = %02X%02X%02X => %.17f\n",
					i, data[0], data[1], data[2],
					((data[0] & 0x80 ? 0xFF : 0x00) << 24  |
					 data[0] << 16 | data[1] << 8 | data[2]) / magnitude
			);
		}
		else if(hex == 1){
			fprintf(fp, "%02X%02X%02X\n", data[0], data[1], data[2]);
		}
		else{
			fprintf(fp, "%.17f\n", (int)((data[0] & 0x80 ? 0xFF : 0x00) << 24  |
					 data[0] << 16 | data[1] << 8 | data[2]) / magnitude
			);
		}
	}
	if(n == 2) fclose(fp);
}

/****************************** timing service ********************************/
static const UCHAR timingAddr[] = {
		SET_YEAR_H,
		SET_YEAR_L,
		SET_MONTH,
		SET_DAY,
		SET_HOUR,
		SET_MINUTE,
		SET_SECOND
};
void writeTiming(int fd, const char* time)
{
	UCHAR arr[7];
	if(SUCCESS != parseTime(arr, time)){
		fprintf(stderr, "Invalid time\n");
		return;
	}
	if(SUCCESS != writeRegisters(fd, timingAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Timing done\n");
}
void readTiming(int fd)
{
	UCHAR arr[7];
	if(SUCCESS != readRegisters(fd, timingAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Timing UTC: %u%u-%02u-%02u %02u:%02u:%02u\n", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6]);
}
void readTWAIT(int fd)
{
	UCHAR data;
	if(SUCCESS != readRegister(fd, T_WAIT_PPS, &data)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("T_Wait: %02X\n", data);
}

/****************************** transmit start ********************************/
static const UCHAR transmitStartAddr[] = {
		AUDIO_TX_START_YEAR_H,
		AUDIO_TX_START_YEAR_L,
		AUDIO_TX_START_MONTH,
		AUDIO_TX_START_DAY,
		AUDIO_TX_START_HOUR,
		AUDIO_TX_START_MINUTE,
		AUDIO_TX_START_SECOND
};
void writeTransmitStart(int fd, const char* time)
{
	UCHAR arr[7];
	if(SUCCESS != parseTime(arr, time)){
		fprintf(stderr, "Invalid time\n");
		return;
	}
	if(SUCCESS != writeRegisters(fd, transmitStartAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Set transmit start done\n");
}
void readTransmitStart(int fd)
{
	UCHAR arr[7];
	if(SUCCESS != readRegisters(fd, transmitStartAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Transmit start: %u%u-%02u-%02u %02u:%02u:%02u\n", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6]);
}
void startTransmit(int fd)
{
	if(SUCCESS != writeRegister(fd, AUDIO_TX_PREPARE, 0x01)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Start transmitting done\n");
}

/****************************** transmit stop ********************************/
static const UCHAR transmitStopAddr[] = {
		AUDIO_TX_STOP_YEAR_H,
		AUDIO_TX_STOP_YEAR_L,
		AUDIO_TX_STOP_MONTH,
		AUDIO_TX_STOP_DAY,
		AUDIO_TX_STOP_HOUR,
		AUDIO_TX_STOP_MINUTE,
		AUDIO_TX_STOP_SECOND
};
void writeTransmitStop(int fd, const char* time)
{
	UCHAR arr[7];
	if(SUCCESS != parseTime(arr, time)){
		fprintf(stderr, "Invalid time\n");
		return;
	}
	if(SUCCESS != writeRegisters(fd, transmitStopAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Set transmit stop done\n");
}
void readTransmitStop(int fd)
{
	UCHAR arr[7];
	if(SUCCESS != readRegisters(fd, transmitStopAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Transmit stop: %u%u-%02u-%02u %02u:%02u:%02u\n", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6]);
}

/****************************** receive start ********************************/
static const UCHAR receiveStartAddr[] = {
		AUDIO_RX_START_YEAR_H,
		AUDIO_RX_START_YEAR_L,
		AUDIO_RX_START_MONTH,
		AUDIO_RX_START_DAY,
		AUDIO_RX_START_HOUR,
		AUDIO_RX_START_MINUTE,
		AUDIO_RX_START_SECOND
};
void writeReceiveStart(int fd, const char* time)
{
	UCHAR arr[7];
	if(SUCCESS != parseTime(arr, time)){
		fprintf(stderr, "Invalid time\n");
		return;
	}
	if(SUCCESS != writeRegisters(fd, receiveStartAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Set receive start done\n");
}
void readReceiveStart(int fd)
{
	UCHAR arr[7];
	if(SUCCESS != readRegisters(fd, receiveStartAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Receive start: %u%u-%02u-%02u %02u:%02u:%02u\n", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6]);
}
void startReceive(int fd)
{
	if(SUCCESS != writeRegister(fd, AUDIO_RX_PREPARE, 0x01)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Start receiving done\n");
}

/****************************** transmit stop ********************************/
static const UCHAR receiveStopAddr[] = {
		AUDIO_RX_STOP_YEAR_H,
		AUDIO_RX_STOP_YEAR_L,
		AUDIO_RX_STOP_MONTH,
		AUDIO_RX_STOP_DAY,
		AUDIO_RX_STOP_HOUR,
		AUDIO_RX_STOP_MINUTE,
		AUDIO_RX_STOP_SECOND
};
void writeReceiveStop(int fd, const char* time)
{
	UCHAR arr[7];
	if(SUCCESS != parseTime(arr, time)){
		fprintf(stderr, "Invalid time\n");
		return;
	}
	if(SUCCESS != writeRegisters(fd, receiveStopAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Set receive stop done\n");
}
void readReceiveStop(int fd)
{
	UCHAR arr[7];
	if(SUCCESS != readRegisters(fd, receiveStopAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Receive stop: %u%u-%02u-%02u %02u:%02u:%02u\n", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6]);
}

/****************************** frame number ********************************/
static const UCHAR frameAddr[] = {
	ALL_FRAME_NUM_H,
	ALL_FRAME_NUM_M,
	ALL_FRAME_NUM_L
};
void readFrame(int fd)
{
	UCHAR arr[3];
	if(SUCCESS != readRegisters(fd, frameAddr, arr, 3)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Frame number: %u\n", (arr[0] << 16) | (arr[1] << 8) | arr[2]);
}

/****************************** fpga utc ********************************/
static const UCHAR utcAddr[] = {
		UTC_SECOND,
		UTC_YEAR_H,
		UTC_YEAR_L,
		UTC_MONTH,
		UTC_DAY,
		UTC_HOUR,
		UTC_MINUTE,
};
void readUTC(int fd)
{
	UCHAR arr[7];
	if(SUCCESS != readRegisters(fd, utcAddr, arr, 7)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("UTC in fpga: %02u%02u-%02u-%02u %02u:%02u:%02u\n", arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[0]);
}

/****************************** accurate receive start ********************************/
static const UCHAR accurateStartAddr[] = {
		AUDIO_RX_ACCU_YEAR_H,
		AUDIO_RX_ACCU_YEAR_L,
		AUDIO_RX_ACCU_MONTH,
		AUDIO_RX_ACCU_DAY,
		AUDIO_RX_ACCU_HOUR,
		AUDIO_RX_ACCU_MINUTE,
		AUDIO_RX_ACCU_SECOND,
		AUDIO_RX_ACCU_MS_H,
		AUDIO_RX_ACCU_MS_L,
		AUDIO_RX_ACCU_US_H,
		AUDIO_RX_ACCU_US_L
};
void readAccurate(int fd)
{
	UCHAR arr[11];
	if(SUCCESS != readRegisters(fd, accurateStartAddr, arr, 11)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Accurate receive start: %u%u-%02u-%02u %02u:%02u:%02u %u %u\n",
			arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6],
			(UINT)100 * arr[7] + arr[8], (UINT)100 * arr[9] + arr[10]);
}

/****************************** mission state ********************************/
void readMissionState(int fd)
{
	UCHAR data;
	if(SUCCESS != readRegister(fd, MISSION_STATE, &data)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Mission state: %02X\n", data);
}

/****************************** error info ********************************/
void readErrorInfo(int fd)
{
	UCHAR data;
	if(SUCCESS != readRegister(fd, ERROR_INFO, &data)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Error info: %02X\n", data);
}

/****************************** reset mission ********************************/
void resetMission(int fd)
{
	if(SUCCESS != writeRegister(fd, MISSION_RESET, 0x03)){
		fprintf(stderr, "Failed\n");
		return;
	}
	printf("Reset done\n");
}
