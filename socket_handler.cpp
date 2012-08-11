#ifndef _WIN32
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "socket_handler.h"
#include "common.h"

using namespace COMMON;

SocketHandler::SocketHandler()
{
	connected = 0;
	fp_ptr = 0;
}

SocketHandler::SocketHandler(int s, struct sockaddr_in s_in)
{
	Init(s, s_in);
}

int SocketHandler::Init(int s_, struct sockaddr_in s_in_)
{
	u_long on_mode = 1;

	s = s_;
	s_in = s_in_;

	buf_size = 0;

	fp_ptr = 0;

	connected = 1;

	strcpy(ipaddress,(char*)inet_ntoa(s_in.sin_addr));

#ifdef _WIN32
	ioctlsocket(s, FIONBIO, &on_mode);
#else
	ioctl(s, FIONBIO, &on_mode);
#endif

	printf("socket connected:%s\n", ipaddress);

	return 1;
}

int SocketHandler::Connected()
{
	return connected;
}

int SocketHandler::Disconnect()
{
	if(connected)
	{
		//first close
#ifdef _WIN32
		closesocket(s);
#else
		close(s);
#endif

		//set stuff...
		connected = 0;
		printf("socket disconnected: %s\n", ipaddress);

		//clear out
		ipaddress[0] = 0;
	}

	return 1;
}

int SocketHandler::recv_good(int rcv_amt)
{
#ifdef _WIN32
	int terr = WSAGetLastError();
	if(terr == 10054 || (!terr && !rcv_amt))
#else
	if(!rcv_amt)
#endif
	{
		Disconnect();
		return 0;
	}
	else if(rcv_amt == -1)
	{
		//simply no packet yet
		return 0;
	}

	return 1;
}

int SocketHandler::PacketAvailable()
{
	int rcv_amt;
	char temp_buf[MAX_BUF_SIZE];
	int pack_size;

	rcv_amt = recv(s, temp_buf, sizeof(int), MSG_PEEK);

	if(!recv_good(rcv_amt)) return 0;

	//get pack size
	if(rcv_amt != sizeof(int))
		return 0;
	else
		pack_size = *(int*)temp_buf + 8;

	//bad packet so just disconnect?
	if(pack_size < 8 || pack_size > MAX_BUF_SIZE)
	{
		Disconnect();
		return 0;
	}

	//check if there is the right amount of data on the buffer
	//there has to be a faster way...
	rcv_amt = recv(s, temp_buf, pack_size, MSG_PEEK);

	//if(!recv_good(rcv_amt)) return 0;

	return rcv_amt == pack_size;
}

int SocketHandler::GetPacket(char **message, int *size, int *pack_id)
{
	int rcv_amt;
	//char temp_buf[MAX_BUF_SIZE];
	char *temp_buf = dp_temp_buf;
	int pack_size;

	rcv_amt = recv(s, temp_buf, sizeof(int), MSG_PEEK);

	if(!recv_good(rcv_amt)) return 0;

	//get pack size
	if(rcv_amt != sizeof(int))
		return 0;
	else
		pack_size = *(int*)temp_buf + 8;

	//bad packet so just disconnect?
	if(pack_size < 8 || pack_size > MAX_BUF_SIZE)
	{
		Disconnect();
		return 0;
	}

	rcv_amt = recv(s, temp_buf, pack_size, 0);

	if(!recv_good(rcv_amt)) return 0;

	if(rcv_amt != pack_size)
	{
		printf("SocketHandler::GetPacket:rcv_amt != pack_size\n");
		return 0;
	}

	*size = ((int*)temp_buf)[0];
	*pack_id = ((int*)temp_buf)[1];
	*message = temp_buf + 8;

	if(*size < 0) return 0;

	return 1;
}

