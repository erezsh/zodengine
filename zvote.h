#ifndef _ZVOTE_H_
#define _ZVOTE_H_

#include <string>

#include "zmap.h"
#include "zfont_engine.h"

using namespace std;

#define MAX_VOTE_TIME 30

enum vote_type
{
	PAUSE_VOTE, RESUME_VOTE, CHANGE_MAP_VOTE, START_BOT, STOP_BOT,
	RESET_GAME_VOTE, RESHUFFLE_TEAMS_VOTE, CHANGE_GAME_SPEED,
	MAX_VOTE_TYPES
};

const string vote_type_string[MAX_VOTE_TYPES] = 
{
	"Pause Game", "Resume Game", "Change Map", "Start Bot", "Stop Bot",
	"Reset Game", "Reshuffle Teams", "Set Game Speed"
};

class ZVote
{
public:
	ZVote();

	static void Init();

	void DoRender(ZMap &the_map);
	void SetupImages(int have, int needed, int for_votes, int against_votes, string append_description);
	bool VoteInProgress() { return in_progress;}
	bool StartVote(int vote_type_, int value_ = -1);
	void ResetVote();

	bool TimeExpired();

	int GetVoteType() { return vote_type; }
	int GetVoteValue() { return value; }
	void SetVoteType(int vote_type);
	void SetVoteValue(int value_) { value = value_; }
	void SetVoteInProgress(bool in_progress_) { in_progress = in_progress_; }

private:
	static ZSDL_Surface vote_in_progress_img;

	ZSDL_Surface description_img;
	ZSDL_Surface for_img;
	ZSDL_Surface against_img;
	ZSDL_Surface needed_img;
	ZSDL_Surface have_img;

	bool in_progress;
	int vote_type;
	int value;
	double start_time;
	double end_time;
};

#endif
