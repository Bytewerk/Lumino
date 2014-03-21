#ifndef STRUTIL_H
#define STRUTIL_H

#include <string>
#include <vector>
#include <cwchar>
#include <sstream>

#include "Exception.h"

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

int split(const std::string &s, std::vector<std::string> *results, const std::string &delim = " ")
{
	std::string::size_type start = 0, end;

	results->clear();

	do {
		end = s.find(delim, start);

		results->push_back(s.substr(start, end - start));

		start = end + delim.length();
	} while(end != std::string::npos);

	return results->size();
}

bool get_line_from_string(std::string *str, std::string *line, char delim = '\n')
{
	std::string::size_type i;

	line->clear();

	for(i = 0; (i < str->length()) && ((*str)[i] != delim); i++) {
		line->append(1, (*str)[i]);
	}

	if(i < str->length()) {
		// delimiter as found
		*str = str->substr(i+1); // remove line including delim from str
		return true;
	} else {
		return false;
	}
}

template<typename T>
  T to_var(const std::string &str)
{
	std::stringstream Str;
  Str << str;
  T var;
  Str >> var;

  if(Str.fail() || Str.bad()) {
  	throw IOException("to_var", "Conversion failed.");
	}

  return var;
}

template<typename T>
  std::string to_str(const T &var)
{
	std::ostringstream Str;
  Str << var;

  if(Str.fail() || Str.bad()) {
  	throw IOException("to_str", "Conversion failed.");
	}

  return Str.str();
}


#endif // STRUTIL_H
