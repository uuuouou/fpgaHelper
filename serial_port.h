#ifndef SERIAL_PORT_H_
#define SERIAL_PORT_H_

#include "user_type.h"


/*
 * configuration of a serial port
 */
struct Config{
	int  baudRate;		      //baud rate
	int  dataBits;			  //data bits
	char parity;			  //parity
	int  stopBits;			  //stop bits
	int  minToRead;			  //minimum bytes to return when calling read
	int  timeoutBetweenBytes; //time out between bytes, unit in 100ms

	//baseAddr is used only when baudRate is supported nonstandard baud rate
	USHORT baseAddr;  //serial port's hardware base address
};

/*
 * This function is used to check if the baud rate is supported.
*@param baudRate: baud rate to check
*@return:
  	  	 1 if the baud rate is supported
  	  	 0 otherwise
*/
int isSupportedBaudRate(int baudRate);

/*
 * This function is used to get the type of given baudRate
*@param baudRate: baud rate to check
*@return:
  	  	 0 if given baud is supported standard baud rate
  	  	 1 if given baud is supported nonstandard baud rate
  	  	-1 if given baud is not supported
 */
int getBaudRateType(int baudRate);

/*
 * This function is used to check if the data bits is supported.
*@param dataBits: data bits to check
*@return:
  	  	 1 if the dataBits is supported
  	  	 0 otherwise
*/
int isSupportedDataBits(int dataBits);

/*
 * This function is used to check if the parity is supported.
*@param parity: parity to check
*@return:
  	  	 1 if the parity is supported
  	  	 0 otherwise
*/
int isSupportedParity(char parity);

/*
 * This function is used to check if the stop bits is supported.
*@param stopBits: stop bits to check
*@return:
  	  	 1 if the stopBits is supported
  	  	 0 otherwise
*/
int isSupportedDataBits(int stopBits);




/*
 * This function is used to open a serial port.
*@param dev:   	  serial port's system file path
*@param mode:     "ReadOnly","WriteOnly","ReadWrite"
*@param blocking: whether to set the read mode to be blocking or not
*@return:
  	  	 corresponding file handle if open the serial port successfully
  	  	 a negative number otherwise
*/
int openSerialPort(
	const char* dev,
	const char* mode,
	int         blocking
);



/*
 * This function is used to set configuration for the serial port.
*@param fd:       		     file handle of target serial port
*@param baud:     		   	 38400, 19200, 9600, 4800, 2400, 1200, 300
*@param dataBits:     	   	 7 or 8
*@param stopBits:            1 or 2
*@param parity:              'N' or 'E' or 'O' or 'S'
*@param minToRead: 	   	  	 minimum bytes to return when calling read
*@param timeoutBetweenBytes: time out between bytes, unit in 100ms
*@return:
		 0 if done configuration successfully
   	   	 negative numbers if error happens
*/
int configurateSerialPort(
	int fd,
	int baud,
	int dataBits,
	char parity,
	int stopBits,
	int minToRead,
	int timeoutBetweenBytes
);



/*
 * This function is used to open a serial port.
*@param dev:  	 serial port's system file path
*@param mode: 	 "ReadOnly","WriteOnly","ReadWrite"
*@param blocking: whether to set the read mode to be blocking or not
*@param pConfig: const pointer to a Config structure
*@return:
  	  	 corresponding file handle if open the serial port successfully
  	  	 a negative number otherwise
*/
int openAndConfigurateSerialPort(
	const char* dev,
	const char* mode,
	int         blocking,
	const struct Config* pConfig
);



/*
 * This function is used to close a serial port.
 * @param fd: serial port handle
 * @return:
 *			 SUCCESS if successfully closed the port
 *			 FAILURE if fail to close
 */
int closeSerialPort(int fd);



/*
 * This function is send frame.
*@param fd:    file descriptor of a serial port
*@param frame: pointer of the frame
*@param len:   bytes to send
*@return:
		 bytes successfully sent
*/
int send(
	int   fd,
	void* frame,
	int   len
);



/*
 * This function is receive within given time limit.
*@param fd:    			 file descriptor of a serial port
*@param buf: 			 pointer to where to hold bytes received
*@param nbytes: 		 target bytes to receive
*@param timeoutSec: 	 second part of timeout
*@param timeoutMicrosec: microsecond part of timeout
*@return:
		 if time runs out before nbytes has been received, returns how many actually received
		 else return nbytes
*/
int treadn(int fd, void *buf, int nbytes, int timeoutSec, int timeoutMicrosec);



/*
 * This function is receive frame.
*@param fd:    	file descriptor of a serial port
*@param frame: 	pointer to where to hold bytes received
*@param maxlen: maximum bytes to receive
*@return:
		 bytes successfully read once
*/
int receive(
	int   fd,
	void* frame,
	int   maxLen
);



#endif /* SERIAL_PORT_H_ */
