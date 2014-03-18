#include <ft2build.h>
#include FT_FREETYPE_H

#include "Font.h"
#include "Logger.h"

using namespace std;

int main(void)
{
	FT_Library ftlib;
	int fterror = 0;

	LOG(Logger::LVL_INFO, "main", "This is DisplayServer v" VERSION);

	fterror = FT_Init_FreeType(&ftlib);
	if(fterror) {
		LOG(Logger::LVL_FATAL, "main", "Failed to initialize freetype: %i", fterror);
		return 1;
	}

	Font testFont(&ftlib, "/usr/share/fonts/dejavu/DejaVuSans.ttf", 10);
	testFont.renderText("e", NULL);
}
