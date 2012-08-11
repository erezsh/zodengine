//#include <stdio.h>

#ifdef _WIN32

//xgetopt stuff
//this is only needed for some compilers
#include "xgetopt.h"
int optind, opterr;
TCHAR *optarg;
//end xgetopt

#else
#include <unistd.h>
#endif


#include "main.h"
#include "common.h"
#include "constants.h"
#include "zsdl.h"
#include "zplayer.h"
#include "zbot.h"
#include "zserver.h"
#include "ztray.h"

#ifdef WEBOS_PORT
#include <sdl\sdl.h>
#include <pdl.h>
#endif

void display_help(char *shell_command);
void display_version();
int run_server_thread(void *nothing);
int run_bot_thread(void *nothing);
void run_player_thread();
void run_tray_app();

input_options starting_conditions;

char bot_bypass_data[MAX_BOT_BYPASS_SIZE];
int bot_bypass_size;

#undef main
int main(int argc, char **argv)
{
#ifdef WEBOS_PORT
#ifndef WIN32 // win32 debug?
	PDL_Init(0);
#endif
#endif
	SDL_Thread *server_thread;

	printf("Welcome to the Zod Engine\n");

	if(argc<=1) starting_conditions.setdefaults();

	//read in the arguments
	starting_conditions.getoptions(argc, argv);

	//make sure there is nothing conflicting,
	//like we are trying to make a dedicated server that is supposed to connect to another server
	starting_conditions.checkoptions();

	//init this for the bots
	ZCore::CreateRandomBotBypassData(bot_bypass_data, bot_bypass_size);

	//now see what we have
	if(starting_conditions.read_display_version) display_version();
	if(starting_conditions.read_display_help) display_help(argv[0]);

	//now what do we really run?
	else if(starting_conditions.read_run_tray)
	{
		run_tray_app();
	}
	else if(starting_conditions.read_is_dedicated)
	{
		//run only a server
		//server_thread = SDL_CreateThread(run_server_thread, NULL);
		run_server_thread(NULL);
	}
	else if(starting_conditions.read_connect_address)
	{
		//connect to a server
		run_player_thread();
	}
	else
	{
		//run a server, then connect to it

		// !!!
		server_thread = SDL_CreateThread(run_server_thread, NULL);
		run_player_thread();
	}
#ifdef WEBOS_PORT
#ifndef WIN32 //Win32 debug?
	PDL_Quit();
#endif
#endif
	return 1;
}

int run_server_thread(void *nothing)
{
	int i;
	vector<SDL_Thread*> bot_thread;
	ZServer zserver;

	for(i=0;i<MAX_TEAM_TYPES;i++)
		if(starting_conditions.read_start_bot[i])
		{
			zserver.InitBot(i);

			//int *team = new int;
			//printf("start bot for team %s\n", team_type_string[i].c_str());

			//*team = i;
			//bot_thread.push_back(SDL_CreateThread(run_bot_thread, (void*)team));
		}

	if(starting_conditions.read_map_name)
		zserver.SetMapName(starting_conditions.map_name);
	else if(starting_conditions.read_map_list)
		zserver.SetMapList(starting_conditions.map_list);

	if(starting_conditions.read_settings)
		zserver.SetSettingsFilename(starting_conditions.settings_filename);
	if(starting_conditions.read_p_settings)
		zserver.SetPerpetualSettingsFilename(starting_conditions.p_settings_filename);

	zserver.SetBotBypassData(bot_bypass_data, bot_bypass_size);

	zserver.Setup();
	zserver.Run();

	return 1;
}

int run_bot_thread(void *nothing)
{
	int *team;
	ZBot zbot;

	team = (int*)nothing;

	zbot.SetDesiredTeam((team_type)*team);
	if(starting_conditions.read_connect_address)
		zbot.SetRemoteAddress(starting_conditions.connect_address);

	zbot.SetBotBypassData(bot_bypass_data, bot_bypass_size);

	zbot.Setup();
	zbot.Run();

	return 1;
}

