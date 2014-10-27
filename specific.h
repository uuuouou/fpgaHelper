/*
 * specific.h
 *
 *  Created on: May 5, 2014
 *      Author: root
 */

#ifndef SPECIFIC_H_
#define SPECIFIC_H_

#include "user_type.h"

#define WRITE				0xA5
#define READ				0xC9
#define CMD_FRAME_SIZE		4
#define ACK_FRAME_SIZE		4
#define MAX_FRAME_SIZE		256


/* Specification:
 *
 * Command frame is in following specific form:
 * frame[] = {head, addr, data, check}, where each file is one unsigned char value, which means command frame' length is 4.
 * 1.head has two simple kind: WRITE/READ, representing it's to write or read
 * 2.addr represents for the address of target register to access
 * 3.when it's to read, data field just makes no difference
 *   when it's to write, data is the new value to be written into target register
 * 4.here check simply equals to (addr + data) & 0xFF
 *
 * After each command frame send to FPGA, there will be ack back, which has exactly the same form as command frame.
 * 1.head is same as command frame
 * 2.addr is same as command frame
 * 3.when it's to read, data is current value of the register corresponding to addr
 *   when it's to write, data is the value of target register after written done
 * 4.ack's check simply equals to ~((addr + data) & 0xFF)
 *
 * Note:
 * when it's to write, expected ack follows that checkOfACK + checkOfCMD = 0xFF
 * when it's to read,  checkOfACK has no relation with checkOfCMD
*/


/*
 * This function is used to prepare command frame to send to FPGA
 * @param cmd:  unsigned char array has at least CMD_FRAME_SIZE bytes
 * @param head: WRITE or READ
 * @param addr: address of target register
 * @param data: data
 */
void prepareCommand(UCHAR* cmd, UCHAR  head, UCHAR  addr, UCHAR  data);

/*
 * This function is used to check if ack of write command is good or not
 * @param cmd: command frame
 * @param ack: ack frame
 * @return:
 *			1 if ack is good
 *			0 if ack is not good
 */
int checkWriteCommandACK(const UCHAR* ack, int ackLength, const UCHAR* cmd);

/*
 * This function is used to check if ack of read command is good or not
 * @param cmd: 		 command frame
 * @param ackLength: length of ack frame
 * @param ack: 		 ack frame
 * @return:
 *			1 if ack is good
 *			0 if ack is not good
 */
int checkReadCommandACK(const UCHAR* ack, int ackLength, const UCHAR* cmd);

/*
 * This function is used to set cmd with given datas, send it and check the ack
 * @param fd:	serial port handle
 * @param head: WRITE or READ
 * @param addr: address of target register
 * @param data: data to send to FPGA
 * @return:
 * 			SUCCESS if all good
 * 			FAILURE if something wrong happened

int sendCommandAndCheckACK(int fd, UCHAR head, UCHAR addr, UCHAR data);
 */

/*
 * This function is used to read a register in FPGA
 * @param fd: serial port handle
 * @param addr: address of target register
 * @param data: target data to write into the register
 * @return:
 * 			SUCCESS if all good
 * 			FAILURE if something wrong happened
 */
int writeRegister(int fd, UCHAR addr, UCHAR data);
int writeRegisters(int fd, const UCHAR addr[], UCHAR arr[], int n);
/*
 * This function is used to read a register in FPGA
 * @param fd: serial port handle
 * @param addr: address of target register
 * @param ack:  if ack != NULL, ack frame will be stored in ack[]
 * @return:
 * 			SUCCESS if all good
 * 			FAILURE if something wrong happened
 */
int readRegister(int fd, UCHAR addr, UCHAR* pData);
int readRegisters(int fd, const UCHAR addr[], UCHAR arr[], int n);

#endif /* SPECIFIC_H_ */
