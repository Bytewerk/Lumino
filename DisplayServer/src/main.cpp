#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

#include "Font.h"
#include "Logger.h"
#include "Bitmap.h"

#include "strutil.h"

using namespace std;

int main(void)
{
	FT_Library ftlib;
	int fterror = 0;

	std::setlocale(LC_ALL, "en_US.UTF-8");

	LOG(Logger::LVL_INFO, "main", "This is DisplayServer v" VERSION);

	fterror = FT_Init_FreeType(&ftlib);
	if(fterror) {
		LOG(Logger::LVL_FATAL, "main", "Failed to initialize freetype: %i", fterror);
		return 1;
	}

	Font testFont(&ftlib, "/usr/share/fonts/dejavu/DejaVuSans.ttf", 10);

	std::wstring testtext = mb_to_wstring(u8"Hello FreeType! With Ünïĉødè: ∫x²dx=x³/₃");
	testFont.renderText(testtext, NULL);
}