void run_player_thread()
{
	ZPlayer zplayer;

	zplayer.DisableCursor(starting_conditions.read_disable_zcursor);
	zplayer.SetSoundsOff(starting_conditions.read_sound_off);
	zplayer.SetMusicOff(starting_conditions.read_music_off);
	zplayer.SetWindowed(starting_conditions.read_is_windowed);
	zplayer.SetUseOpenGL(!starting_conditions.read_opengl_off);
	if(starting_conditions.read_player_team)
		zplayer.SetDesiredTeam((team_type)starting_conditions.team);
	if(starting_conditions.read_player_name)
		zplayer.SetPlayerName(starting_conditions.player_name);
	if(starting_conditions.read_loginname)
		zplayer.SetLoginName(starting_conditions.loginname);
	if(starting_conditions.read_password)
		zplayer.SetLoginPassword(starting_conditions.password);
	if(starting_conditions.read_connect_address)
		zplayer.SetRemoteAddress(starting_conditions.connect_address);
	if(starting_conditions.read_resolution)
		zplayer.SetDimensions(starting_conditions.resolution_width, starting_conditions.resolution_height);

	zplayer.Setup();
	zplayer.Run();
}

void run_tray_app()
{
#ifndef WEBOS_PORT
	ZTray ztray;

	if(starting_conditions.read_connect_address)
		ztray.SetRemoteAddress(starting_conditions.connect_address);

	ztray.Setup();
	ztray.Run();
#endif
}

void display_help(char *shell_command)
{
	printf("\n==================================================================\n");
	printf("Command list...\n");
	printf("-c ip_address        - game host address\n");
	printf("-m filename          - map to be used\n");
	printf("-l filename          - map list to be used\n");
	printf("-z filename          - settings file to be used\n");
	printf("-e filename          - main server settings file to be used\n");
	printf("-n player_name       - your player name\n");
	printf("-g login_name        - your login name\n");
	printf("-i login_password    - your login password\n");
	printf("-t team              - your team\n");
	printf("-b team              - connect a bot player\n");
	printf("-w                   - run game in windowed mode\n");
	printf("-r resolution        - resolution to run the game at\n");
	printf("-d                   - run a dedicated server\n");
	printf("-h                   - display command help\n");
	printf("-s                   - no sound\n");
	printf("-u                   - no music\n");
	printf("-o                   - no opengl\n");
	printf("-k                   - use faster and blander cursor\n");
	printf("-v                   - display version and credits\n");
	printf("-a                   - run shell based tray app\n");

	printf("\nExample usage...\n");
	printf("%s -c localhost -r 800x600 -w\n", shell_command);
	printf("%s -m level1.map -b 1 -p 1\n", shell_command);
	printf("==================================================================\n");
}

void display_version()
{
#ifndef DISABLE_OPENGL
	printf("\nZod: A Zed Engine, Version Alpha\n");
#else
	printf("\nZod: A Zed Engine, Version Alpha (OpenGL Disabled)\n");
#endif
	printf("By Michael Bok\n");
	printf("Please visit http://zod.sourceforge.net/ and http://zzone.lewe.com/\n");
}

void input_options::setdefaults()
{
#ifdef WEBOS_PORT
	//-c localhost -n zlover -t red -r 1024x768 -w -o
	printf("no arguments set, using defaults of '-c localhost -n zlover -t red -r 1024x768 -w -o'\n");

	read_connect_address = false;//true;
	connect_address = "localhost";

	read_player_name = true;
	player_name = "zlover";

	read_player_team = true;
	player_team_str = "red";
	team = RED_TEAM;

    read_map_list = true;
	map_list = "map_list.txt";

	resolution = "1024x768";
	resolution_width = 1024;
	resolution_height = 768;
	read_resolution = false;

	read_is_windowed = false;
	read_start_bot[2]=true;
// TODO (iev0107m#1#): load last setting from file (last completed level) ?

	read_sound_off = false;
	read_music_off = false;
#else
	//-c hestia.nighsoft.net -n zlover -t red -r 800x600 -w -o
	printf("no arguments set, using defaults of '-c hestia.nighsoft.net -n zlover -t red -r 800x600 -w -o'\n");
// -l map_list.txt -n test -t red -r 1024x768 -w -o -b blue

	read_connect_address = true;
	connect_address = "hestia.nighsoft.net";

	read_player_name = true;
	player_name = "zlover";

	read_player_team = true;
	player_team_str = "red";
	team = RED_TEAM;

	resolution = "800x600";
	resolution_width = 800;
	resolution_height = 600;
	read_resolution = true;

	read_is_windowed = true;
#endif
	read_opengl_off = true;
}

