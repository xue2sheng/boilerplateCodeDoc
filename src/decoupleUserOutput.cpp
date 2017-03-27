#include <boost/hana.hpp>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <map>
#include <set>
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

			      error = decoupleUserOutput::ParseErrorCode::OK;
			      message = to_string(error);

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

static inline bool ignore(const std::string& name, const std::set<std::string>& ignoreSet) {
	    return ignoreSet.end() !=  ignoreSet.find(name);
}
using Required = std::vector<std::string>;
using OneOf = std::vector<Required>;
struct Property {
	bool required {false};
	std::string name {};
	Property(bool req, std::string nam) : required{req}, name{std::move(nam)} {}
};
using Properties = std::map<std::string, Property>;

static OneOf collectOneOf(auto&& i) { return OneOf{}; }
static Required collectRequired(auto&& i) { return Required{}; }
static Properties collectProperties(auto&& i)
{
	Properties result {};
	for(auto&& j = i.MemberBegin(); j != i.MemberEnd(); ++j) {
	  std::string name{j->name.GetString()};
	  result.emplace(std::make_pair(name, std::move(Property{false, name})));
	}
	return result;
}
static void processProperties(const OneOf& oneOf, const Required& required, Properties& propierties)
{
	if( oneOf.size() > 0 ) {

	} else if ( required.size() > 0 ) {

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

	    static std::set<std::string> ignoreSchemaRoot {"description", "title", "$schema", "type"};

	    OneOf oneOf {};
	    Required required {};
	    Properties properties {};
	    for(auto&& i = document.MemberBegin(); i != document.MemberEnd(); ++i) {
		std::string name {i->name.GetString()};
		if(ignore(name, ignoreSchemaRoot)) { continue; }
		if( name == "oneOf" ) { oneOf = collectOneOf(i); }
		if( name == "required" ) { required = collectRequired(i); }
		if( name == "properties" ) { properties = collectProperties(document["properties"]); }
	    }

	    for(const auto& o : oneOf) { filtered += "oneOf: " + std::to_string(o.size()) + "\n"; }
	    for(const auto& r : required) { filtered += "required: " + r + "\n"; }
	    for(const auto& p : properties) { filtered += "propierties: " + p.second.name + "\n"; }

	     error = decoupleUserOutput::ParseErrorCode::OK;
	     message = to_string(error);
	     return true;

    } catch(...) {
	error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
	message = "Unexpected exception";
	return false;
    }
}
