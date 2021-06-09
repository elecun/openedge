/**
 * @file host_tx.c
 * @brief
 * @author	Stephan Michaelsen
 * @date	21.01.2015
 * @version 0.0.1
 */


/*******************************************************************************
 *
 * Project  :
 * Module   :
 * Filename : host_tx.c
 * System   : CAN-Ethernet-DR
 * Compiler :
 * Switches :
 * Author   : Stephan Michaelsen
 * Rights   : PEAK-System Technik GmbH
 *            www.peak-system.com
 *
 *******************************************************************************
 * Implementation description
 *
 *
 *
 *******************************************************************************
 * History: (newer entries first!)
 *------------------------------------------------------------------------------
 * Date / Name         Vers.   changes made
 *------------------------------------------------------------------------------
 * 2015.01.21	0.0.1	initial version
 ******************************************************************************/
/*******************************************************************************
                                include files
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>		/* exit()... */
#include <stdint.h>		/* uint32_t... */
#include <string.h>		/* memset() strerror() */
#include <signal.h>		/* memset() strerror() */
#include <sys/socket.h>		/* sockaddr, mandatory */
#include <errno.h>
#include <unistd.h>
#include <libgen.h>

#include <arpa/inet.h>		/* inet_ntoa() */
#include <netinet/tcp.h>	/* SOL_TCP... */

#define DEFAULT_DATA_PORT 	50001
#define DEFAULT_PROTOCOL 	SOCK_DGRAM

#define VERSION_MAJOR 		0
#define VERSION_MINOR 		0
#define VERSION_SUBMINOR 	1

#define MAX_LOC_CMD_LEN 	255

struct ctrl_cmd {
	char *	cmd_text;
	int	(*cmd_handler)(char *p_cmd);
};

typedef  struct _CMDS
{
	uint8_t		cmd;
	char*		name;
} S_CMDS;

typedef union _VALUE{
	int8_t		val8s[8];
	uint8_t		val8u[8];
	int16_t		val16s[4];
	uint16_t 	val16u[4];
	int32_t		val32s[2];
	uint32_t	val32u[2];
	int64_t		val64s;
	uint64_t	val64u;
}U_VALUE;

typedef struct _CAN2IP {
	uint16_t 	sz;
	uint16_t 	type;
	uint64_t 	tag;
	uint32_t 	timestamp64_lo;
	uint32_t 	timestamp64_hi;
	uint8_t 	channel;
	uint8_t 	dlc;
	uint16_t 	flag;
	uint32_t 	id;
	U_VALUE		data;
}__attribute__((__packed__)) S_CAN2IP;

typedef struct _SW_VERSION_INFO{
	uint8_t architecture;
	uint8_t release;
	uint8_t patch;
	char sw_bdate[15+1];
	char sw_btime[15+1];
}S_SW_VERSION_INFO;


/*
 * Command handler declarations
 */
static int handle_loc_cmd_exit(char *p_cmd);
static int handle_loc_cmd_help(char *p_cmd);
static int handle_loc_cmd_connect(char *p_cmd);
static int handle_loc_cmd_send_frame(char *p_cmd);

/*
 * List of commands and their handler
 */
static struct ctrl_cmd loc_cmds[] = {
	{ "exit",				handle_loc_cmd_exit},
	{ "close",				handle_loc_cmd_exit},
	{ "quit",				handle_loc_cmd_exit},
	{ "?",					handle_loc_cmd_help},
	{ "help",				handle_loc_cmd_help},
	{ "connect",		handle_loc_cmd_connect},
	{ "send frame",		handle_loc_cmd_send_frame},
	{ NULL, NULL}
};

static int run = 1;
static int verbose = 0;
static int autoconnect = 0;
static unsigned int dataPort=DEFAULT_DATA_PORT;
static int protocol_type=DEFAULT_PROTOCOL;

static fd_set fds_rd, fds_wr, fds_exc;
static int so_fd_data_out=-1;

char target_ip[20]={'1','9','2','.',
					'1','6','8','.',
					'1','0','0','.',
					'1','1','0',
					'\0','\0','\0','\0','\0'};


static int sig_catched;
static int fdmax;

S_SW_VERSION_INFO sw_ver={VERSION_MAJOR, VERSION_MINOR, VERSION_SUBMINOR,
		__DATE__, __TIME__};

/*******************************************************************************
                          local function prototypes
 ******************************************************************************/
static void print_usage(char *name);
static void print_args(void);
static void printSWinfo(char *name, S_SW_VERSION_INFO *p_info);