int SocketHandler::DoRecv()
{
	int rcv_amt;
	char temp_buf[MAX_BUF_SIZE];

	rcv_amt = recv(s, temp_buf, MAX_BUF_SIZE, 0);

	if(!recv_good(rcv_amt)) return 0;

//#ifdef _WIN32
//	int terr = WSAGetLastError();
//	if(terr == 10054 || (!terr && !rcv_amt))
//#else
//		if(!rcv_amt)
//#endif
//	{
//		Disconnect();
//		return 0;
//	}
//	else if(rcv_amt == -1)
//	{
//		//simply no packet yet
//		return 0;
//	}

	//process that monkey yo!
	//should we clear?
	if(rcv_amt + buf_size > MAX_DATA_STORED)
	{
		printf("WARNING: recv tossed because of oversize\n");
		buf_size = 0;
		return 0;
	}

	//fill it
	memcpy(buf + buf_size, temp_buf, rcv_amt);
	buf_size += rcv_amt;

	return 1;
}

int SocketHandler::DoProcess(char **message, int *size, int *pack_id)
{
	//char temp_buf[MAX_DATA_STORED];
	char *temp_buf = dp_temp_buf;
	int packet_size;

	//dont even have the indentifier?
	if(buf_size < sizeof(int) + sizeof(int)) return 0;
	*size = ((int*)buf)[0];
	*pack_id = ((int*)buf)[1];
	packet_size = *size + 8;

// 	print_dump(buf, 20, "rcv ");
// 	printf("rcv buf_size:%d packet_size:%d\n", buf_size, packet_size);

	if(*size < 0)
	{
		//whoops garbage
		printf("invalid packet size:%d\n", *size);
		buf_size = 0;
		return 0;
	}

	//all the packet there?
	if(packet_size > buf_size) return 0;

	memcpy(temp_buf, buf + 8, *size);
	*message = temp_buf;

	//push back stuff at the end?
	if(packet_size < buf_size) memcpy(buf, buf + packet_size, buf_size - packet_size);

	//shrink buf_size
	buf_size -= packet_size;

	return 1;
}

int SocketHandler::DoFastProcess(char **message, int *size, int *pack_id)
{
	char *fp_buf = buf + fp_ptr;
	int fp_buf_size = buf_size - fp_ptr;
	int packet_size;

	//dont even have the indentifier?
	if(fp_buf_size < sizeof(int) + sizeof(int)) return 0;
	*size = ((int*)fp_buf)[0];
	*pack_id = ((int*)fp_buf)[1];
	*message = fp_buf + 8;
	packet_size = *size + 8;

	if(*size < 0)
	{
		//whoops garbage
		printf("invalid packet size:%d\n", *size);
		buf_size = 0;
		return 0;
	}

	//all the packet there?
	if(packet_size > fp_buf_size) return 0;

	//move fp_ptr forward
	fp_ptr += packet_size;

	//return good
	return 1;
}

void SocketHandler::ResetFastProcess()
{
	if(!fp_ptr) return;

	char *fp_buf = buf + fp_ptr;
	int fp_buf_size = buf_size - fp_ptr;

	//if some remaining then push it back to the front
	if(fp_buf_size) memcpy(buf, fp_buf, fp_buf_size);

	//set real size to the fp size
	buf_size = fp_buf_size;

	//reset ptr
	fp_ptr = 0;
}

int SocketHandler::SendMessage(int pack_id, const char *data, int size)
{
	char temp_buf[MAX_DATA_STORED];
	int error_no;

	if(!this) return 0;

	//if(!pause_for_send())
	//{
	//	Disconnect();
	//	return 0;
	//}

	//pack
	((int*)temp_buf)[0] = size;
	((int*)temp_buf)[1] = pack_id;
	if(size) memcpy(temp_buf + sizeof(int) + sizeof(int), data, size);

	//send
#ifdef MSG_NOSIGNAL
	error_no = send(s, temp_buf, size + sizeof(int) + sizeof(int), MSG_NOSIGNAL);
#else
	error_no = send(s, temp_buf, size + sizeof(int) + sizeof(int), 0);
#endif

// 	print_dump(temp_buf, 20, "send");

	//error in send?
	if(error_no <= 0)
	{
		Disconnect();
		return 0;
	}

	return 1;
}

