/**
 * @file host_rx.c
 * @brief
 * @author	Stephan Michaelsen
 * @date	03.12.2014
 * @version 0.0.1
 */


/*******************************************************************************
 *
 * Project  :
 * Module   :
 * Filename : host_rx.c
 * System   : CAN-Ethernet-DR
 * Compiler :
 * Switches :
 * Author   : Stephan Michaelsen
 * Rights   : PEAK-System Technik GmbH
 *                www.peak-system.com
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
 * 2014.12.03	0.0.1	initial version
 ******************************************************************************/
/*******************************************************************************
                                include files
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>			/* exit()... */
#include <stdint.h>			/* uint32_t... */
#include <string.h>			/* memset() strerror() */
#include <signal.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/time.h>

#include <arpa/inet.h>		/* inet_ntoa() */
#include <netinet/tcp.h>	/* SOL_TCP... */


#define VERSION_MAJOR			0
#define VERSION_MINOR			0
#define VERSION_SUBMINOR		1


#define MAX_LOC_CMD_LEN			255
#define CLIENT_QUEUE_LEN 	  	3
#define BUFF_SZ					2048

#define DEFAULT_DATA_PORT 	50000
#define DEFAULT_PROTOCOL 	SOCK_DGRAM

#define array_sizeof(X) (sizeof(X) / sizeof(X[0]))

typedef char char_t;
typedef unsigned int uint_t;
typedef int int_t;
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned long u32_t;
typedef unsigned long long u64_t;
typedef signed char s8_t;
typedef signed short s16_t;
typedef signed long s32_t;
typedef signed long long s64_t;

struct ctrl_cmd {
	char *	cmd_text;
	int	(*cmd_handler)(char *p_cmd);
};

typedef  struct _CMDS
{
	u8_t		cmd;
	char*		name;
} S_CMDS;

typedef union _VALUE{
		s8_t	Value8s[8];
		u8_t	Value8u[8];
		s16_t	Value16s[4];
		u16_t 	Value16u[4];
		s32_t	Value32s[2];
		u32_t	Value32u[2];
		u64_t	Value64u;
		s64_t	Value64s;
		float	ValueFlt[2];
		double	ValueDbl;
}U_VALUE;

typedef struct _SW_VERSION_INFO{
	u8_t architecture;
	u8_t release;
	u8_t patch;
	char sw_bdate[15+1];
	char sw_btime[15+1];
}S_SW_VERSION_INFO;

typedef  struct _LAN_MSG
{
    u16_t	size;
    u16_t	type;
    u64_t	tag;
    u64_t	timestamp;
    u8_t	channel;
    u8_t	dlc;
    u16_t	flag;
    u32_t	id;
    U_VALUE	value;
} S_LAN_MSG;

static int handle_loc_cmd_exit(char *p_cmd);
static int handle_loc_cmd_help(char *p_cmd);

/*
 * List of control plane commands and their resp. handler
 */
static struct ctrl_cmd loc_cmds[] = {
	{ "exit",				handle_loc_cmd_exit},
	{ "close",				handle_loc_cmd_exit},
	{ "quit",				handle_loc_cmd_exit},
	{ "?",					handle_loc_cmd_help},
	{ "help",				handle_loc_cmd_help},
	{ NULL, NULL}
};

/*
 * Global parameters with their default values
 */
static int run = 1;
static int verbose = 3;
static unsigned int dataPort=DEFAULT_DATA_PORT;
static int protocol_type=DEFAULT_PROTOCOL;

/*
 * Globals
 */
static fd_set fds_rd, fds_wr;

static int so_fd_data_in=-1;

static int sig_catched;
static int fdmax;

S_SW_VERSION_INFO sw_ver={VERSION_MAJOR, VERSION_MINOR, VERSION_SUBMINOR, __DATE__, __TIME__};

/*******************************************************************************
                          local function prototypes
 ******************************************************************************/
static void print_usage(char *prg);
static int init_application(void);
static void exit_application(void);
static void signal_handler(int s);
static int handle_loc_cmd(int fd);
static int handle_rem_data(int fd);
static int parseDataMsg(unsigned char * p_buff, int len, S_LAN_MSG *p_msg);
static int printData(S_LAN_MSG *p_msg);
int priv_openSocketIn(int port, int type, unsigned int queue_len);
int priv_closeSocket(int sock);
static int printSWinfo(S_SW_VERSION_INFO *p_info);
static void init_incoming_data_socket(void);

