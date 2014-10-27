/*
 * fpga.h
 *
 *  Created on: Apr 22, 2014
 *      Author: root
 */

#ifndef FPGA_H_
#define FPGA_H_

/*
 * This part defines register name with corresponding address in FPGA
 */
#define     CHIP_VER                    0x00       //芯片硬件版本号

#define     ATOM_CLOCK_CTRL             0x01       //原子钟驯服及测试控制寄存器

#define     SET_YEAR_H                  0x02       //配置UTC时间：年高两位
#define     SET_YEAR_L                  0x03       //配置UTC时间：年低两位
#define     SET_MONTH                   0x04       //配置UTC时间：月
#define     SET_DAY                     0x05       //配置UTC时间：日
#define     SET_HOUR                    0x06       //配置UTC时间：时
#define     SET_MINUTE                  0x07       //配置UTC时间：分
#define     SET_SECOND                  0x08       //配置UTC时间：秒
#define     T_WAIT_PPS                  0x09       //配置秒结束后距下一个秒脉冲到来的时间差

#define     AUDIO_DATA_UPDATE_ADDR      0x0a       //待更新的音频发送数据的地址
#define     AUDIO_DATA_UPDATE_HIG       0x0b       //待更新的音频发送数据的高8位
#define     AUDIO_DATA_UPDATE_MID       0x0c       //待更新的音频发送数据的中8位
#define     AUDIO_DATA_UPDATE_LOW       0x0d       //待更新的音频发送数据的低8位
#define     AUDIO_DATA_UPDATE_EN        0x0e       //更新使能
#define     AUDIO_DATA_MAX_ADDR         0x0f       //RAM中待发送数据的最大地址

#define     AIC_CFG_ADDR                0x10       //配置音频芯片内寄存器的地址
#define     AIC_CFG_DATA                0x11       //配置音频芯片内寄存器的数据
#define     AIC_CFG_STATE               0x12       //音频芯片寄存器配置状态标志

#define     AUDIO_TX_START_YEAR_H       0x13       //音频发送的起始时间：年高8位
#define     AUDIO_TX_START_YEAR_L       0x14       //音频发送的起始时间：年低8位
#define     AUDIO_TX_START_MONTH        0x15       //音频发送的起始时间：月
#define     AUDIO_TX_START_DAY          0x16       //音频发送的起始时间：日
#define     AUDIO_TX_START_HOUR         0x17       //音频发送的起始时间：时
#define     AUDIO_TX_START_MINUTE       0x18       //音频发送的起始时间：分
#define     AUDIO_TX_START_SECOND       0x19       //音频发送的起始时间：秒
#define     AUDIO_TX_STOP_YEAR_H        0x1a       //音频发送的终止时间：年高8位
#define     AUDIO_TX_STOP_YEAR_L        0x1b       //音频发送的终止时间：年低8位
#define     AUDIO_TX_STOP_MONTH         0x1c       //音频发送的终止时间：月
#define     AUDIO_TX_STOP_DAY           0x1d       //音频发送的终止时间：日
#define     AUDIO_TX_STOP_HOUR          0x1e       //音频发送的终止时间：时
#define     AUDIO_TX_STOP_MINUTE        0x1f       //音频发送的终止时间：分
#define     AUDIO_TX_STOP_SECOND        0x20       //音频发送的终止时间：秒
#define     AUDIO_TX_PREPARE            0x21       //音频发送准备

#define     AUDIO_RX_START_YEAR_H       0x22       //音频接收的起始时间：年高8位
#define     AUDIO_RX_START_YEAR_L       0x23       //音频接收的起始时间：年低8位
#define     AUDIO_RX_START_MONTH        0x24       //音频接收的起始时间：月
#define     AUDIO_RX_START_DAY          0x25       //音频接收的起始时间：日
#define     AUDIO_RX_START_HOUR         0x26       //音频接收的起始时间：时
#define     AUDIO_RX_START_MINUTE       0x27       //音频接收的起始时间：分
#define     AUDIO_RX_START_SECOND       0x28       //音频接收的起始时间：秒
#define     AUDIO_RX_STOP_YEAR_H        0x29       //音频接收的终止时间：年高8位
#define     AUDIO_RX_STOP_YEAR_L        0x2a       //音频接收的终止时间：年低8位
#define     AUDIO_RX_STOP_MONTH         0x2b       //音频接收的终止时间：月
#define     AUDIO_RX_STOP_DAY           0x2c       //音频接收的终止时间：日
#define     AUDIO_RX_STOP_HOUR          0x2d       //音频接收的终止时间：时
#define     AUDIO_RX_STOP_MINUTE        0x2e       //音频接收的终止时间：分
#define     AUDIO_RX_STOP_SECOND        0x2f       //音频接收的终止时间：秒
#define     AUDIO_RX_PREPARE            0x30       //音频接收准备