int SocketHandler::SendMessageAscii(int pack_id, const char *data)
{
	return SendMessage(pack_id, data, strlen(data) + 1);
}

int SocketHandler::socket_good_to_send(bool &kill_me)
{
	fd_set write_flags;
	fd_set error_flags;
	int the_stat;
	struct timeval waitd;

	waitd.tv_sec = 0;
	waitd.tv_usec = 0;

	FD_ZERO(&write_flags);
	FD_ZERO(&error_flags);
	FD_SET(s, &write_flags);
	FD_SET(s, &error_flags);

	the_stat = select(s + 1, (fd_set*)0, &write_flags, &error_flags, &waitd);

	if(the_stat < 0)
	{
#ifdef _WIN32
		printf("select errno:%d\n",errno);
#else
		printf("select errno:%d-%s\n",errno, strerror(errno));
#endif
		switch(errno)
		{
		case 9: //bad file descriptor
			kill_me = true;
			break;
		}
		return 0;
	}

	if(FD_ISSET(s, &error_flags))
	{
		printf("socket_good_to_send:error flag set\n");
		return 0;
	}

	if(FD_ISSET(s, &write_flags))
	{
		FD_CLR(s, &write_flags);
		return 1;
	}
	else
		return 0;
}

int SocketHandler::pause_for_send()
{
	const double max_wait = 0.5;
	double start_time;
	bool kill_me = false;

	if(socket_good_to_send(kill_me)) return 1;

	start_time = current_time();

	do
	{
		uni_pause(10);

		printf("pause_for_send:had to begin pausing\n");

		if(socket_good_to_send(kill_me)) return 1;
		if(kill_me) return 0;
	} while(current_time() - start_time < max_wait);

	return 0;
}

char *SocketHandler::GetMAC(char *buf)
{
	//clear
	memset(buf, 0, 6);
#ifdef _WIN32
#ifndef WEBOS_PORT
	IP_ADAPTER_INFO AdapterInfo[16];

	DWORD dwBufLen = sizeof(AdapterInfo);

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	if(dwStatus == ERROR_SUCCESS)
	{
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

		do
		{
			for(int i=0; i<6;i++)
				if(pAdapterInfo->Address[i])
				{
					memcpy(buf, pAdapterInfo->Address, 6);
					return buf;
				}

			pAdapterInfo = pAdapterInfo->Next;
		}
		while(pAdapterInfo);
	}
#else
// Simulate MAC
buf[0]=buf[1]=buf[2]=buf[3]=buf[4]=buf[5]=11;
#endif
#else
	int skfd;
	struct ifreq sIfReq;
	struct if_nameindex *pIfList, *pIfList_tofree;

	//create socket
	skfd = socket(AF_INET,SOCK_DGRAM,0);
	if(skfd<0)
	{
		printf( "SocketHandler::GetMAC: create socket failed\n");
		return buf;
	}

	pIfList = if_nameindex();
	pIfList_tofree = pIfList;

	for ( pIfList; *(char *)pIfList != 0; pIfList++ )
	{
		strncpy( sIfReq.ifr_name, pIfList->if_name, IF_NAMESIZE );

		if ( ioctl(skfd, SIOCGIFHWADDR, &sIfReq) != 0 )
			printf( "SocketHandler::GetMAC: ioctl failed\n");
		else
		{
			for(int i=0; i<6;i++)
				if(sIfReq.ifr_ifru.ifru_hwaddr.sa_data[i])
				{
					memcpy(buf, sIfReq.ifr_ifru.ifru_hwaddr.sa_data, 6);

					if(pIfList_tofree) if_freenameindex(pIfList_tofree);
					close(skfd);

					return buf;
				}
		}
	}

	if(pIfList_tofree) if_freenameindex(pIfList_tofree);
	close(skfd);
#endif

	printf("MAC ADDRESS: ");
	for(int i=0;i<6;i++) printf("%02x-", (unsigned char)buf[i]);
	printf("\n");

	return buf;
}

