//local headers
#include "user_type.h"
#include "serial_port.h"

//standard C headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

//linux headers
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <sys/io.h>
#include <termios.h>
#include <sys/time.h>


int getBaudRateType(int baudRate)
{
	//if not in valid range
	if(baudRate <= 0 || baudRate > 1500000) return -1;
	//if supported standard
	if(baudRate == 110 ||
	   baudRate == 300 ||
	   baudRate == 600 ||
	   baudRate == 1200 ||
	   baudRate == 2400 ||
	   baudRate == 4800 ||
	   baudRate == 9600 ||
	   baudRate == 14400 ||
	   baudRate == 19200 ||
	   baudRate == 38400 ||
	   baudRate == 57600 ||
	   baudRate == 115200
	  ) return 0;
	//if supported nonstandard
	if(115200 % baudRate == 0 && 115200 / baudRate < 256) return 1;
	if(460800 % baudRate == 0 && 460800 / baudRate < 256) return 1;
	if(921600 % baudRate == 0 && 921600 / baudRate < 256) return 1;
	if(1500000 % baudRate == 0 && 1500000 / baudRate < 256) return 1;

	return -1;
}
int isSupportedBaudRate(int baudRate)
{
	return getBaudRateType(baudRate) >= 0;
}

int isSupportedDataBits(int dataBits)
{
	return dataBits >= 5 && dataBits <= 8;
}

int isSupportedParity(char parity)
{
	return parity == 'N' || parity == 'n' ||
		   parity == 'O' || parity == 'o' ||
		   parity == 'E' || parity == 'e' ||
		   parity == 'S' || parity == 's';
}

int isSupportedStopBits(int stopBits)
{
	return stopBits == 1 || stopBits == 2;
}


int closeSerialPort(int fd)
{
	if(0 == close(fd)) return SUCCESS;
	return FAILURE;
}
int openSerialPort(const char* dev, const char* mode, int blocking)
{
	int fd = -1;
	if(strcmp(mode, "ReadOnly") == 0)
		fd = open(dev, O_RDONLY | O_NOCTTY | O_NDELAY);
	else if(strcmp(mode, "WriteOnly") == 0)
		fd = open(dev, O_WRONLY | O_NOCTTY | O_NDELAY);
	else if(strcmp(mode, "ReadWrite") == 0)
		fd = open(dev, O_RDWR   | O_NOCTTY | O_NDELAY);
	else
		fprintf(stderr, "error@openSerialPort: invalid open mode\n");
	if(fd > 0 && blocking){
		if(0 > fcntl(fd, F_SETFL, 0)) perror("fcntl F_SETFL");
	}
	return fd;
}

int configurateSerialPort(int fd, int baud, int dataBits, char parity, int stopBits, int minToRead, int timeoutBetweenBytes)
{
	struct termios config;

//step 0: get current attribute of the serial port
	if(tcgetattr(fd, &config) != 0){
		fprintf(stderr, "error@configSerialPort: fail to tcgetattr(fd, &config)\n");
		return FAILURE;
	}

//step 1: translate baud into a system parameter
	switch(baud){
		case 115200: baud = B115200; break;
		case 57600:  baud = B57600;  break;
		case 38400:  baud = B38400;  break;
		case 19200:  baud = B19200;  break;
		case 9600:   baud = B9600;   break;
		case 4800:   baud = B4800;   break;
		case 2400:   baud = B2400;   break;
		case 1200:   baud = B1200;   break;
		case 300:    baud = B300;    break;
		case 110:    baud = B110;    break;
		default:
			fprintf(stderr, "error@configSerialPort: invalid baud rate\n");
			return FAILURE;
	}

//step 2: set input/output baud
	if(cfsetispeed(&config, baud) != 0){
		fprintf(stderr, "error@configSerialPort: fail to cfsetispeed(&config, baud)\n");
		return FAILURE;
	}
    if(cfsetospeed(&config, baud) != 0){
    	fprintf(stderr, "error@configSerialPort: fail to cfsetospeed(&config, baud)\n");
		return FAILURE;
	}

//step 3: set stream attribute
	//to disable input software stream control
	config.c_iflag &= ~(IXON | IXOFF | IXANY);
	config.c_iflag &= ~(INLCR | ICRNL | IGNCR);
	//to let every char for every char
	config.c_oflag  &= ~OPOST;
	config.c_oflag &= ~(ONLCR | OCRNL);
	//to make output no wait for a '\n'
    config.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);

