#ifndef _ZCLIENT_H_
#define _ZCLIENT_H_

#include "zcore.h"
#include "client_socket.h"

class ZClient : public ZCore
{
	public:
		ZClient();
		
		void SetRemoteAddress(string ipaddress);
		void SetPlayerName(string player_name_);
		virtual void SetPlayerTeam(team_type player_team);
		virtual void SetDesiredTeam(team_type player_team);
	protected:
		int ProcessMapDownload(char *data, int size);
		ZObject* ProcessNewObject(char *data, int size);
		void ProcessZoneInfo(char *data, int size);
		ZObject* ProcessObjectTeam(char *data, int size);
		ZObject* ProcessObjectAttackObject(char *data, int size);
		ZObject* ProcessDeleteObject(char *data, int size);
		ZObject* ProcessObjectHealthTeam(char *data, int size);
		ZObject* ProcessFireMissile(char *data, int size);
		ZObject* ProcessObjectLoc(char *data, int size);
		ZObject* ProcessBuildingState(char *data, int size);
		ZObject* ProcessBuildingQueueList(char *data, int size);
		ZObject* ProcessBuildingCannonList(char *data, int size);
		ZObject* ProcessObjectGroupInfo(char *data, int size);
		ZObject* ProcessObjectLidState(char *data, int size);
		ZObject* ProcessSetGrenadeState(char *data, int size);
		bool ProcessZSettings(char *data, int size);
		bool ProcessAddLPlayer(char *data, int size);
		bool ProcessDeleteLPlayer(char *data, int size);
		bool ProcessSetLPlayerName(char *data, int size);
		bool ProcessSetLPlayerTeam(char *data, int size);
		bool ProcessSetLPlayerMode(char *data, int size);
		bool ProcessSetLPlayerIgnored(char *data, int size);
		bool ProcessSetLPlayerLogInfo(char *data, int size);
		bool ProcessSetLPlayerVoteInfo(char *data, int size);
		bool ProcessUpdateGamePaused(char *data, int size);
		bool ProcessUpdateGameSpeed(char *data, int size);
		bool ProcessVoteInfo(char *data, int size);
		bool ProcessPlayerID(char *data, int size);
		bool ProcessSelectableMapList(char *data, int size);
		bool ProcessSetTeam(char *data, int size);
		void ProcessConnect();
		virtual void ProcessDisconnect();
		virtual void ProcessEndGame();
		virtual void ProcessResetGame();
		void RequestObjectList();
		void RequestZoneList();
		void SendPlayerInfo();
		void SendPlayerTeam(int new_team);
		void SendPlayerMode();
		void RequestPlayerList();
		void SendBotBypassData();
		p_info &OurPInfo();
		virtual void DeleteObjectCleanUp(ZObject *obj);
		
		
		string remote_address;
		string player_name;
		team_type our_team;
		team_type desired_team;
		player_mode our_mode;
		int p_id;
		
		char *map_data;
		int map_data_size;
		ClientSocket client_socket;
};

#endif
