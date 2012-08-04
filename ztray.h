#ifndef _ZTRAY_H_
#define _ZTRAY_H_

#include "zclient.h"

class ZTray : public ZClient
{
	public:
		ZTray();
		
		void Setup();
		void Run();

		void SetUpdateCallBack(void (*update_func_)(ZTray*));
		vector<p_info> &GetPlayerInfoList();
	private:
		void SetupEHandler();
		void ProcessDisconnect();

		void UpdateDisplay();

		void (*update_func)(ZTray*);
		
		EventHandler<ZTray> ehandler;
		
		static void nothing_event(ZTray *p, char *data, int size, int dummy);
		static void test_event(ZTray *p, char *data, int size, int dummy);
		static void clear_player_list_event(ZTray *p, char *data, int size, int dummy);
		static void add_player_event(ZTray *p, char *data, int size, int dummy);
		static void delete_player_event(ZTray *p, char *data, int size, int dummy);
		static void set_player_name_event(ZTray *p, char *data, int size, int dummy);
		static void set_player_team_event(ZTray *p, char *data, int size, int dummy);
		static void set_player_mode_event(ZTray *p, char *data, int size, int dummy);
		static void set_player_ignored_event(ZTray *p, char *data, int size, int dummy);
		static void set_player_voteinfo_event(ZTray *p, char *data, int size, int dummy);
		static void set_player_loginfo_event(ZTray *p, char *data, int size, int dummy);

		static void connect_event(ZTray *p, char *data, int size, int dummy);
		static void disconnect_event(ZTray *p, char *data, int size, int dummy);
};

#endif