//step 4: set data bits
	config.c_cflag &= ~CSIZE;
	switch (dataBits){
		case 5: config.c_cflag |= CS5; break;
	    case 6: config.c_cflag |= CS6; break;
        case 7: config.c_cflag |= CS7; break;
        case 8: config.c_cflag |= CS8; break;
        default:
        	fprintf(stderr, "error@configSerialPort: invalid data bits\n");
        	return FAILURE;
    }

//step 5: set parity
    switch (parity){
        case 'n': case 'N':
            config.c_cflag &= ~PARENB;    /* Clear parity enable */
            config.c_iflag &= ~INPCK;     /* Enable parity checking */
            break;
        case 'o': case 'O':
            config.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
            config.c_iflag |= INPCK;             /* Disnable parity checking */
            break;
        case 'e': case 'E':
            config.c_cflag |= PARENB;      /* Enable parity */
            config.c_cflag &= ~PARODD;     /* 转换为偶效验*/
            config.c_iflag |= INPCK;       /* Disnable parity checking */
            break;
        case 'S': case 's':
            config.c_cflag &= ~PARENB;		/*as no parity*/
            config.c_cflag &= ~CSTOPB;
            break;
        default:
        	fprintf(stderr, "error@configSerialPort: invalid parity\n");
            return FAILURE;
    }

//step 6: set stop bits
    switch (stopBits){
        case 1:
            config.c_cflag &= ~CSTOPB; break;
        case 2:
            config.c_cflag |= CSTOPB;  break;
        default:
        	fprintf(stderr, "error@configSerialPort: invalid stop bits\n");
        	return FAILURE;
	}

//step 7: set read attribute
	config.c_cc[VMIN] = minToRead;
	config.c_cc[VTIME]  = timeoutBetweenBytes;

//step 8: set configuration to serial port
	if(tcsetattr(fd, TCSANOW, &config) != 0){
		fprintf(stderr, "error@configSerialPort: fail to tcsetattr(fd, TCSANOW, &config)\n");
		return FAILURE;
	}

//step 9: flush input/output queues
	if(tcflush(fd, TCIOFLUSH) != 0){
		fprintf(stderr, "error@configSerialPort: fail to tcflush(fd, TCIOFLUSH)\n");
		return FAILURE;
	}

	return SUCCESS;
}

static int  getBaseBaud(int customBaud)
{
	if(customBaud <= 0) return 0;
	if(115200 % customBaud == 0 && 115200 / customBaud < 256) return 115200;
	if(460800 % customBaud == 0 && 460800 / customBaud < 256) return 460800;
	if(921600 % customBaud == 0 && 921600 / customBaud < 256) return 921600;
	if(1500000 % customBaud == 0 && 1500000 / customBaud < 256) return 1500000;
	return -1;
}
static void setSerialPortRegister(unsigned short serialPortAddr, int baseBaud, int customBaud)
{
	unsigned short enableRegAddr = serialPortAddr + 3;
	unsigned char DLM = 0x00, DLL = (baseBaud / customBaud) & 0xFF, tmp;

	//step 1: set high byte of divisor latch
	switch(baseBaud){
		case 115200:  DLM = 0x00; break;
		case 460800:  DLM = 0x80; break;
		case 921600:  DLM = 0xC0; break;
		case 1500000: DLM = 0x20; break;
	}

	//step 2: change permission and change register
	ioperm(enableRegAddr, 1, 1);
	tmp = inb(enableRegAddr);
	outb(0x80 | tmp, enableRegAddr);

	ioperm(serialPortAddr, 2, 1);
	outb(DLL, serialPortAddr + 0);
	outb(DLM, serialPortAddr + 1);

	//step 3: retrieve permission
	outb(tmp, enableRegAddr);
	ioperm(enableRegAddr, 1, 0);
	ioperm(serialPortAddr, 2, 0);
}
static int openSerialPortAtNonStandardBaud(const char* dev, const char* mode, int blocking, const struct Config* pConfig)
{
	int customBaud = pConfig->baudRate, baseBaud = getBaseBaud(pConfig->baudRate);

//step 0: check if customBaud is supported
	if(baseBaud < 0){
		fprintf(stderr, "error@openSerialPortAtNonStandardBaud: invalid baud rate\n");
		return -1;
	}

//step 1: open serial port
	int fd = openSerialPort(dev, mode, blocking);
	if(fd < 0) return -2;
	//set hardware control
	struct serial_struct ss;
	if(ioctl(fd, TIOCGSERIAL, &ss) < 0){
		fprintf(stderr, "error@openSerialPortAtNonStandardBaud: fail to ioctl(fd, TIOCGSERIAL, &ss)\n");
		return -3;
	}
	ss.flags = ASYNC_SPD_CUST;
	ss.baud_base = baseBaud;
	ss.custom_divisor = ss.baud_base / customBaud;
	//add hardware control
	if(ioctl(fd, TIOCSSERIAL, &ss) < 0){
		fprintf(stderr, "error@openSerialPortAtNonStandardBaud: fail to ioctl(fd, TIOCSSERIAL, &ss)\n");
		return -4;
	}
	if(ioctl(fd, TIOCGSERIAL, &ss) < 0){
		fprintf(stderr, "error@openSerialPortAtNonStandardBaud: fail to ioctl(fd, TIOCGSERIAL, &ss)\n");
		return -5;
	}

//step 3: set configuration
	if(SUCCESS != configurateSerialPort(fd, 38400,	//must set baud rate to 38400 to get custom baud
								 pConfig->dataBits,
								 pConfig->parity,
								 pConfig->stopBits,
								 pConfig->minToRead,
								 pConfig->timeoutBetweenBytes)){
		fprintf(stderr, "error@openSerialPortAtNonStandardBaud: fail to configurateSerialPort(...)\n");
		return -6;
	}

//step 4: change register to reinterpret 38400
	setSerialPortRegister(pConfig->baseAddr, baseBaud, customBaud);

	return fd;
}

