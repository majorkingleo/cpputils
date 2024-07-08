/**
 * read text files with correct encoding
 * @author Copyright (c) 2022 - 2024 Martin Oberzalek
 */

#ifndef READ_FILE_H_
#define READ_FILE_H_

#include "../../tools_config.h"

#ifndef DISABLE_CPPUTILS_READFILE

#include <string>
#include <optional>
#include <array>
#include <span>
#include <stdint.h>

class ReadFile
{
public:
	struct BOM
	{
	private:
		const char * encoding;
		std::span<uint8_t> bom;
		std::array<uint8_t,4> bom_data;

	public:
		BOM( const char *encoding_, const std::initializer_list<uint8_t> & bom_il_data );
		BOM( const BOM & other );

		BOM & operator=( const BOM & other ) {
			encoding = other.encoding;
			bom_data = other.bom_data;
			bom = { bom_data.data(), other.bom.size() };
			return *this;
		}

		bool hasBom( const std::string_view & s ) const;

		const char *getEncoding() const {
			return encoding;
		}

		const std::span<uint8_t> & getBom() const {
			return bom;
		}
	};

	static std::array<const ReadFile::BOM*,5> BOM_LIST;

	static const BOM BOM_UTF8;;
	static const BOM BOM_UTF16LE;
	static const BOM BOM_UTF16BE;
	static const BOM BOM_UTF32LE;
	static const BOM BOM_UTF32BE;

protected:

	std::string error;
	std::string encoding;
	std::optional<BOM> bom;

public:
	ReadFile();

	bool read_file( const std::string & name, std::wstring & content );

	const std::string & getError() const {
		return error;
	}

	std::string convert( const std::string & s, const std::string & from, const std::string & to );
	bool convert( const std::string & s, const std::string & from, const std::string & to, std::string & result );

	std::string latin12utf8( const std::string & s ) {
	    return convert( s, "LATIN1", "UTF-8" );
	}

	std::string utf82latin1( const std::string & s ) {
	    return convert( s, "UTF-8", "LATIN1" );
	}

	const std::string & getFileEncoding() const {
		return encoding;
	}

	/** indicates if a byte order mark (BOM) was detected and cut off */
	bool hasBom() const {
		return bom.has_value();
	}

	const std::optional<BOM> & getBom() const {
		return bom;
	}
};

extern ReadFile READ_FILE;

#endif

#endif /* READ_FILE_H_ */
