#include "ecraneconco.h"

const double ECraneConco::travel_time_width = 0.8;

bool ECraneConco::finished_init = false;
ZSDL_Surface ECraneConco::conco[MAX_TEAM_TYPES][8];
ZSDL_Surface ECraneConco::cone_no_shadow[MAX_TEAM_TYPES];
ZSDL_Surface ECraneConco::cone[MAX_TEAM_TYPES];
ZSDL_Surface ECraneConco::robot_jackhammer[MAX_TEAM_TYPES][2];
ZSDL_Surface ECraneConco::robot_paper[MAX_TEAM_TYPES][2];
ZSDL_Surface ECraneConco::robot_point[MAX_TEAM_TYPES][3];
ZSDL_Surface ECraneConco::robot_travel_right[MAX_TEAM_TYPES];
ZSDL_Surface ECraneConco::robot_travel_left[MAX_TEAM_TYPES];
ZSDL_Surface ECraneConco::robot_travel_updown[MAX_TEAM_TYPES];
ZSDL_Surface ECraneConco::sign_flip[MAX_TEAM_TYPES][8];
ZSDL_Surface ECraneConco::sign[MAX_TEAM_TYPES];
int ECraneConco::conco_w = 0, ECraneConco::conco_h = 0;
int ECraneConco::cone_w = 0, ECraneConco::cone_h = 0;
int ECraneConco::sign_w = 0, ECraneConco::sign_h = 0;

ECraneConco::ECraneConco(ZTime *ztime_, int team_, int c_x, int c_y, int b_x, int b_y, int b_w, int b_h, bool is_bridge) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	int i;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	our_team = team_;

	if(our_team < 0 || our_team >= MAX_TEAM_TYPES)
	{
		killme = true;
		return;
	}

	int c_cx, c_cy;
	int b_cx, b_cy;

	//set the center of the building and crane
	c_cx = c_x + 16;
	c_cy = c_y + 16;
	b_cx = b_x + (b_w >> 1);
	b_cy = b_y + (b_h >> 1);

	//defaults
	{
		travel_to = true;
		travel_back = false;
		conco_i = 7;
		travel_time_start = the_time;
		travel_time_end = travel_time_start + travel_time_width;
		next_jackbot_time = 0;
		jackbot_i = 0;
		pbot_pointing = false;
		next_pbot_time = 0;
		pbot_i = 0;
		sign_i = 0;

		for(i=0;i<ECC_RENDER_ITEM_TYPES;i++)
		{
			switch(i)
			{
			case ECC_CONCO:
				render_item[i].Init(i, c_cx, c_cy, conco_w, conco_h);
				break;
			case ECC_CONE0:
			case ECC_CONE1:
				render_item[i].Init(i, c_cx, c_cy, cone_w, cone_h);
				break;
			case ECC_JACK:
			case ECC_PAPER:
				render_item[i].Init(i, c_cx, c_cy, 16, 16);
				break;
			case ECC_SIGN:
				render_item[i].Init(i, c_cx, c_cy, sign_w, sign_h);
				break;
			default:
				render_item[i].Init(i, c_cx, c_cy);
				break;
			}

			render_item_list.push_back(render_item + i);
		}
	}

	//setup destinations
	{
		const int conco_dist_from_entrace = 12;
		const int cone_dist_from_entrace = 6;
		const int cone_dist_from_center = 18;
		const int sign_dist_from_conco = 6;

		SetBotInitCords(is_bridge, c_cx, c_cy, b_x, b_y, b_w, b_h, render_item[ECC_JACK].dest_x, render_item[ECC_JACK].dest_y);
		SetBotInitCords(is_bridge, c_cx, c_cy, b_x, b_y, b_w, b_h, render_item[ECC_PAPER].dest_x, render_item[ECC_PAPER].dest_y);

		if(!is_bridge)
		{
			//buildings are easy
			render_item[ECC_CONCO].dest_x = b_cx - (conco_w >> 1);
			render_item[ECC_CONCO].dest_y = b_y + b_h + conco_dist_from_entrace;

			render_item[ECC_CONE0].dest_x = b_cx - (cone_w + cone_dist_from_center);
			render_item[ECC_CONE1].dest_x = b_cx + cone_dist_from_center;
			render_item[ECC_CONE0].dest_y = render_item[ECC_CONE1].dest_y = b_y + b_h + cone_dist_from_entrace;

			render_item[ECC_SIGN].dest_x = (render_item[ECC_CONCO].dest_x + (conco_w >> 1)) - (sign_w >> 1);
			render_item[ECC_SIGN].dest_y = render_item[ECC_CONCO].dest_y - (sign_h + 1);
		}
		else
		{
			//bah..
			if(b_w > b_h) //horz bridge
			{
				if(c_cx > b_cx) // from the right
				{
					render_item[ECC_CONCO].dest_x = b_x + b_w + conco_dist_from_entrace;
					render_item[ECC_CONCO].dest_y = b_cy - (conco_h >> 1);

					render_item[ECC_CONE0].dest_x = render_item[ECC_CONE1].dest_x = b_x + b_w + cone_dist_from_entrace;
					render_item[ECC_CONE0].dest_y = b_cy - (cone_h + cone_dist_from_center);
					render_item[ECC_CONE1].dest_y = b_cy + cone_dist_from_center;

					render_item[ECC_SIGN].dest_x = render_item[ECC_CONCO].dest_x - (sign_dist_from_conco + sign_w);
					render_item[ECC_SIGN].dest_y = (render_item[ECC_CONCO].dest_y + (conco_h >> 1)) - (sign_h >> 1);
				}
				else // from the left
				{
					render_item[ECC_CONCO].dest_x = b_x - (conco_dist_from_entrace + conco_w);
					render_item[ECC_CONCO].dest_y = b_cy - (conco_h >> 1);

					render_item[ECC_CONE0].dest_x = render_item[ECC_CONE1].dest_x = b_x - (cone_dist_from_entrace + cone_w);
					render_item[ECC_CONE0].dest_y = b_cy - (cone_h + cone_dist_from_center);
					render_item[ECC_CONE1].dest_y = b_cy + cone_dist_from_center;

					render_item[ECC_SIGN].dest_x = render_item[ECC_CONCO].dest_x + conco_w + sign_dist_from_conco;					
					render_item[ECC_SIGN].dest_y = (render_item[ECC_CONCO].dest_y + (conco_h >> 1)) - (sign_h >> 1);
				}
			}
			else //vert bridge
			{
				if(c_cy < b_cy) //from the top
				{
					render_item[ECC_CONCO].dest_x = b_cx - (conco_w >> 1);
					render_item[ECC_CONCO].dest_y = b_y - (conco_dist_from_entrace + conco_h);

					render_item[ECC_CONE0].dest_x = b_cx - (cone_w + cone_dist_from_center);
					render_item[ECC_CONE1].dest_x = b_cx + cone_dist_from_center;
					render_item[ECC_CONE0].dest_y = render_item[ECC_CONE1].dest_y = b_y - (cone_dist_from_entrace + cone_h);
				}
				else //from the bottom
				{
					render_item[ECC_CONCO].dest_x = b_cx - (conco_w >> 1);
					render_item[ECC_CONCO].dest_y = b_y + b_h + conco_dist_from_entrace;

					render_item[ECC_CONE0].dest_x = b_cx - (cone_w + cone_dist_from_center);
					render_item[ECC_CONE1].dest_x = b_cx + cone_dist_from_center;
					render_item[ECC_CONE0].dest_y = render_item[ECC_CONE1].dest_y = b_y + b_h + cone_dist_from_entrace;
				}

				render_item[ECC_SIGN].dest_x = (render_item[ECC_CONCO].dest_x + (conco_w >> 1)) - (sign_w >> 1);
				render_item[ECC_SIGN].dest_y = render_item[ECC_CONCO].dest_y - (sign_h + 1);
			}
		}

		//some internal stuff needs done now that the start and dest are set
		for(i=0;i<ECC_RENDER_ITEM_TYPES;i++)
			render_item[i].SetTravelDistances();
	}
}

