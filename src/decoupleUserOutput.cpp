#include <boost/hana.hpp>
#include <rapidjson/rapidjson.h>
#include <cassert>
#include <string>
#include <fstream>
#include <set>
#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>

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

bool decoupleUserOutput::Parse(std::string filename, decoupleUserOutput::JsonHandler& handler)
{

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
		     if (rapidjson::kParseErrorNone != result && rapidjson::kParseErrorDocumentEmpty != result) {
			     handler.error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
			     handler.message = to_string(handler.error);
			     handler.message += " [";
			     handler.message += std::string(rapidjson::GetParseError_En(result.Code())) + " : " + std::to_string(result.Offset());
			     handler.message += "]";
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

bool decoupleUserOutput::JsonSchema2GithubMarkdown::Null() { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::Bool(bool b) { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::Int(int i) { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::Uint(unsigned u) { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::Int64(int64_t i) { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::Uint64(uint64_t u) { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::Double(double d) { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::RawNumber(const char* str, rapidjson::SizeType length, bool copy) { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::String(const char* str, rapidjson::SizeType length, bool copy) { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::StartArray() { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::EndArray(rapidjson::SizeType elementCount) { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::StartObject() { return true; }
bool decoupleUserOutput::JsonSchema2GithubMarkdown::EndObject(rapidjson::SizeType memberCount) { return true; }

static inline bool reservedWord(const std::string& key) {
	static std::set<std::string> reserved {"type", "minimum", "items","properties","default","maximum","enum","required","description"};
	return reserved.count(key) != 0;
}

std::string decoupleUserOutput::JsonSchema2GithubMarkdown::to_markdown() const
{
	std::string result;
	for(const auto& t : table) { result += t + "\n"; }
	return result;
}

bool decoupleUserOutput::JsonSchema2GithubMarkdown::Key(const char* str, rapidjson::SizeType length, bool copy)
{
	std::string key(str);

	if(not reservedWord(key) ) {
		table.emplace_back(std::move(key));
	}
	return true;
}


