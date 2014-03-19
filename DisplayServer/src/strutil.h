#ifndef STRUTIL_H
#define STRUTIL_H

#include <string>
#include <cwchar>

std::wstring mb_to_wstring(const std::string &s)
{
	std::mbstate_t state = std::mbstate_t(); // initial state
	const char* ptr = s.data();
	const char* end = s.data() + s.length();
	int len;
	wchar_t wc;
	std::wstring ws;

	while((len = std::mbrtowc(&wc, ptr, end-ptr, &state)) > 0) {
		ws += wc;
		ptr += len;
	}

	return ws;
}

#endif // STRUTIL_H