int openAndConfigurateSerialPort(const char* dev, const char* mode, int blocking, const struct Config* pConfig)
{
	int fd, type = getBaudRateType(pConfig->baudRate);
	//check if baud rate supported
	if(type < 0){
		fprintf(stderr, "error@openSerialPort: invalid baud rate\n");
		return -1;
	}
	//check if data bits supported
	if(!isSupportedDataBits(pConfig->dataBits)){
		fprintf(stderr, "error@openSerialPort: invalid data bits\n");
		return -2;
	}
	//check if parity supported
	if(!isSupportedParity(pConfig->parity)){
		fprintf(stderr, "error@openSerialPort: invalid parity\n");
		return -3;
	}
	//check if stop bits supported
	if(!isSupportedStopBits(pConfig->stopBits)){
		fprintf(stderr, "error@openSerialPort: invalid stop bits\n");
		return -4;
	}

	//baud rate is supported nonstandard baud rate
	if(type == 1){
		fd = openSerialPortAtNonStandardBaud(dev, mode, blocking, pConfig);
		if(fd < 0) fprintf(stderr, "error@openSerialPort: fail to openSerialPortAtNonStandardBaud(dev, mode, pConfig)\n");
		return fd;
	}
	//baud rate is standard baud rate
	fd = openSerialPort(dev, mode, blocking);
	if(fd < 0){
		fprintf(stderr, "error@openSerialPort: fail to openSerialPort(dev, mode)\n");
		return -5;
	}
	if(SUCCESS != configurateSerialPort(
			fd,
			pConfig->baudRate,
			pConfig->dataBits,
			pConfig->parity,
			pConfig->stopBits,
			pConfig->minToRead,
			pConfig->timeoutBetweenBytes)
	){
		fprintf(stderr, "error@openSerialPort: fail to configurateSerialPort(...)\n");
		return -6;
	}

	return fd;
}

int send(int fd, void* frame, int len)
{
	int nsent = 0, alreadySent = 0;
	
	for(; alreadySent < len; alreadySent += nsent){
		nsent = write(fd, frame + alreadySent, len - alreadySent);
		if(nsent <= 0){
			fprintf(stderr, "error@sendFrame: fail to write(fd, frame + alreadySent, len - alreadySent)\n");
			break;
		}
	}
	return alreadySent;
}
static int tread(int fd, void *buf, int maxSize, struct timeval* ptv)
{
	int    nfds;
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    nfds = select(fd+1, &readfds, NULL, NULL, ptv);
    if(nfds <= 0){
    	if(nfds == 0) errno = ETIME;
        return -1;
    }

    return read(fd, buf, maxSize);
}
int treadn(int fd, void *buf, int nbytes, int timeoutSec, int timeoutMicrosec)
{
	int nleft;
    int nread;
    struct timeval tv;

    tv.tv_sec  = timeoutSec;
    tv.tv_usec = timeoutMicrosec;

    nleft = nbytes;
    while(nleft > 0){
    	if((nread = tread(fd, buf, nleft, &tv)) < 0){
    		if (nleft == nbytes) return -1; /* error, received nothing at all return -1 */
            else break; /* error, return amount received so far */
        }
    	else if (nread == 0) break;
        nleft -= nread;
        buf   += nread;
    }

    return nbytes - nleft; /* return >= 0 */
}
int receive(int fd, void* frame, int maxLen)
{
	return read(fd, frame, maxLen);
}


