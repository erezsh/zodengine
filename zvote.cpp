#include "zvote.h"
#include "common.h"

using namespace COMMON;

ZSDL_Surface ZVote::vote_in_progress_img;

ZVote::ZVote()
{
	ResetVote();
}

void ZVote::Init()
{
	vote_in_progress_img.LoadBaseImage("assets/other/menus/vote_in_progress.png");
	vote_in_progress_img.MakeAlphable();
}

void ZVote::DoRender(ZMap &the_map)
{
	if(in_progress && vote_in_progress_img.GetBaseSurface())
	{
		int shift_x, shift_y, view_w, view_h;

		int x, y;

		the_map.GetViewShiftFull(shift_x, shift_y, view_w, view_h);

		x = view_w - vote_in_progress_img.GetBaseSurface()->w;
		//y = view_h - vote_in_progress_img.GetBaseSurface()->h;

		x -= 4;
		y = 4;

		x += shift_x;
		y += shift_y;

		vote_in_progress_img.SetAlpha(200);
		description_img.SetAlpha(200);
		have_img.SetAlpha(200);
		needed_img.SetAlpha(200);
		for_img.SetAlpha(200);
		against_img.SetAlpha(200);

		the_map.RenderZSurface(&vote_in_progress_img, x, y);

		//the_map.RenderZSurface(&description_img, x+7, y+37, false, true);
		the_map.RenderZSurface(&description_img, x+57, y+36+5, false, true);

		the_map.RenderZSurface(&have_img, x+57, y+46+7, false, true);
		the_map.RenderZSurface(&needed_img, x+57, y+57+7, false, true);
		the_map.RenderZSurface(&for_img, x+22, y+57+7, false, true);
		the_map.RenderZSurface(&against_img, x+91, y+57+7, false, true);
	}
}

void ZVote::SetupImages(int have, int needed, int for_votes, int against_votes, string append_description)
{
	const int max_description_len = 112 - 8;
	char message[500];
	string description;

	description = vote_type_string[vote_type];

	if(append_description.length())
		description += ": " + append_description;

	if(vote_type >= 0 && vote_type < MAX_VOTE_TYPES)
		description_img.LoadBaseImage(ZFontEngine::GetFont(YELLOW_MENU_FONT).Render(description.c_str()));

	while(description.length() >= 3 && description_img.GetBaseSurface() && description_img.GetBaseSurface()->w > max_description_len)
	{
		description = description.substr(0, description.length()-3);
		description += "..";

		description_img.LoadBaseImage(ZFontEngine::GetFont(YELLOW_MENU_FONT).Render(description.c_str()));
	}

	sprintf(message,"%d", for_votes);
	for_img.LoadBaseImage(ZFontEngine::GetFont(YELLOW_MENU_FONT).Render(message));

	sprintf(message,"%d", against_votes);
	against_img.LoadBaseImage(ZFontEngine::GetFont(YELLOW_MENU_FONT).Render(message));

	sprintf(message,"%d", needed);
	needed_img.LoadBaseImage(ZFontEngine::GetFont(YELLOW_MENU_FONT).Render(message));

	sprintf(message,"%d", have);
	have_img.LoadBaseImage(ZFontEngine::GetFont(YELLOW_MENU_FONT).Render(message));

	description_img.MakeAlphable();
	for_img.MakeAlphable();
	against_img.MakeAlphable();
	needed_img.MakeAlphable();
	have_img.MakeAlphable();
}

bool ZVote::StartVote(int vote_type_, int value_)
{
	if(in_progress) return false;

	in_progress = true;
	vote_type = vote_type_;
	value = value_;
	start_time = current_time();
	end_time = start_time + MAX_VOTE_TIME;

	return true;
}

bool ZVote::TimeExpired()
{
	return current_time() >= end_time;
}

void ZVote::SetVoteType(int vote_type_)
{
	if(vote_type_ < 0) vote_type_ = -1;
	if(vote_type_ >= MAX_VOTE_TYPES) vote_type_ = -1;

	vote_type = vote_type_;
}

void ZVote::ResetVote()
{
	in_progress = false;
	vote_type = -1;
	value = -1;
}
