#include "Exception.h"

#include "Base64.h"

using namespace std;

const char *Base64::DEFAULT_MAPPING = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
const char *Base64::URLSAFE_MAPPING = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=";

Base64::Base64(const std::string &mapping) {
	if(mapping.length() < 65) {
		throw Exception("Base64", "Mapping string too short!");
	}

	m_mapping = mapping.substr(0, 65);

	m_invMapping.resize(256, -1);
	for(string::size_type i = 0; i < mapping.length(); i++) {
		m_invMapping[mapping[i]] = i;
	}
}

std::string Base64::encode(const std::string &data) {
	const unsigned char *data_ptr = reinterpret_cast<const unsigned char*>(data.c_str());
	unsigned long pos = 0;
	unsigned int tmp = 0;

	std::string output;

	int bytes_to_process;
	do {
		if(pos < (data.length() - 2))
			bytes_to_process = 3;
		else
			bytes_to_process = data.length() % 3;

		if(bytes_to_process != 0) {
			tmp = 0;
			// read bytes from the input
			for(int i = 0; i < bytes_to_process; i++) {
				tmp |= static_cast<unsigned int>(data_ptr[pos + i]) << (8 * (2-i));
			}

			// map them to the output
			for(int i = 0; i < (bytes_to_process + 1); i++) {
				output += m_mapping[(tmp & (0x3F << (6 * (3-i)))) >> (6 * (3-i))];
			}

			// at the end, apply padding as necessary
			for(int i = 3; i > bytes_to_process; i--) {
				output += m_mapping[64];
			}
		}

		pos += 3;
	} while(bytes_to_process == 3);

	return output;
}

std::string Base64::decode(const std::string &str) {
	const unsigned char *str_ptr;
	unsigned long pos = 0;
	unsigned int tmp = 0;

	string cleanedStr;

	// remove spaces and newlines from the base64-string
	for(string::size_type i = 0; i < str.length(); i++) {
		if(str[i] != ' ' && str[i] != '\n' && str[i] != '\r')
			cleanedStr += str[i];
	}

	// length of base64-encoded data without overhead must be divisible by 4
	if(cleanedStr.length() % 4 != 0) {
		throw Exception("Base64", "Encoded data length is not divisible by 4!");
	}

	str_ptr = reinterpret_cast<const unsigned char*>(cleanedStr.c_str());

	std::string output;

	int bytes_to_process, i;

	do {
		tmp = 0;
		// read 4 chars from the input and merge them into an integer
		for(i = 0; i < 4; i++) {
			// stop reading on the first padding character
			if(str_ptr[pos + i] == m_mapping[64])
				break;

			tmp |= static_cast<unsigned int>(m_invMapping[str_ptr[pos + i]] & 0x3F) << (6 * (3-i));
		}

		bytes_to_process = i - 1;

		// get the original data out of temp and append it to the output
		for(i = 0; i < bytes_to_process; i++) {
			output += static_cast<char>((tmp >> (8 * (2-i))) & 0xFF);
		}

		pos += 4;
	} while(pos < cleanedStr.length());

	return output;
}
