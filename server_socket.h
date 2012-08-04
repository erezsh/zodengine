#ifndef _SERVERSOCKET_H_
#define _SERVERSOCKET_H_

#include <vector>

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

class ZServer;

class ServerSocket
{
	public:
		ServerSocket();
		int Start(int port = 8000);
		void SetEventList(list<Event*> *event_list_);
		void SetEventHandler(EventHandler<ZServer> *ehandler_) { ehandler = ehandler_; }
		int Process();
		
		int SendMessage(int player, int pack_id, const char *data, int size);
		int SendMessageAll(int pack_id, const char *data, int size);
		int SendMessageAscii(int player, int pack_id, const char *data);
		
		int PlayersConnected();
		
		SocketHandler* GetHandler(int id);
	private:
		int Bind();
		int Listen();
		int CreateSocket();
		int CheckConnects();
		int CheckData();
		
		int port;
		int bound;
		int started;
		
		int listen_socket;
		vector<SocketHandler*> client_socket;
		list<Event*> *event_list;
		EventHandler<ZServer> *ehandler;
};

#endif
