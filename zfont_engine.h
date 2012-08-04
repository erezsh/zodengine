#ifndef _ZFONT_ENGINE_H_
#define _ZFONT_ENGINE_H_

#include "zfont.h"

class ZFontEngine
{
	public:
		ZFontEngine();

		static void Init();
		static ZFont &GetFont(int font_type);
	private:
		static ZFont zfont[MAX_FONT_TYPES];
};

#endif
