/*
 * main.cpp
 *
 *  Created on: May 8, 2014
 *      Author: root
 */

//for serial port, aDIO and communication with fpga
extern "C"{
	#include "fpga.h"
	#include "aDIO.h"
	#include "user_type.h"
	#include "serial_port.h"
}
//for command input
#include <readline/readline.h>
#include <readline/history.h>
//for common
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#define CMD_MAX_LENGTH	128


/****************************** global variables ********************************/
const char *commands[] = {
	"help",
	"quit",
	"read",
	"write",
	"configure",
	"update",
	"signal",
	"timing",
	"twait",
	"transmit",
	"transmitstart",
	"transmitstop",
	"receive",
	"receivestart",
	"receivestop",
	"frame",
	"accurate",
	"utc",
	"state",
	"error",
	"reset",
	"aDIO",
	"bd0",
	"port0",
	"pd1",
	"port1"
};


/****************************** functions ********************************/
void initialize_readline();
void printHelp();
bool inputCommand(char*& command);

int main()
{
	int fd;
	char* command;

//step 0: open aDIO
	open_aDIO();
//step 1: open serial port
	fd = openSerialPort("/dev/ttyS0", "ReadWrite", 0);
	if(fd < 0){
		fprintf(stderr, "error@main: fail to open serial port.\n");
		return FAILURE;
	}
//step 2: configurate serial port
	if(SUCCESS != configurateSerialPort(fd, 115200, 8, 'N', 1, 0, 0)){
		fprintf(stderr, "error@main: fail to configure serial port.\n");
		return FAILURE;
	}
//step 3: wait for command and execute
        initialize_readline();
	printHelp();
	while(true){
		//input command from keyboard
		if(!inputCommand(command)) continue;
		//switch command and execute
		if(strcmp(command, "quit") == 0) break;
		else if(strcmp(command, "help") == 0)     printHelp();
		else if(strcmp(command, "utc") == 0)      readUTC(fd);
		else if(strcmp(command, "twait") == 0)    readTWAIT(fd);
		else if(strcmp(command, "state") == 0)    readMissionState(fd);
		else if(strcmp(command, "error") == 0)    readErrorInfo(fd);
		else if(strcmp(command, "reset") == 0)    resetMission(fd);
		else if(strcmp(command, "transmit") == 0) startTransmit(fd);
		else if(strcmp(command, "receive") == 0)  startReceive(fd);
		else if(strcmp(command, "accurate") == 0) readAccurate(fd);
		else if(strcmp(command, "frame") == 0)    readFrame(fd);
                else if(strcmp(command, "aDIO") == 0)     print_aDIO();
		else if(strncmp(command, "bd0", strlen("bd0")) == 0)
			setPort0Direction(command + strlen("bd0"));
		else if(strncmp(command, "pd1", strlen("pd1")) == 0)
			setPort1Direction(command + strlen("pd1"));
		else if(strncmp(command, "port0", strlen("port0")) == 0)
			setPort0Value(command + strlen("port0"));
		else if(strncmp(command, "port1", strlen("port1")) == 0)
			setPort1Value(command + strlen("port1"));
		else if(strncmp(command, "signal", strlen("signal")) == 0)
			readSignal(fd, command + strlen("signal"));
		else if(strncmp(command, "configure", strlen("configure")) == 0)
			configure(fd, command + strlen("configure"));
		else if(strncmp(command, "update", strlen("update")) == 0)
			update(fd, command + strlen("update"));
		else if(strncmp(command, "read", strlen("read")) == 0)
			readAddr(fd, command + strlen("read"));
		else if(strncmp(command, "write", strlen("write")) == 0)
			writeAddr(fd, command + strlen("write"));
		else if(strncmp(command, "timing", strlen("timing")) == 0){
			if(strcmp(command, "timing") == 0) readTiming(fd);
			else writeTiming(fd, command + strlen("timing"));
		}
		else if(strncmp(command, "transmitstart", strlen("transmitstart")) == 0){
			if(strcmp(command, "transmitstart") == 0) readTransmitStart(fd);
			else writeTransmitStart(fd, command + strlen("transmitstart"));
		}
		else if(strncmp(command, "transmitstop", strlen("transmitstop")) == 0){
			if(strcmp(command, "transmitstop") == 0) readTransmitStop(fd);
			else writeTransmitStop(fd, command + strlen("transmitstop"));
		}
		else if(strncmp(command, "receivestart", strlen("receivestart")) == 0){
			if(strcmp(command, "receivestart") == 0) readReceiveStart(fd);
			else writeReceiveStart(fd, command + strlen("receivestart"));
		}
		else if(strncmp(command, "receivestop", strlen("receivestop")) == 0){
			if(strcmp(command, "receivestop") == 0) readReceiveStop(fd);
			else writeReceiveStop(fd, command + strlen("receivestop"));
		}
		else puts("Invalid command");
	}

//final step: clear up
	closeSerialPort(fd);
	close_aDIO();
	return SUCCESS;
}