/*******************************************************************************
                               global functions
 ******************************************************************************/

//------------------------------------------------------------------------------
// int main(int argc, char **argv)
//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	int i, err;
	int opt=0;


	/******************
	 * d data port
	 * v verbose on/off
	 * */
	while ((opt = getopt(argc, argv, "td:v:")) != -1) {
		switch (opt) {
			case 'd':		//data port
				dataPort = atoi(optarg);
				if(dataPort<0)
					dataPort = DEFAULT_DATA_PORT;
				break;
			case 'v':		//verbose
				verbose = atoi(optarg);
				break;
			case 't':		// use tcp
				protocol_type = SOCK_STREAM;
				break;
			default:
				printSWinfo(&sw_ver);
				print_usage(basename(argv[0]));
				exit(1);
				break;
		}
	} //while (getopt)


	/* Do init first */
	err = init_application();
	if (err)
		exit(2);

	/* now, waits on event */
	while (run) {
		int fd_sel;

		if(so_fd_data_in>0){
			FD_SET(so_fd_data_in, &fds_rd);
		}
		FD_SET(STDIN_FILENO, &fds_rd);

		fdmax = (STDIN_FILENO>fdmax)?STDIN_FILENO:fdmax;
		fdmax = (so_fd_data_in>fdmax)?so_fd_data_in:fdmax;

		if (verbose > 3)
			printf("%s(): waiting for events (fd_max=%d)\n", __func__, fdmax);

		/* start waiting */
		fd_sel = select(fdmax+1, &fds_rd, &fds_wr, NULL, NULL);

		/* check if waiting failed */
		if (fd_sel < 0) {
			if (errno == EINTR) {
				if (sig_catched == SIGALRM)
					continue;
				printf("Interrupted!\n");
			} else {
				fprintf(stderr, "select() call failed (errno=%d)\n", errno);
			}
			break;
		}

		/* check if timeout */
		if (!fd_sel) {
			if (verbose)
				printf("timeout on waiting for anything... loop again\n");
			continue;
		}

		/* otherwise check every bit set in descriptor set(s): */
		if (verbose > 3)
			printf("%s(): %u event(s) received\n", __func__, fd_sel);

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &fds_rd)) {
				fd_sel--;
				FD_CLR(i, &fds_rd);

				if (verbose > 3)
					printf("%s(): read event on %d\n", __func__, i);

				if (i==STDIN_FILENO){ //should be first
					err = handle_loc_cmd(STDIN_FILENO);
				} else if (i==so_fd_data_in){
					handle_rem_data(i);
				} else{

				}
			}

			if (FD_ISSET(i, &fds_wr)) {

				fd_sel--;
				FD_CLR(i, &fds_wr);

				if (verbose > 3)
					printf("%s(): write event on %d\n",
								__func__, i);
			}

			if (!fd_sel)
				break;
		}
	}

	printf("exiting\n");
	exit_application();

	return 0;
}

/*******************************************************************************
                                local functions
 ******************************************************************************/
//------------------------------------------------------------------------------
// static void print_usage(char* pFkt)
//------------------------------------------------------------------------------
static void print_usage(char *prg)
{
	fprintf(stdout, "\nUsage: %s [-d data_port] [-t] [-v] \n", prg);
	fprintf(stderr, "d   set port for incoming data - default is %d\n", DEFAULT_DATA_PORT);
	fprintf(stderr, "t   use TCP protocol - default is UDP\n");
	fprintf(stderr, "v   set verbosity level\n");
	return;
}


//------------------------------------------------------------------------------
// static void init_application(void)
//------------------------------------------------------------------------------
//! @brief
//------------------------------------------------------------------------------
static int init_application(void)
{
	FD_ZERO(&fds_rd);
	FD_ZERO(&fds_wr);

	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGALRM, signal_handler);
	sig_catched = 0;

	init_incoming_data_socket();

	return 0;
}


