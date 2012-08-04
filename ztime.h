#ifndef _ZTIME_H_
#define _ZTIME_H_

class ZTime
{
public:
	ZTime();

	void UpdateTime();
	void Pause();
	void Resume();
	inline bool IsPaused() { return paused; }
	void SetGameSpeed(double new_speed);
	inline double GameSpeed() { return game_speed; }

	double ztime;
	//double lost_time;
	//double pause_time;
	bool paused;
	double game_speed;
	double last_change_front_time;
	double last_change_back_time;
};

#endif