static int init_application(void);
static void exit_application(void);
static void signal_handler(int s);
static int handle_loc_cmd(int fd);
int priv_openSocketOut(int port, char* p_ip, int type);
int priv_closeSocket(int sock);
static void init_outgoing_data_socket(void);



/*******************************************************************************
                               global functions
 ******************************************************************************/

//------------------------------------------------------------------------------
// int main(int argc, char **argv)
//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	int i, err;
	int opt = 0;

	/* get opts */
	while ((opt = getopt(argc, argv, "hatud:v:i:")) != -1) {
		switch (opt) {
		case 'd':		//data port
			dataPort = atoi(optarg);
			if (dataPort < 0)
				dataPort = DEFAULT_DATA_PORT;
			break;
		case 'v':		//verbose
			verbose = atoi(optarg);
			break;
		case 't':		//use TCP
			protocol_type = SOCK_STREAM;
			break;
		case 'u':		//use UDP
			protocol_type = SOCK_DGRAM;
			break;
		case 'a':		//connect immediately
			autoconnect = 1;
			break;
		case 'i':		//ip address
			strncpy(target_ip, optarg, 14);
			break;
		case 'h':		//help
		default:
			printSWinfo(basename(argv[0]), &sw_ver);
			print_usage(basename(argv[0]));
			exit(1);
			break;
		}
	} //while (getopt)

	if (verbose)
		print_args();

	/* do basic initializations */
	err = init_application();
	if (err)
		exit(2);

	/* now, waits on event */
	while (run) {
		int fd_sel;

		FD_ZERO(&fds_wr);
		FD_ZERO(&fds_rd);
		FD_ZERO(&fds_exc);

		if (so_fd_data_out > 0) {
			FD_SET(so_fd_data_out, &fds_rd);
			FD_SET(so_fd_data_out, &fds_exc);
		}

		FD_SET(STDIN_FILENO, &fds_rd);

		fdmax = (STDIN_FILENO > fdmax) ? STDIN_FILENO : fdmax;
		fdmax = (so_fd_data_out > fdmax) ? so_fd_data_out : fdmax;

		if (verbose > 3)
			printf("%s(): waiting for events (fd_max=%d)\n", __func__, fdmax);

		/* start waiting */
		fd_sel = select(fdmax + 1, &fds_rd, &fds_wr, &fds_exc, NULL);

		/* check if waiting failed */
		if (fd_sel < 0) {
			if (errno == EINTR) {
				if (sig_catched == SIGALRM)
					continue;
				if (verbose > 1)
					printf("Interrupted!\n");
			} else {
				fprintf(stderr, "select() call failed (errno=%d)\n", errno);
			}
			break;
		}

		/* otherwise check every bit set in descriptor set(s): */
		if (verbose > 2)
			printf("%s(): %u event(s) received\n", __func__, fd_sel);

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &fds_rd)) {
				fd_sel--;
				FD_CLR(i, &fds_rd);

				if (verbose > 2)
					printf("%s(): read event on %d\n", __func__, i);

				if (i == STDIN_FILENO) { //should be first
					err = handle_loc_cmd(STDIN_FILENO);
				} else if (i == so_fd_data_out) {
					char buff[256];
					err = recv(i, buff, 256, 0);

					if (err <= 0) {
						if (verbose)
							printf("%s(): error while receiving data (%s)\n",
									__func__, strerror(errno));
						priv_closeSocket(i);
						so_fd_data_out = -1;
					}
				}
			}

			if (FD_ISSET(i, &fds_wr)) {
				fd_sel--;
				FD_CLR(i, &fds_wr);

				if (verbose > 2)
					printf("%s(): write event on %d\n", __func__, i);
			}

			if (FD_ISSET(i, &fds_exc)) {
				fd_sel--;
				FD_CLR(i, &fds_exc);

				if (verbose > 2)
					printf("%s(): exception on %d\n", __func__, i);

				if (i == so_fd_data_out) {

				}
			}

			if (!fd_sel)
				break;
		}
	}

	exit_application();

	return 0;
}

/*******************************************************************************
 local functions
 ******************************************************************************/