//------------------------------------------------------------------------------
// static void signal_handler(int s)
//------------------------------------------------------------------------------
//! @brief	Signals handler
//------------------------------------------------------------------------------
static void signal_handler(int s)
{
	if (verbose > 2)
		printf("%s(signal=%d)\n", __func__, s);

	sig_catched = s;

	switch (s) {
	case SIGALRM:
		init_incoming_data_socket();
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
//! @brief	Do all what should be done when application properly exits.
//------------------------------------------------------------------------------
static void exit_application(void)
{

	/* close connected sockets */
	priv_closeSocket(so_fd_data_in);

	return;
}

//------------------------------------------------------------------------------
// static void init_incoming_data_socket(void)
//------------------------------------------------------------------------------
//! @brief	clean up
//------------------------------------------------------------------------------
//! @param
//------------------------------------------------------------------------------
static void init_incoming_data_socket(void)
{

	/* open incoming data connection */
	if (so_fd_data_in<0){
		so_fd_data_in = priv_openSocketIn(dataPort, protocol_type, CLIENT_QUEUE_LEN);
		if(so_fd_data_in<0){
			fprintf(stderr, "%s: open socket failed (%s)\n", __func__, strerror(errno));
			alarm(5);
		}
		else{
			fprintf(stderr, "%s: so_fd_data_in is %d\n", __func__, so_fd_data_in);
		}
	}else{
		if(verbose > 2)
			printf("%s(): so_fd_data_in already open (fd=%d)\n", __func__, so_fd_data_in);
	}

	return;
}


//------------------------------------------------------------------------------
// static int handle_loc_cmd(int fd)
//------------------------------------------------------------------------------
//! @brief	handles commands from stdin
//------------------------------------------------------------------------------
//! @param	ind fd
//------------------------------------------------------------------------------
static int handle_loc_cmd(int fd){
	char cmdbuf[MAX_LOC_CMD_LEN+1];
	int len=0, i=0;

	if(fd!=STDIN_FILENO) /* not STDIN_FILENO */
		return -1;

	len = read(fd, &cmdbuf[len], MAX_LOC_CMD_LEN-len);

	if (verbose>3){
		printf("got %d chars:\n   ", len);
		for(i=0; i<len; i++)
			printf("%0x2 ", cmdbuf[i]);
	}

	if (len == 0) {
		if (verbose > 1)
			printf("%s(): nothing to read on %d\n", __func__, fd);
		return -2;
	} else if(len == MAX_LOC_CMD_LEN){
		if (verbose > 1)
			printf("%s(): maybe more then %d chars to read on %d\n", __func__, MAX_LOC_CMD_LEN, fd);
		//@ ToDo return here?
		//return;
	}
	cmdbuf[len]=0;

	if (verbose > 3)
		printf("%s(): cmd line=%s\n", __func__, cmdbuf);

	i=0;
	while(loc_cmds[i].cmd_text){
		if(strstr(cmdbuf, loc_cmds[i].cmd_text))
			break;
		else
			i++;
	}

	if(!loc_cmds[i].cmd_text){
		if (verbose > 2)
				printf("%s(): no such command found: %s\n", __func__, cmdbuf);
		return -3;
	}

	if(!loc_cmds[i].cmd_handler){
		if (verbose > 2)
				printf("%s(): no handler defined for command: %s\n", __func__, cmdbuf);
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
static int handle_loc_cmd_exit(char *p_cmd){
	if (verbose > 1)
			printf("%s(): exiting\n", __func__);

	run=0;

	return 0;
}

//------------------------------------------------------------------------------
// static int handle_loc_cmd_help(char *p_cmd)
//------------------------------------------------------------------------------
//! @brief	prints all local commands
//------------------------------------------------------------------------------
static int handle_loc_cmd_help(char *p_cmd){
	int i=0;

	printf("\n");

	while(loc_cmds[i].cmd_text)
		printf("%s\n", loc_cmds[i++].cmd_text);

	return 0;
}



//------------------------------------------------------------------------------
// static int handle_rem_data(int fd)
//------------------------------------------------------------------------------
//! @brief	heandler for data from remote peer
//------------------------------------------------------------------------------
//! @param	fd of remote connection
//------------------------------------------------------------------------------
static int handle_rem_data(int fd){
	static unsigned char buff[BUFF_SZ];
	unsigned int flgs=0;
	unsigned int cur_pos=0;
	static unsigned int remaining_data=0;
	int res=0, i=0, rec_len;
	S_LAN_MSG rec_msg;

	if (verbose > 3)
			printf("%s()\n", __func__);

	memset(&buff[remaining_data], 0,BUFF_SZ-remaining_data);
	rec_len = recv(fd , &buff[remaining_data], BUFF_SZ-remaining_data, flgs );
	rec_len += remaining_data;
	remaining_data=0;

	if (rec_len < 0){
		if(verbose)
			printf("%s(): error while receiving data (%s)\n",
					__func__, strerror(errno));
	} else if(rec_len ==0){
		if(verbose)
			printf("socket %d: remote disconnection\n", fd);
		priv_closeSocket(fd);
		return -2;
	} else{
		if(verbose > 3)
			printf("%s(): data received (%d)\n",	__func__, rec_len);
		if(verbose > 4){
			for(i=0;i<rec_len;i++)
				printf("%02X ", buff[i]);
			printf("\n");
		}

		while(rec_len>(cur_pos+1)){
			// parse Data until all CAN frames have been extracted
			res=parseDataMsg(&buff[cur_pos], rec_len-cur_pos, &rec_msg);
			if(res == -2){
				unsigned char tmp_buff[BUFF_SZ];
				remaining_data = rec_len-cur_pos;
				memcpy(tmp_buff, &buff[cur_pos], remaining_data);
				memset(buff, 0, BUFF_SZ);
				memcpy(buff, tmp_buff, remaining_data);
				rec_len=0;
				continue;
			}
			else if(res<0){
				// error during parsing of the message -> quit loop
				rec_len=0;
				continue;
			}
			cur_pos+=res;

			if(verbose>1)
				printData(&rec_msg);
		}
	}

	return 0;
}


//------------------------------------------------------------------------------
//	static int parseDataMsg(unsigned char * p_buff, int len)
//------------------------------------------------------------------------------
//! @brief parses data message from gateway
//------------------------------------------------------------------------------
static int parseDataMsg(unsigned char * p_buff, int len, S_LAN_MSG *p_msg){

	/* check message length */
	p_msg->size = ntohs(*((u16_t*)&p_buff[0]));
	if(p_msg->size != 36){
		if (verbose)
			printf("%s: wrong message length\n", __func__);
		return -1;
	}
	if(p_msg->size > len){
		if (verbose>2)
			printf("%s: wrong buffer length. message maybe incomplete!\n", __func__);
		return -2;
	}

	/* check message type */
	p_msg->type = ntohs(*((u16_t*)&p_buff[2]));
	if(p_msg->type != 0x80){
		if (verbose>1)
			printf("%s: wrong message type\n", __func__);
		return -1;
	}

	/* get message tag */
	p_msg->tag =  ntohl(*((u32_t*)&p_buff[4]));
	p_msg->tag <<= 32;
	p_msg->tag += ntohl(*((u32_t*)&p_buff[8]));

	/* get message time stamp */
	p_msg->timestamp += ntohl(*((u32_t*)&p_buff[16]));
	p_msg->timestamp <<= 32;
	p_msg->timestamp =  ntohl(*((u32_t*)&p_buff[12]));

	/* get message channel */
	p_msg->channel = p_buff[20];

	/* get message dlc */
	p_msg->dlc = p_buff[21];
	if(p_msg->dlc > 8){
		if (verbose)
			printf("%s: wrong dlc\n", __func__);
		return -1;
	}

	/* get message dlc */
	p_msg->flag = ntohs(*((u16_t*)&p_buff[22]));

	/* get message time stamp */
	p_msg->id =  ntohl(*((u32_t*)&p_buff[24]));

	/* get value and set unused bytes to 0 */
	memset(&(p_msg->value.Value8u[0]), 0, 8);
	memcpy(&(p_msg->value.Value8u[0]), &p_buff[28], p_msg->dlc);

	return p_msg->size;
}



//------------------------------------------------------------------------------
//	static int printData(S_LAN_MSG *p_msg)
//------------------------------------------------------------------------------
//! @brief
//------------------------------------------------------------------------------
static int printData(S_LAN_MSG *p_msg){
	unsigned int i;

	printf("%012llu ", p_msg->timestamp);
	printf("[%01u] ", p_msg->dlc);
	if(p_msg->id & 0xE0000000)
		printf("0x%08X", (unsigned int) p_msg->id);
	else
		printf("0x%03X", (unsigned int) p_msg->id);

	if(p_msg->dlc)
		printf(" : ");

	for(i=0; i<p_msg->dlc; i++)
		printf("%02X ", p_msg->value.Value8u[i]);

	printf("\n");

	return 0;
}


//------------------------------------------------------------------------------
//	int priv_openSocketIn(int port, int type, unsigned int queue_len)
//------------------------------------------------------------------------------
//! @brief open socket for incoming packets from GW
//------------------------------------------------------------------------------
int priv_openSocketIn(int port, int type, unsigned int queue_len){
	struct sockaddr_in sockname;
	s32_t optval = 1;
	s32_t sockID=0, retVal=0;

	//New Socket Instance
	if (type == SOCK_STREAM){
		//New Socket Instance
		if ((sockID = socket(PF_INET, SOCK_STREAM, 0)) < 0){
			perror("unable to open socket");
			exit(-2);			   				/* Rückkehr bei Fehler */
		}
	}
	else if (type == SOCK_DGRAM){
		//New Socket Instance
		if ((sockID = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
			perror("unable to open socket");
			exit(-2);			   				/* Rückkehr bei Fehler */
		}
	}
	else{
		exit(-3);
	}

	/* Set socket option SO_REUSEADDR
	 * Indicates that the rules used in validating addresses supplied in a bind(2)
	 *  call should allow reuse of local addresses. For PF_INET sockets this means
	 *  that a socket may bind, except when there is an active listening socket bound
	 *  to the address. When the listening socket is bound to INADDR_ANY with a specific
	 *  port then it is not possible to bind to this port for any local address.
	 */
	retVal = setsockopt(sockID, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if(retVal < 0){
		perror("error using setsockopt (SOL_SOCKET, SO_REUSEADDR)");
		exit(-2);			   				/* Rückkehr bei Fehler */
	}

	// Enable keepalive packets
	retVal = setsockopt(sockID, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
	if(retVal < 0){
		perror("error using setsockopt (SOL_SOCKET, SO_KEEPALIVE)");
		exit(-2);			   				/* Rückkehr bei Fehler */
	}

	retVal = setsockopt(sockID, SOL_IP, IP_RECVERR, &optval, sizeof(int));
	if(retVal < 0){
		perror("error using setsockopt (SOL_IP, IP_RECVERR)");
		exit(-2);			   				/* Rückkehr bei Fehler */
	}

	//Config Socket
	memset((char *) &sockname, 0, sizeof(struct sockaddr_in));	/* unbenutzte Bytes loeschen! */
	sockname.sin_family = AF_INET;	   							/* IP-Adresse aufsetzen */
	sockname.sin_port = htons(port);							/* Portnummer des Services */
	sockname.sin_addr.s_addr = htonl(INADDR_ANY);				/* wildcard */


    if (bind(sockID, (const struct sockaddr*) &sockname, sizeof(sockname)) < 0){	    /* an Adresse binden */
		perror("unable to bind socket");
		exit(-2);			   				/* Rückkehr bei Fehler */
    }

    if (type == SOCK_STREAM){
    	struct sockaddr peer_addr;
    	socklen_t addr_len;

		if (listen(sockID, queue_len) < 0){
			perror("unable to listen to clients");
			exit(-2);			   				/* Rückkehr bei Fehler */
		}

		sockID = accept(sockID, &peer_addr, &addr_len);
    }

	return sockID;
}//priv_openSocketIn()



//------------------------------------------------------------------------------
//	int priv_closeSocket(int sock)
//------------------------------------------------------------------------------
//! @brief closes given socket
//------------------------------------------------------------------------------
int priv_closeSocket(int sock){
	shutdown(sock, SHUT_RDWR);
	close(sock);
	return 0;
}


//------------------------------------------------------------------------------
//	printSWinfo(S_SW_VERSION_INFO *p_info)
//------------------------------------------------------------------------------
//! @return
//------------------------------------------------------------------------------
static int printSWinfo(S_SW_VERSION_INFO *p_info)
{
	#define DC stdout
	int wr=0;
	int tmp=0;

	wr = fprintf(DC, "%03d ", p_info->architecture);
	if(wr<=0)
		return 0;

	tmp = fprintf(DC, "%03d ", p_info->release);
	if(tmp<=0)
		return 0;
	wr+=tmp;

	tmp = fprintf(DC, "%03d", p_info->patch);
	if(tmp<=0)
		return 0;
	wr+=tmp;

	tmp = fprintf(DC, "%s ", p_info->sw_bdate);
	if(tmp<=0)
		return 0;
	wr+=tmp;

	tmp = fprintf(DC, "%s\n", p_info->sw_btime);
	if(tmp<=0)
		return 0;
	wr+=tmp;

	return wr;
}







