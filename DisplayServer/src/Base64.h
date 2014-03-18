#ifndef UTILS_BASE64_H
#define UTILS_BASE64_H

#include <string>

/*!
 * \brief A class for Base64 encoding and decoding.
 */
class Base64 {
	public:
		// *** Constants
		static const char *DEFAULT_MAPPING; /*< standard 65-character set */
		static const char *URLSAFE_MAPPING; /*< alternate 65-character set (URL safe) */

	private:
		// *** Attributes
		std::string m_mapping; /*< the character set used for Base64 encoding */
		std::string m_invMapping; /*< inverse mapping for faster decoding */

	public:
		// *** Constructor and Destructor

		/*!
		 * Create a Base64 converter with a user defined mapping.
		 *
		 * \param mapping   A string containing one character for each of the 64
		 *                  possible values in Base64 plus one for padding. If
		 *                  the string is shorter than 65 characters, an
		 *                  Exception will be thrown, otherwise the first 65
		 *                  characters will be used.
		 */
		Base64(const std::string &mapping);

		// *** Methods

		/*!
		 * Base64-encode the given string.
		 *
		 * \param data   The data to be encoded.
		 * \return       The Base64 representation of the data.
		 */
		std::string encode(const std::string &data);

		/*!
		 * Base64-decode the given string.
		 *
		 * \param str  The Base64-encoded string.
		 * \return     The decoded data
		 */
		std::string decode(const std::string &str);
};

#endif // UTILS_BASE64_H