void printHelp()
{
	puts("\nCommand List:");
	puts("help                      Print the command list.");
	puts("quit                      Exit the program.");
	puts("read start [end]          Read address(hex) from start to end in fpga.");
	puts("write addr value          Write value(hex) to address(hex) in fpga.");
	puts("configure file            Configure audio chip with given file.");
	puts("update file [num]         Update audio signal with data(multiplies num) in given file.");
	puts("signal [file hex|dec]     Read audio signal (and write into file in hex or dec format).");
	puts("timing [time]             Read/Write UTC of timing service.");
	puts("twait                     Read t_wait of timing service.");
	puts("transmitstart [time]      Read/Write start time of transmitting.");
	puts("transmitstop [time]       Write/Read stop time of transmitting.");
	puts("transmit                  Start to transmit.");
	puts("receivestart [time]       Read/Write start time of receiving.");
	puts("receivestop [time]        Read/Write stop time of receiving.");
	puts("receive                   Start to Receive.");
	puts("frame                     Read how many frames sent to CPU.");
	puts("accurate                  Read accurate start time of receiving.");
	puts("utc                       Read UTC of FPGA.");
	puts("state                     Read mission state.");
	puts("error                     Read error info.");
	puts("reset                     Reset state and error.");
	puts("aDIO                      Read the state of aDIO.");
	puts("bd0 val                   Set the bit-direction of Port 0 by val(hex).");
	puts("port0 val                 Set the output value of Port 0 by val(hex).");
	puts("pd1 val                   Set the port direction of Port 1 by val(0 or 1).");
	puts("port1 val                 Set the output value of Port 1 by val(hex).");
}


char *rl_gets()
{
	static char *line_read = (char *)NULL;
    if(line_read)
    {
        free(line_read);
        line_read = (char *)NULL;
    }
    line_read = readline("\n> ");

    if(line_read && *line_read) add_history(line_read);
    return(line_read);
}
static char* command_generator(const char *text, int state)
{
    static int list_index, len;
    const char *name;

    if (!state)
    {
        list_index = 0;
        len = strlen (text);
    }

    while(name = commands[list_index])
    {
        ++list_index;
        if (strncmp (name, text, len) == 0)
            return strdup(name);
    }

    return ((char *)NULL);
}
char** command_completion (const char *text, int start, int end)
{
    char **matches = NULL;

    if (start == 0)
        matches = rl_completion_matches(text, command_generator);

    return (matches);
}
void initialize_readline ()
{
    rl_readline_name = "jdbshell";
    rl_attempted_completion_function = command_completion;
}


void trim(char*& s)
{
	char* p = s;
	for(; isspace(*p); ++p) ;
	s = p;
	for(p = s + strlen(s) - 1; p >= s; --p){
		if(!isspace(*p)) break;
	}
	p[1] = '\0';
}
bool inputCommand(char*& command)
{
	command = rl_gets();
	if(command){
		trim(command);
		return *command != '\0';
	}
	return false;
}

