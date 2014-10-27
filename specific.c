/*
 * specific.c
 *
 *  Created on: May 5, 2014
 *      Author: root
 */

//local headers
#include "specific.h"
#include "serial_port.h"

#include <stdio.h>

void prepareCommand(UCHAR* cmd, UCHAR head, UCHAR addr, UCHAR data)
{
	cmd[0] = head;
	cmd[1] = addr;
	cmd[2] = data;
	cmd[3] = addr + data;
}

int checkWriteCommandACK(const UCHAR* ack, int ackLength, const UCHAR* cmd)
{
/*
	int i = 0;
	printf("ack back is:");
	for(; i < ackLength; ++i) printf(" %02X", ack[i]);
	puts("");
*/
	int flag = 1;
	if(ackLength != ACK_FRAME_SIZE) flag = 0;
	if(ack[0] != cmd[0]) flag = 0;
	if(ack[1] != cmd[1]) flag = 0;
	if(ack[2] != cmd[2]) flag = 0;
	if(((cmd[3] + ack[3]) & 0xFF) != 0xFF) flag = 0;
	if(flag == 0){
		int i = 0;
		for(; i < ackLength; ++i) printf("%02X ", ack[i]);
		printf("\n");
	}
	return flag;
}

int checkReadCommandACK(const UCHAR* ack, int ackLength, const UCHAR* cmd)
{
/*
	int i = 0;
	printf("ack back is:");
	for(; i < ackLength; ++i) printf(" %02X", ack[i]);
	puts("");
*/
	if(ackLength != ACK_FRAME_SIZE) return 0;
	if(ack[0] != cmd[0]) return 0;
	if(ack[1] != cmd[1]) return 0;
	return ((ack[1] + ack[2] + ack[3]) & 0xFF) == 0xFF;
}
/*
int sendCommandAndCheckACK(int fd, UCHAR head, UCHAR addr, UCHAR data)
{
	UCHAR cmd[CMD_FRAME_SIZE], ack[MAX_FRAME_SIZE];

	//step 1: send cmd
	prepareCommand(cmd, head, addr, data);
	if(CMD_FRAME_SIZE != send(fd, cmd, CMD_FRAME_SIZE)) return FAILURE;

	//step 2: receive ack and check ack
	if((head == WRITE &&
		checkWriteCommandACK(ack, treadn(fd, ack, MAX_FRAME_SIZE, 0, 1000), cmd)) ||
	   (head == READ  &&
	    checkReadCommandACK(ack, treadn(fd, ack, MAX_FRAME_SIZE, 0, 1000), cmd))){
		return SUCCESS;
	}
	return FAILURE;
}
*/
int readRegister(int fd, UCHAR addr, UCHAR* pData)
{
	UCHAR cmd[CMD_FRAME_SIZE], ack[MAX_FRAME_SIZE];

	//step 1: send cmd
	prepareCommand(cmd, READ, addr, 0x00);
	//printf("cmd to send is %02X %02X %02X %02X\n", cmd[0], cmd[1], cmd[2], cmd[3]);
	if(CMD_FRAME_SIZE != send(fd, cmd, CMD_FRAME_SIZE)) return FAILURE;
	//step 2: receive ack and check ack
	if(checkReadCommandACK(ack, treadn(fd, ack, MAX_FRAME_SIZE, 0, 4000), cmd)){
		*pData = ack[2];
		return SUCCESS;
	}

	return FAILURE;
}
int readRegisters(int fd, const UCHAR addr[], UCHAR arr[], int n)
{
	int i;
	for(i = 0; i < n; ++i){
		if(SUCCESS != readRegister(fd, addr[i], &arr[i])) return FAILURE;
	}
	return SUCCESS;
}
int writeRegister(int fd, UCHAR addr, UCHAR data)
{
	UCHAR cmd[CMD_FRAME_SIZE], ack[MAX_FRAME_SIZE];

	//step 1: send cmd
	prepareCommand(cmd, WRITE, addr, data);
	//printf("cmd to send is %02X %02X %02X %02X\n", cmd[0], cmd[1], cmd[2], cmd[3]);
	if(CMD_FRAME_SIZE != send(fd, cmd, CMD_FRAME_SIZE)) return FAILURE;
	//step 2: receive ack and check ack
	if(checkWriteCommandACK(ack, treadn(fd, ack, MAX_FRAME_SIZE, 0, 2000), cmd)) return SUCCESS;

	return FAILURE;
}
int writeRegisters(int fd, const UCHAR addr[], UCHAR arr[], int n)
{
	int i;
	for(i = 0; i < n; ++i){
		if(SUCCESS != writeRegister(fd, addr[i], arr[i])) return FAILURE;
	}
	return SUCCESS;
}
