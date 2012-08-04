#ifndef _SOCKETHANDLER_H_
#define _SOCKETHANDLER_H_

#ifdef _WIN32 //if windows
#include <windows.h>		//win for Sleep(1000)
#include <direct.h>		//win
#include <Iphlpapi.h>
#else
#include <sys/types.h>		//lin
#include <sys/socket.h>		//lin
#include <netinet/in.h>		//lin
#include <netdb.h>		//lin
#include <sys/stat.h>		//lin
#include <sys/ioctl.h>		//lin
#include <arpa/inet.h>
#include <net/if.h>
#endif

#define MAX_DATA_STORED 400000
#define MAX_BUF_SIZE 20000

class SocketHandler
{
	public:
		SocketHandler();
		SocketHandler(int s, struct sockaddr_in s_in);
		int Init(int s, struct sockaddr_in s_in);
		int Connected();
		int Disconnect();
		int DoRecv();
		int DoProcess(char **message, int *size, int *pack_id);
		int DoFastProcess(char **message, int *size, int *pack_id);
		void ResetFastProcess();
		int PacketAvailable();
		int GetPacket(char **message, int *size, int *pack_id);
		int SendMessage(int pack_id, const char *data, int size);
		int SendMessageAscii(int pack_id, const char *data);
		static char *GetMAC(char *buf);
	private:
		int recv_good(int rcv_amt);
		int pause_for_send();
		int socket_good_to_send(bool &kill_me);
		
		int s;
		struct sockaddr_in s_in;
		char ipaddress[50];
		int connected;
		int fp_ptr;
		
		char buf[MAX_DATA_STORED];
		int buf_size;

		//for the do process func
		char dp_temp_buf[MAX_DATA_STORED];
};

#endif
