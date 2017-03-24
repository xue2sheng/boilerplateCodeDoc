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

static inline std::string to_string(const decoupleUserOutput::ParseErrorCode& error) {
	    switch(error) {
		default: return "Unknown ParseErrorCode";
		case decoupleUserOutput::ParseErrorCode::OK: return "OK";
		case decoupleUserOutput::ParseErrorCode::EMPTY_FILENAME: return "EMPTY_FILENAME";
		case decoupleUserOutput::ParseErrorCode::UNEXPECTED_ERROR_PROCESSING_SCHEMA_JSON_FILE: return "UNEXPECTED_ERROR_PROCESSING_SCHEMA_JSON_FILE";
		case decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON: return "ERROR_PARSING_SCHEMA_JSON";
		case decoupleUserOutput::ParseErrorCode::UNABLE_OPEN_FILE: return "UNABLE_OPEN_FILE";
	    }
}

bool decoupleUserOutput::Parse(std::string filename, decoupleUserOutput::JsonHandler& handler) {

	if( filename.empty() ) {
		handler.error = decoupleUserOutput::ParseErrorCode::EMPTY_FILENAME;
		handler.message = to_string(handler.error);
		return true;
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
		     rapidjson::ParseResult result = reader.Parse(ss, handler);
		     if (rapidjson::kParseErrorNone != result) {

			     handler.error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
			     handler.message = to_string(handler.error);
			     return false;
		     }

		} else {
			handler.error = decoupleUserOutput::ParseErrorCode::UNABLE_OPEN_FILE;
			handler.message = to_string(handler.error);
			return false;
		}

	} catch(...) {
		handler.error = decoupleUserOutput::ParseErrorCode::UNEXPECTED_ERROR_PROCESSING_SCHEMA_JSON_FILE;
		handler.message = to_string(handler.error);
		return false;
	}

	handler.error = decoupleUserOutput::ParseErrorCode::OK;
	handler.message = to_string(handler.error);
	return true;
}