//------------------------------------------------------------------------------
// static void print_usage(char* pFkt)
//------------------------------------------------------------------------------
static void print_usage(char *name) {
	fprintf(stdout,
			"\nUsage: %s [-i remote IP] [-d data port] [-v level] [-atu]\n",
			name);
	fprintf(stdout, "-i\t IP of target gateway (default: %s)\n", target_ip);
	fprintf(stdout, "-d\t target port (default: %u)\n", DEFAULT_DATA_PORT);
	fprintf(stdout, "-a\t automatically connect after start\n");
	fprintf(stdout, "-t\t use TCP protocol\n");
	fprintf(stdout, "-u\t use UDP protocol (default)\n");
	fprintf(stdout, "-v\t verbose output\n");
	fprintf(stdout, "-h\t print this text\n");
	fprintf(stderr, "\n");
	return;
}

//------------------------------------------------------------------------------
// static void print_args(char* pFkt)
//------------------------------------------------------------------------------
static void print_args(void) {
	fprintf(stdout, "target IP is: %s\n", target_ip);
	fprintf(stdout, "target port is: %u\n", DEFAULT_DATA_PORT);
	fprintf(stdout, "used protocol is: %s\n",
			protocol_type == SOCK_DGRAM ? "UDP" : "TCP");
	fprintf(stdout, "verbosity level is: %d\n",verbose);
	if (autoconnect)
		fprintf(stdout, "automatically connecting\n");
	return;
}

//------------------------------------------------------------------------------
//	printSWinfo(S_SW_VERSION_INFO *p_info)
//------------------------------------------------------------------------------
//! @return
//------------------------------------------------------------------------------
static void printSWinfo(char *name, S_SW_VERSION_INFO *p_info) {
	fprintf(stdout, "%s version ", name);
	fprintf(stdout, "%03d ", p_info->architecture);
	fprintf(stdout, "%03d ", p_info->release);
	fprintf(stdout, "%03d\n", p_info->patch);
	fprintf(stdout, "compiled at %s ", p_info->sw_bdate);
	fprintf(stdout, "%s\n", p_info->sw_btime);
	return;
}

//------------------------------------------------------------------------------
// static void init_application(void)
//------------------------------------------------------------------------------
//! @brief
//------------------------------------------------------------------------------
//! @param
//------------------------------------------------------------------------------
//! @return
//------------------------------------------------------------------------------
static int init_application(void) {
	FD_ZERO(&fds_rd);
	FD_ZERO(&fds_wr);

	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGALRM, signal_handler);
	sig_catched = 0;

	if (autoconnect)
		init_outgoing_data_socket();

	return 0;
}

//------------------------------------------------------------------------------
// static void signal_handler(int s)
//------------------------------------------------------------------------------
//! @brief	Signals handler
//------------------------------------------------------------------------------
//! @param	int s 	caught signal
//------------------------------------------------------------------------------
//! @return
//------------------------------------------------------------------------------
static void signal_handler(int s) {

	if (verbose > 2)
		printf("%s(signal=%d)\n", __func__, s);

	sig_catched = s;

	switch (s) {
	case SIGALRM:
		init_outgoing_data_socket();
		break;
	case SIGINT:
	case SIGTERM:
		run = 0;
		break;

	}
}

//------------------------------------------------------------------------------
// static void exit_application(void)
//------------------------------------------------------------------------------
//! @brief	Clean up
//------------------------------------------------------------------------------
static void exit_application(void) {
	/* close connected sockets */
	priv_closeSocket(so_fd_data_out);
	return;
}

//------------------------------------------------------------------------------
// static void init_outgoing_data_socket(void)
//------------------------------------------------------------------------------
//! @brief	open outgoing data socket to target gateway
//------------------------------------------------------------------------------
static void init_outgoing_data_socket(void) {

	/* open incoming data connection */
	if (so_fd_data_out < 0) {
		so_fd_data_out = priv_openSocketOut(dataPort, target_ip, protocol_type);
		if (so_fd_data_out < 0) {
			fprintf(stderr, "%s: open socket failed (%s)\n", __func__,
					strerror(errno));
			// retry connecting in 5 seconds from signal handler
			alarm(5);
		} else {
			fprintf(stderr, "%s: so_fd_data_in is %d\n", __func__,
					so_fd_data_out);
		}
	} else {
		if (verbose > 2)
			printf("%s(): so_fd_data_in already open (fd=%d)\n", __func__,
					so_fd_data_out);
	}

	return;
}