void ECraneConco::SetBotInitCords(bool is_bridge, int c_cx, int c_cy, int b_x, int b_y, int b_w, int b_h, int &dest_x, int &dest_y)
{
	const int dist_from_entrance = 16;
	const int dist_from_entrance_box = 32;
	int b_cx;
	int b_cy;

	if(b_w - 16 <= 0) return;
	if(b_h - 16 <= 0) return;

	b_cx = b_x + (b_w >> 1);
	b_cy = b_y + (b_h >> 1);

	if(!is_bridge)
	{
		//buildings are easy
		dest_x = b_x + (rand() % (b_w - 16));
		dest_y = b_y + b_h + dist_from_entrance + (rand() % dist_from_entrance_box);
	}
	else
	{
		//bah..
		if(b_w > b_h) //horz bridge
		{
			if(rand() % 2)
			{
				if(c_cx > b_cx) // from the right
				{
					dest_x = b_x + b_w + dist_from_entrance + (rand() % dist_from_entrance_box);
					dest_y = b_y + (rand() % (b_h - 16));
				}
				else // from the left
				{
					dest_x = b_x - (dist_from_entrance + (rand() % dist_from_entrance_box));
					dest_y = b_y + (rand() % (b_h - 16));
				}
			}
			else
			{
				//on the bridge
				dest_x = b_x + (rand() % (b_w - 16));
				dest_y = b_y + 16 + (rand() % 16);
			}
		}
		else //vert bridge
		{
			if(rand() % 2)
			{
				if(c_cy < b_cy) //from the top
				{
					dest_x = b_x + (rand() % (b_w - 16));
					dest_y = b_y - (dist_from_entrance + (rand() % dist_from_entrance_box)) + 16;
				}
				else //from the bottom
				{
					dest_x = b_x + (rand() % (b_w - 16));
					dest_y = b_y + b_h + dist_from_entrance + (rand() % dist_from_entrance_box);
				}
			}
			else
			{
				//on the bridge
				dest_x = b_x + 16 + (rand() % 16);
				dest_y = b_y + (rand() % (b_h - 16));
			}
		}
	}
}