int input_options::checkoptions()
{
	//nothing?
	if(!read_connect_address &&
		!read_map_name &&
		!read_is_windowed &&
		!read_resolution &&
		!read_is_dedicated &&
		!read_display_help &&
		!read_display_version &&
		!read_map_list)
	{
		read_display_help = true;
	}

	if(read_connect_address && read_is_dedicated)
	{
		printf("cannot be a dedicated server and have a connect address\n");
		return 0;
	}

	if(read_is_dedicated && read_resolution)
	{
		printf("cannot be a dedicated server and have a screen resolution\n");
		return 0;
	}

	if(read_is_dedicated && read_is_windowed)
	{
		printf("cannot be a dedicated server and be in windowed mode\n");
		return 0;
	}

	if(read_connect_address && (read_map_name || read_map_list))
	{
		printf("cannot have a connect address and set the map\n");
		return 0;
	}

	if(read_map_name && read_map_list)
	{
		printf("cannot a read map list and a specific map file\n");
		return 0;
	}

	if(read_run_tray && !read_connect_address)
	{
		printf("need a connect address to run the tray app\n");
		return 0;
	}

	return 1;
}

int input_options::getoptions(int argc, char **argv)
{
	int c;
	int i;
	int temp_int;
#ifdef _WIN32
	extern char *optarg;
//	extern int optind;
#endif

	while ((c = getopt(argc, argv, "c:m:l:n:t:b:z:e:g:i:wr:dhvksuoa")) != -1)
	{
		switch(c)
		{
			case 'c':
				if(!optarg) return 0;
				read_connect_address = true;
				connect_address = optarg;
				break;
			case 'm':
				if(!optarg) return 0;
				read_map_name = true;
				map_name = optarg;
				break;

			case 'l':
				if(!optarg) return 0;
				read_map_list = true;
				map_list = optarg;
				break;

			case 'n':
				if(!optarg) return 0;
				read_player_name = true;
				player_name = optarg;
				break;

			case 'z':
				if(!optarg) return 0;
				read_settings = true;
				settings_filename = optarg;
				break;

			case 'e':
				if(!optarg) return 0;
				read_p_settings = true;
				p_settings_filename = optarg;
				break;

			case 'g':
				if(!optarg) return 0;
				read_loginname = true;
				loginname = optarg;
				break;

			case 'i':
				if(!optarg) return 0;
				read_password = true;
				password = optarg;
				break;

			case 't':
				if(!optarg) return 0;
				read_player_team = true;
				player_team_str = optarg;

				for(i=0;i<MAX_TEAM_TYPES;i++)
					if(team_type_string[i] == player_team_str)
						break;

				if(i!=MAX_TEAM_TYPES)
					team = i;
				else
					printf("could not find the team '%s', perhaps try lowercase?\n", player_team_str.c_str());
				break;

			case 'b':
				if(!optarg) return 0;
				//read_bot_count = true;
				//bot_count = atoi(optarg);
				if(!optarg) return 0;

				for(i=0;i<MAX_TEAM_TYPES;i++)
					if(team_type_string[i] == optarg)
						break;

				if(i!=MAX_TEAM_TYPES)
					read_start_bot[i] = true;
				else
					printf("could not find the team '%s', perhaps try lowercase?\n", optarg);
				break;

			case 'r':
				if(!optarg) return 0;
				resolution = optarg;

				temp_int = resolution.find('x');

				if(temp_int != (int)string::npos)
				{
					resolution_width = atoi(resolution.substr(0, temp_int).c_str());
					resolution_height = atoi(resolution.substr(temp_int+1, 10).c_str());
					read_resolution = true;
				}
				else
					read_resolution = false;

				break;

			case 'w':
				read_is_windowed = true;
				break;

			case 'd':
				read_is_dedicated = true;
				break;

			case 'h':
				read_display_help = true;
				break;

			case 'v':
				read_display_version = true;
				break;

			case 's':
				read_sound_off = true;
				break;

			case 'u':
				read_music_off = true;
				break;

			case 'k':
				read_disable_zcursor = true;
				break;

			case 'o':
				read_opengl_off = true;
				break;

			case 'a':
				read_run_tray = true;
				break;

			case '?':
				printf("unrecognized option -%c\n", c);
				read_display_help = true;
				return 0;
				break;


		}

	}

	return 1;
}