//------------------------------------------------------------------------------
// static int handle_loc_cmd(int fd)
//------------------------------------------------------------------------------
//! @brief	handles commands from given file descriptor
//------------------------------------------------------------------------------
//! @param	ind 	 file descriptor to read from
//------------------------------------------------------------------------------
//! @return	<0 in case of errors or any return value from called command handler
//------------------------------------------------------------------------------
static int handle_loc_cmd(int fd){
	char cmdbuf[MAX_LOC_CMD_LEN+1];
	int len=0, i=0;

	if (fd != STDIN_FILENO) /* not STDIN_FILENO */
		return -1;

	len = read(fd, &cmdbuf[len], MAX_LOC_CMD_LEN - len);

	if (verbose > 3) {
		printf("got %d chars:\n   ", len);
		for (i = 0; i < len; i++)
			printf("%0x2 ", cmdbuf[i]);
	}

	if (len == 0) {
		if (verbose > 1)
			printf("%s(): nothing to read on %d\n", __func__, fd);
		return -2;
	} else if (len == MAX_LOC_CMD_LEN) {
		if (verbose > 1) {
			printf("%s(): maybe more then %d chars to read on %d\n", __func__,
					MAX_LOC_CMD_LEN, fd);
			return -1;
		}
	}
	cmdbuf[len] = 0;

	if (verbose > 3)
		printf("%s(): cmd line=%s\n", __func__, cmdbuf);

	i = 0;
	while (loc_cmds[i].cmd_text) {
		if (strstr(cmdbuf, loc_cmds[i].cmd_text))
			break;
		else
			i++;
	}

	if (!loc_cmds[i].cmd_text) {
		if (verbose > 2)
			printf("%s(): no such command found: %s\n", __func__, cmdbuf);
		return -3;
	}

	if (!loc_cmds[i].cmd_handler) {
		if (verbose > 2)
			printf("%s(): no handler defined for command: %s\n", __func__,
					cmdbuf);
		return -4;
	}

	return loc_cmds[i].cmd_handler(&cmdbuf[strlen(loc_cmds[i].cmd_text)]);
}




//------------------------------------------------------------------------------
// static int handle_loc_cmd_exit(char *p_cmd)
//------------------------------------------------------------------------------
//! @brief	handler for data start command. sends start_data_transmit cmd to
//! 		server
//------------------------------------------------------------------------------
//! @return
//------------------------------------------------------------------------------
static int handle_loc_cmd_exit(char *p_cmd) {
	if (verbose > 1)
		printf("%s(): exiting\n", __func__);

	run = 0;

	return 0;
}

//------------------------------------------------------------------------------
// static int handle_loc_cmd_help(char *p_cmd)
//------------------------------------------------------------------------------
//! @brief	prints all defined local commands
//------------------------------------------------------------------------------
//! @return 0
//------------------------------------------------------------------------------
static int handle_loc_cmd_help(char *p_cmd){
	int i=0;

	printf("\n");
	while(loc_cmds[i].cmd_text)
		printf("%s\n", loc_cmds[i++].cmd_text);
	printf("\n");

	return 0;
}


//------------------------------------------------------------------------------
// static int handle_loc_cmd_connect(char *p_cmd)
//------------------------------------------------------------------------------
//! @brief	opens data socket to target gateway if not done yet
//------------------------------------------------------------------------------
//! @return 0
//------------------------------------------------------------------------------
static int handle_loc_cmd_connect(char *p_cmd) {

	/* open outgoing crtl connection to remote address */
	if (so_fd_data_out < 0) {
		init_outgoing_data_socket();
	} else {
		if (verbose > 1)
			printf("%s(): init_outgoing_data_socket already open (fd=%d)\n",
					__func__, so_fd_data_out);
	}

	return 0;
}


