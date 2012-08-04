#ifndef _CLIENTSOCKET_H_
#define _CLIENTSOCKET_H_

#include <string>
#include "constants.h"
#include "socket_handler.h"
#include "event_handler.h"

#ifdef _WIN32 //if windows
#include <windows.h>		//win for Sleep(1000)
#include <direct.h>		//win
#else
#include <sys/types.h>		//lin
#include <sys/socket.h>		//lin
#include <netinet/in.h>		//lin
#include <netdb.h>		//lin
#include <sys/stat.h>		//lin
#include <sys/ioctl.h>		//lin
#include <arpa/inet.h>
#endif

using namespace std;

class ClientSocket
{
	public:
		ClientSocket();
		
		int Start(const char *address_ = "localhost", int port_ = 8000);
		void SetEventList(list<Event*> *event_list_);
		int Process();
		
		int SendMessage(int pack_id, const char *data, int size);
		int SendMessageAscii(int pack_id, const char *data);
		
		SocketHandler* GetHandler();
		void ClearConnection();
	private:
		int CreateSocket();
		int Connect();
		
		string address;
		int port;
		int s;
		
		SocketHandler* shandler;
		list<Event*> *event_list;
};

#endif
