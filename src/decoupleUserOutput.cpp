#include <boost/hana.hpp>
#include <string>
#include <fstream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/pointer.h>

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

decoupleUserOutput::JsonSchema::~JsonSchema()
{
	if(document_ptr) {
		delete reinterpret_cast<rapidjson::Document*>(document_ptr);
		document_ptr = nullptr;
	}
}

decoupleUserOutput::JsonSchema::JsonSchema(std::string filename)
{

	if( filename.empty() ) {
		error = decoupleUserOutput::ParseErrorCode::EMPTY_FILENAME;
		message = to_string(error);
		return;
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

		     // get json ready to process
		     rapidjson::Document* temp = new rapidjson::Document;
		     rapidjson::Document& document {*temp};
		     document_ptr = reinterpret_cast<void*>(temp);

		     rapidjson::ParseResult ok = document.Parse(contents.c_str());
		     if( not ok ) {
			 error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
			 message = to_string(error);
			 message += " [";
			 message += std::string(rapidjson::GetParseError_En(ok.Code())) + " : " + std::to_string(ok.Offset());
			 message += "]";
			 return;
		     }

		     if( document.IsNull() ) {
			     error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
			     message = "Json Schema document shouldn't be NULL";
			     return;
		     }

		     if(document.IsObject()) {
			     if( not document.HasMember("$schema") ) {
				error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
				message = "Missing expected $schema root object";
				return;
			     }
			      if( not document.HasMember("title") ) {
				error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
				message = "Missing expected schema title";
				return;
			     }

			      if( not document.HasMember("description") ) {
				error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
				message = "Missing expected schema description";
				return;
			     }

			      title = document["title"].GetString();
			      description = document["description"].GetString();

		     } else {
			error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
			message = "Root element of a schema json should be an object";
		     }

		} else {
			error = decoupleUserOutput::ParseErrorCode::UNABLE_OPEN_FILE;
			message = to_string(error);
		}

	} catch(...) {
		error = decoupleUserOutput::ParseErrorCode::UNEXPECTED_ERROR_PROCESSING_SCHEMA_JSON_FILE;
		message = to_string(error);
	}
}

bool decoupleUserOutput::JsonSchema2HTML::operator()(const decoupleUserOutput::JsonSchema& jsonSchema)
{
    if( not jsonSchema.document_ptr ) {
	error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
	message = "Empty document pointer";
	return false;
    }

    rapidjson::Document& document {*reinterpret_cast<rapidjson::Document*>(jsonSchema.document_ptr)};
    try {
	    if(document.IsNull()) {
		error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
		message = "Root element shouldn't be NULL";
		return false;
	    }

	    for(rapidjson::Value::ConstMemberIterator i = document.MemberBegin(); i != document.MemberEnd(); ++i) {
		filtered += std::string(i->name.GetString()) + "\n";
	    }

    } catch(...) {
	error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
	message = "Unexpected exception";
	return false;
    }

    return true;
}