//------------------------------------------------------------------------------
// static int handle_loc_cmd_send_frame(char *p_cmd)
//------------------------------------------------------------------------------
//! @brief	send frame to target gateway
//------------------------------------------------------------------------------
//! @return
//------------------------------------------------------------------------------
static int handle_loc_cmd_send_frame(char *p_cmd){
	char *pS, *pE;
	unsigned int i, cmd_len;
	S_CAN2IP frame={
		.sz = htons(0x24),
		.type = htons(0x80),
		.tag = 0,
		.timestamp64_lo = htonl(0),
		.timestamp64_hi = htonl(0),
		.channel = 0,
		.dlc = 8,
		.flag = htons(0),
		.id = htonl(0),
		.data.val64u = 0
	};

	if (verbose > 2)
		printf("%s(): send message to target\n", __func__);

	if(so_fd_data_out<0){
		printf("%s(): data socket not valid(%d)\n", __func__, so_fd_data_out);
	}
	

	//Parse message (should be like ID#[Data0][Data1]... (e.g. 0x100#0011)
	frame.id=(uint32_t)strtoul(&p_cmd[1], &pE, 16);
	printf("id : %d\n", frame.id);
	if((frame.id & 0x1FFFFFFF)>0x7FF)	// if CAN ID > 0x7FF always set EFF bit
		frame.id |= 1<<31;
	frame.id= htonl(frame.id);

	pS = strchr(pE, '#');
	if(!pS){
		printf("ERROR while parsing command (no '#' found)\n");
		return -1;
	}
	pS++;

	cmd_len = strnlen(p_cmd, 140);

	// read 8 Data Byte
	for(i=0; i<8 ;i++){
		char buf[3]={0,0,0};

		if(pS>=(p_cmd+cmd_len))
			break;

		buf[0]=*pS++;
		if(buf[0]<'0' || buf[0]>'9')
			break;

		buf[1]=*pS++;
		if(buf[1]<'0' || buf[1]>'9')
			break;

		frame.data.val8u[i]=(uint8_t)strtoul(buf, NULL, 16);
	}

	frame.dlc = (uint8_t)i;

	send(so_fd_data_out,(uint8_t*)&frame,0x24,0);
	return 0;
}




//------------------------------------------------------------------------------
//	int priv_openSocketOut(int port, int type, unsigned int queue_len)
//------------------------------------------------------------------------------
//! @brief	opens socket to target gateway
//------------------------------------------------------------------------------
//! @param
//------------------------------------------------------------------------------
//! @return	<0	in case of errors
//!				fd number of socket elsewise
//------------------------------------------------------------------------------
int priv_openSocketOut(int port, char* p_ip, int type){
	struct sockaddr_in sockname;
	int optval = 1;
	int sockID=0, retVal=0;

	if(verbose)
		fprintf(stdout, "connecting to target\n");

	//New Socket Instance
	if (type == SOCK_STREAM){
		if ((sockID = socket(PF_INET, SOCK_STREAM, 0)) < 0){
			perror("unable to open socket");
			return(-2);
		}
		retVal = setsockopt(sockID, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(int));
		if(retVal < 0){
			perror("error using setsockopt");
			return(-2);
		}
	}
	else if (type == SOCK_DGRAM){
		if ((sockID = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
			perror("unable to open socket");
			return(-2);
		}
	}
	else{
		return(-3);
	}

	retVal = setsockopt(sockID, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	if(retVal < 0){
		perror("error using setsockopt (SOL_SOCKET, SO_REUSEADDR)");
		return(-2);
	}

	retVal = setsockopt(sockID, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(int));
	if(retVal < 0){
		perror("error using setsockopt (SOL_SOCKET, SO_KEEPALIVE)");
		return(-2);
	}

	retVal = setsockopt(sockID, SOL_IP, IP_RECVERR, &optval, sizeof(int));
	if(retVal < 0){
		perror("error using setsockopt (SOL_IP, IP_RECVERR)");
		return(-2);
	}



	//Config Socket
	memset((char *) &sockname, 0, sizeof(struct sockaddr_in));
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(port);
	if (strncmp(p_ip, "*", 2)) {
		sockname.sin_addr.s_addr = inet_addr(p_ip);
		if (sockname.sin_addr.s_addr == INADDR_NONE ){
			if(verbose)
				fprintf(stdout, "invalid IP-address\n");
			perror("invalid IP-address");
			return(-2);
		}

	}
	else{
		sockname.sin_addr.s_addr = htonl(INADDR_ANY); /* wildcard */
	}


	if (connect(sockID,  (struct sockaddr *) &sockname, sizeof(struct sockaddr_in)) == 0){
		if(verbose)
			fprintf(stdout, "SocketID: %d\n\n", sockID);
		return sockID;
	}
	else{
		fprintf(stderr, "connect failed (%s)\n", strerror(errno));
		close(sockID);
	}

	if(verbose)
		fprintf(stdout, "connect faild!\n");

	return -1;
}//priv_openSocketOut()


//------------------------------------------------------------------------------
//	int priv_closeSocket(int sock)
//------------------------------------------------------------------------------
//! @brief	shutdown and close socket
//------------------------------------------------------------------------------
//! @param	sock	fd number of socket to close
//------------------------------------------------------------------------------
//! @return	0
//------------------------------------------------------------------------------
int priv_closeSocket(int sock){
	shutdown(sock, SHUT_RDWR);
	close(sock);
	return 0;
}








