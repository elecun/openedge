/**
 * @file    pcan.parse.hpp
 * @brief   PEAK PCAN interface data parser
 * @author  Byunghun Hwang<bh.hwang@iae.re.kr>
 */


#define MAX_LOC_CMD_LEN			255
#define CLIENT_QUEUE_LEN 	  	3
#define BUFF_SZ					2048
#define VERSION_MAJOR			0
#define VERSION_MINOR			0
#define VERSION_SUBMINOR		1

#define array_sizeof(X) (sizeof(X) / sizeof(X[0]))

struct ctrl_cmd {
	char *	cmd_text;
	int	(*cmd_handler)(char *p_cmd);
};

typedef  struct _CMDS
{
	unsigned char	cmd;
	char*		    name;
} S_CMDS;

typedef union _VALUE{
		signed char Value8s[8];
		unsigned char	Value8u[8];
		signed short	Value16s[4];
		unsigned short 	Value16u[4];
		signed long	Value32s[2];
		unsigned long Value32u[2];
		unsigned long long	Value64u;
		signed long long    Value64s;
		float	ValueFlt[2];
		double	ValueDbl;
}U_VALUE;

typedef struct _SW_VERSION_INFO{
	unsigned char   architecture;
	unsigned char   release;
	unsigned char patch;
	char sw_bdate[15+1];
	char sw_btime[15+1];
}S_SW_VERSION_INFO;

typedef  struct _LAN_MSG
{
    unsigned short	size;
    unsigned short	type;
    unsigned long long	tag;
    unsigned long long	timestamp;
    unsigned char	channel;
    unsigned char	dlc;
    unsigned short	flag;
    unsigned long	id;
    U_VALUE	value;
	_LAN_MSG(){ memset(this, 0, sizeof(_LAN_MSG)); }
} S_LAN_MSG;


S_SW_VERSION_INFO sw_ver={VERSION_MAJOR, VERSION_MINOR, VERSION_SUBMINOR, __DATE__, __TIME__};


