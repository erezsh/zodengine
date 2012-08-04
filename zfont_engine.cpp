#include "zfont_engine.h"

ZFont ZFontEngine::zfont[MAX_FONT_TYPES];

ZFontEngine::ZFontEngine()
{

}

void ZFontEngine::Init()
{
	int i;

	for(i=0;i<MAX_FONT_TYPES;i++)
	{
		zfont[i].SetType(i);
		zfont[i].Init();
	}
}

ZFont &ZFontEngine::GetFont(int font_type)
{
	return zfont[font_type];
}