#define     UTC_YEAR_H                  0x31       //当前UTC时间：年高两位
#define     UTC_YEAR_L                  0x32       //当前UTC时间：年低两位
#define     UTC_MONTH                   0x33       //当前UTC时间：月
#define     UTC_DAY                     0x34       //当前UTC时间：日
#define     UTC_HOUR                    0x35       //当前UTC时间：时
#define     UTC_MINUTE                  0x36       //当前UTC时间：分
#define     UTC_SECOND                  0x37       //当前UTC时间：秒

#define     AUDIO_RX_ACCU_YEAR_H        0x38       //音频接收的准确时间：年高两位
#define     AUDIO_RX_ACCU_YEAR_L        0x39       //音频接收的准确时间：年低两位
#define     AUDIO_RX_ACCU_MONTH         0x3a       //音频接收的准确时间：月
#define     AUDIO_RX_ACCU_DAY           0x3b       //音频接收的准确时间：日
#define     AUDIO_RX_ACCU_HOUR          0x3c       //音频接收的准确时间：时
#define     AUDIO_RX_ACCU_MINUTE        0x3d       //音频接收的准确时间：分
#define     AUDIO_RX_ACCU_SECOND        0x3e       //音频接收的准确时间：秒
#define     AUDIO_RX_ACCU_MS_H          0x3f       //音频接收的准确时间：毫秒高位
#define     AUDIO_RX_ACCU_MS_L          0x40       //音频接收的准确时间：毫秒低位
#define     AUDIO_RX_ACCU_US_H          0x41       //音频接收的准确时间：微秒高位
#define     AUDIO_RX_ACCU_US_L          0x42       //音频接收的准确时间：微秒低位

#define     ALL_FRAME_NUM_H				0x43
#define     ALL_FRAME_NUM_M				0x44
#define     ALL_FRAME_NUM_L				0x45

#define 	AUDIO_DATA_RD_ADDR			0x46
#define 	AUDIO_DATA_RD_HIG			0x47
#define 	AUDIO_DATA_RD_MID			0x48
#define 	AUDIO_DATA_RD_LOW			0x49

#define     MISSION_STATE               0xf5       //当前任务状态
#define     ERROR_INFO					0xf6	   //出错信息
#define     MISSION_RESET               0xfa       //任务复位


/*
 * This part defines all kinds of states in FPGA.
 */
#define     TX_NO_MISSION               0x00	//3'b000 << 3     //初始状态,无发送任务
#define     TX_READY                    0x08	//3'b001 << 3     //发送准备好
#define     TX_ON                       0x10	//3'b010 << 3      //发送正在进行
#define     TX_OVER                     0x18	//3'b011 << 3      //发送正常结束
#define     TX_ERR                      0x20	//3'b100 << 3      //发送出错
#define     RX_NO_MISSION               0x00	//3'b000      //初始状态，无接收任务
#define     RX_READY                    0x01	//3'b001      //接收准备好
#define     RX_ON                       0x02	//3'b010      //接收正在进行
#define     RX_OVER                     0x03	//3'b011      //接收正常结束
#define     RX_ERR                      0x04	//3'b100      //接收出错

void readAddr(int fd, const char* addr);
void writeAddr(int fd, const char* addrValue);

void writeTiming(int fd, const char* time);
void readTiming(int fd);
void readTWAIT(int fd);

void configure(int fd, const char* file);
void update(int fd, const char* file);
void readSignal(int fd, const char* fileFormat);

void writeTransmitStart(int fd, const char* time);
void readTransmitStart(int fd);
void startTransmit(int fd);

void writeTransmitStop(int fd, const char* time);
void readTransmitStop(int fd);

void writeReceiveStart(int fd, const char* time);
void readReceiveStart(int fd);
void startReceive(int fd);

void writeReceiveStop(int fd, const char* time);
void readReceiveStop(int fd);

void readFrame(int fd);

void readUTC(int fd);

void readAccurate(int fd);

void readMissionState(int fd);

void readErrorInfo(int fd);

void resetMission(int fd);


#endif /* FPGA_H_ */