void ECraneConco::Init()
{
	int i, j;
	char filename_c[500];

	for(i=1;i<MAX_TEAM_TYPES;i++)
	{
		for(j=0;j<8;j++)
		{
			sprintf(filename_c, "assets/units/vehicles/crane/effects/conco_%s_n%02d.png", team_type_string[i].c_str(), j);
			//conco[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, conco[ZTEAM_BASE_TEAM][j], conco[i][j], filename_c);

			sprintf(filename_c, "assets/units/vehicles/crane/effects/sign_flip_%s_n%02d.png", team_type_string[i].c_str(), j);
			//sign_flip[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, sign_flip[ZTEAM_BASE_TEAM][j], sign_flip[i][j], filename_c);
		}

		sprintf(filename_c, "assets/units/vehicles/crane/effects/sign_%s.png", team_type_string[i].c_str());
		//sign[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, sign[ZTEAM_BASE_TEAM], sign[i], filename_c);

		sprintf(filename_c, "assets/units/vehicles/crane/effects/cone_no_shadow_%s.png", team_type_string[i].c_str());
		//cone_no_shadow[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, cone_no_shadow[ZTEAM_BASE_TEAM], cone_no_shadow[i], filename_c);

		sprintf(filename_c, "assets/units/vehicles/crane/effects/cone_%s.png", team_type_string[i].c_str());
		//cone[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, cone[ZTEAM_BASE_TEAM], cone[i], filename_c);

		for(j=0;j<2;j++)
		{
			sprintf(filename_c, "assets/units/vehicles/crane/effects/robot_jackhammer_%s_n%02d.png", team_type_string[i].c_str(), j);
			//robot_jackhammer[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, robot_jackhammer[ZTEAM_BASE_TEAM][j], robot_jackhammer[i][j], filename_c);

			sprintf(filename_c, "assets/units/vehicles/crane/effects/robot_paper_%s_n%02d.png", team_type_string[i].c_str(), j);
			//robot_paper[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, robot_paper[ZTEAM_BASE_TEAM][j], robot_paper[i][j], filename_c);
		}

		for(j=0;j<3;j++)
		{
			sprintf(filename_c, "assets/units/vehicles/crane/effects/robot_point_%s_n%02d.png", team_type_string[i].c_str(), j);
			//robot_point[i][j].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
			ZTeam::LoadZSurface(i, robot_point[ZTEAM_BASE_TEAM][j], robot_point[i][j], filename_c);
		}

		sprintf(filename_c, "assets/units/vehicles/crane/effects/robot_travel_left_%s.png", team_type_string[i].c_str());
		//robot_travel_left[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, robot_travel_left[ZTEAM_BASE_TEAM], robot_travel_left[i], filename_c);

		sprintf(filename_c, "assets/units/vehicles/crane/effects/robot_travel_right_%s.png", team_type_string[i].c_str());
		//robot_travel_right[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, robot_travel_right[ZTEAM_BASE_TEAM], robot_travel_right[i], filename_c);

		sprintf(filename_c, "assets/units/vehicles/crane/effects/robot_travel_updown_%s.png", team_type_string[i].c_str());
		//robot_travel_updown[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		ZTeam::LoadZSurface(i, robot_travel_updown[ZTEAM_BASE_TEAM], robot_travel_updown[i], filename_c);
	}

	//setup dimensions
	{
		if(conco[RED_TEAM][0].GetBaseSurface())
		{
			conco_w = conco[RED_TEAM][0].GetBaseSurface()->w;
			conco_h = conco[RED_TEAM][0].GetBaseSurface()->h;
		}

		if(cone[RED_TEAM].GetBaseSurface())
		{
			cone_w = cone[RED_TEAM].GetBaseSurface()->w;
			cone_h = cone[RED_TEAM].GetBaseSurface()->h;
		}

		if(sign[RED_TEAM].GetBaseSurface())
		{
			sign_w = sign[RED_TEAM].GetBaseSurface()->w;
			sign_h = sign[RED_TEAM].GetBaseSurface()->h;
		}
	}

	finished_init = true;
}

