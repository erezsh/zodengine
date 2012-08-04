#include "zeffect.h"

ZSettings ZEffect::default_settings;
ZSettings *ZEffect::zsettings = &ZEffect::default_settings;
vector<ZEffect*> *ZEffect::effect_list = NULL;
ZMap *ZEffect::zmap = NULL;

ZEffect::ZEffect(ZTime *ztime_)
{
	killme = false;
	ztime = ztime_;
}

void ZEffect::SetSettings(ZSettings *zsettings_)
{
	zsettings = zsettings_;
}

void ZEffect::SetEffectList(vector<ZEffect*> *effect_list_)
{
	effect_list = effect_list_;
}

void ZEffect::Process()
{

}

bool ZEffect::KillMe()
{
	return killme;
}

effect_flags &ZEffect::GetEFlags()
{
	return eflags;
}
