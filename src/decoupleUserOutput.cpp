#include <boost/hana.hpp>
#include <rapidjson/rapidjson.h>
#include <cassert>
#include <string>
#include <fstream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>

// automatically generate version info
#include "version.h"

#include "decoupleUserOutput.h"

decoupleUserOutput::ParseErrorCode decoupleUserOutput::Parse(std::string filename, decoupleUserOutput::JsonHandler& handler) {

	if( filename.empty() ) {
		return decoupleUserOutput::ParseErrorCode::EMPTY_FILENAME;
	}

	try {

		std::ifstream json{filename};
		if( json.is_open() ) {

		     // temporary var
		     std::string contents;

		     // allocate all the memory up front
		     json.seekg(0, std::ios::end);
		     contents.reserve(json.tellg());
		     json.seekg(0, std::ios::beg);

		     // read data
		     contents.assign((std::istreambuf_iterator<char>(json)), std::istreambuf_iterator<char>());
		     rapidjson::StringStream ss{contents.c_str()};

		     rapidjson::Reader reader;
		     if (rapidjson::kParseErrorNone != reader.Parse(ss, handler)) {
			     return decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
		     }

		} else {
			return decoupleUserOutput::ParseErrorCode::UNABLE_OPEN_FILE;
		}

	} catch(...) {
		return decoupleUserOutput::ParseErrorCode::UNEXPECTED_ERROR_PROCESSING_SCHEMA_JSON_FILE;
	}

	return decoupleUserOutput::ParseErrorCode::OK;
}