void ECraneConco::Process()
{
	double &the_time = ztime->ztime;
	int i;

	if(killme) return;

	if(the_time >= next_jackbot_time)
	{
		next_jackbot_time = the_time + 0.045 + ((rand() % 20) * 0.001);

		if(jackbot_i) jackbot_i = 0;
		else jackbot_i = 1;
	}

	if(the_time >= next_pbot_time)
	{
		next_pbot_time = the_time + 0.15 + ((rand() % 20) * 0.01);

		if(pbot_pointing)
		{
			if(pbot_i >= 2)
			{
				pbot_i = 0;
				pbot_pointing = false;
			}
			else
			{
				pbot_i++;
				next_pbot_time = the_time + 0.3 + ((rand() % 30) * 0.01);
			}
		}
		else
		{
			if(!(rand() % 10))
			{
				pbot_i = 0;
				pbot_pointing = true;
			}
			else
			{
				if(pbot_i) pbot_i = 0;
				else pbot_i = 1;
			}
		}
	}

	if(travel_to)
	{
		if(the_time < travel_time_end)
		{
			double percentage = (the_time - travel_time_start) / travel_time_width;
			conco_i = 7 * (1 - percentage);

			if(conco_i < 0) conco_i = 0;
			if(conco_i > 7) conco_i = 7;

			sign_i = conco_i;

			for(i=0;i<ECC_RENDER_ITEM_TYPES;i++)
				render_item[i].Move(percentage);
		}
		else
		{
			travel_to = false;
			conco_i = 0;

			for(i=0;i<ECC_RENDER_ITEM_TYPES;i++)
				render_item[i].MoveToDest();
		}

		render_item_list.sort(ecc_render_item_comp);
	}
	else if(travel_back)
	{
		if(the_time < travel_time_end)
		{
			double percentage = (the_time - travel_time_start) / travel_time_width;
			conco_i = 7 * (percentage);

			if(conco_i < 0) conco_i = 0;
			if(conco_i > 7) conco_i = 7;

			sign_i = conco_i;

			for(i=0;i<ECC_RENDER_ITEM_TYPES;i++)
				render_item[i].Move(percentage);

			render_item_list.sort(ecc_render_item_comp);
		}
		else
		{
			travel_back = false;
			killme = true;
		}
	}
}

void ECraneConco::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;
	ZSDL_Surface *surface;
	int i;
	int the_dir;

	if(killme) return;

	for(list<ECraneConcoItem*>::iterator r=render_item_list.begin(); r!=render_item_list.end(); r++)
	{
		surface = NULL;

		switch((*r)->type)
		{
		case ECC_CONCO:
			surface = &conco[our_team][conco_i];
			break;
		case ECC_SIGN:
			if(!travel_back && !travel_to)
				surface = &sign[our_team];
			else
				surface = &sign_flip[our_team][sign_i];
			break;
		case ECC_CONE0:
		case ECC_CONE1:
			if(!travel_back && !travel_to)
				surface = &cone[our_team];
			else
				surface = &cone_no_shadow[our_team];
			break;
		case ECC_JACK:
			if(!travel_back && !travel_to)
				surface = &robot_jackhammer[our_team][jackbot_i];
			else
			{
				if((*r)->w_dist > 0)
					surface = &robot_travel_right[our_team];
				else if((*r)->w_dist < 0)
					surface = &robot_travel_left[our_team];
				else
					surface = &robot_travel_updown[our_team];
			}
			break;
		case ECC_PAPER:
			if(!travel_back && !travel_to)
			{
				if(pbot_pointing)
					surface = &robot_point[our_team][pbot_i];
				else
					surface = &robot_paper[our_team][pbot_i];
			}
			else
			{
				if((*r)->w_dist > 0)
					surface = &robot_travel_right[our_team];
				else if((*r)->w_dist < 0)
					surface = &robot_travel_left[our_team];
				else
					surface = &robot_travel_updown[our_team];
			}
			break;
		}

		if(!surface) continue;

		zmap.RenderZSurface(surface, (*r)->x, (*r)->y);
		//if(zmap.GetBlitInfo( surface, (*r)->x, (*r)->y, from_rect, to_rect))
		//	SDL_BlitSurface( surface, &from_rect, dest, &to_rect);
	}

}

void ECraneConco::BeginDeath(int c_x, int c_y)
{
	double &the_time = ztime->ztime;
	int i;

	travel_back = true;
	travel_time_start = the_time;
	travel_time_end = travel_time_start + travel_time_width;

	for(i=0;i<ECC_RENDER_ITEM_TYPES;i++)
		render_item[i].SetReturn(c_x + 16, c_y + 16);
}

bool ecc_render_item_comp (ECraneConcoItem* a, ECraneConcoItem* b)
{
	if(a->y + a->h < b->y + b->h)
		return true;
	else
		return false;
}
