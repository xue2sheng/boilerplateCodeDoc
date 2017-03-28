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

// debug
#include <iostream>

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
	std::string type {};
	std::string description {};
	Property(bool req, std::string nam, std::string typ, std::string des)
		: required{req}, name{std::move(nam)}, type{std::move(typ)}, description{std::move(des)} {}
};
using Properties = std::map<std::string, Property>;

static bool getString(const rapidjson::Document& document, const std::string& element, const std::string& prefix, const std::string& name, const std::string& suffix, std::string& result)
{
  // Do nothing if there's nothing to do
  std::string temp {element + prefix + name + suffix};
  if( not rapidjson::Pointer(temp.c_str()).IsValid() ) { return false; }
  const rapidjson::Value* pointer {rapidjson::Pointer(temp.c_str()).Get(document)};
  if( not pointer ) { return false; }
  if( pointer->IsNull() ) { return false; }
  if( not pointer->IsString() ) { return false; }
  result = pointer->GetString();
  return true;
}

static void processProperties(const OneOf& oneOf, const Required& required, Properties& properties)
{
	if( oneOf.size() > 0 ) {

		// tricky part of selecting which "oneOf" array of requirements must be applied
		for(const auto& o : oneOf) {
			bool allRequirementsPresent {false};
			for(const auto& r : o) {
// TODO: oneOf
			}
		}

	} else if ( required.size() > 0 ) {
		for(const auto& r : required) {
			auto found = properties.find(r);
			if( found != properties.end() ) {
				found->second.required = true;
			}

		}
	}
}

static std::set<std::string> ignoreSchemaRoot {"description", "title", "$schema", "type"};

using lambda_t = std::function<void(const OneOf&, const Required&, const Properties&, std::string&)>;

static lambda_t html = [](const OneOf& oneOf, const Required& required, const Properties& properties, std::string& filtered)
{
    for(const auto& p : properties) { filtered += p.second.name
			    + (p.second.required?"<required>":"")
			    + "{" + p.second.type + "}\n"; }
};

static void SetProperties(const rapidjson::Document& document, std::string element, const std::set<std::string> ignoreSet, std::string& filtered, const lambda_t& lambda)
{
    OneOf oneOf {};
    Required required {};
    Properties properties {};

    // Do nothing if there's nothing to do
    if( not rapidjson::Pointer(element.c_str()).IsValid() ) { return; }
    const rapidjson::Value* pointer {rapidjson::Pointer(element.c_str()).Get(document)};
    if( not pointer ) { return; }
    if( pointer->IsNull() ) { return; }
    if( not pointer->IsObject() ) { return; }

    auto&& object {pointer->GetObject()};

    for(rapidjson::Value::ConstMemberIterator i = pointer->GetObject().MemberBegin(); i != pointer->GetObject().MemberEnd(); ++i) {
        std::string name {i->name.GetString()};
        if(ignore(name, ignoreSchemaRoot)) { continue; }
        if( name == "oneOf" ) {
        } else if( name == "required" ) {
            for(auto&& j : object["required"].GetArray()) {
                required.emplace_back(std::string{j.GetString()});
            }
        } else if( name == "properties" ) {
            for(auto&& j = object["properties"].MemberBegin(); j != object["properties"].MemberEnd(); ++j) {
              std::string name{j->name.GetString()};

              std::string type {}; // required
              if(not getString(document, element, "/properties/", name, "/type", type)) { continue; }

              std::string description {}; // optional
              getString(document, element, "/properties/", name, "/description", description);

              properties.emplace(std::make_pair(name, Property{false, name, type, description}));
            }
        }
    }
    processProperties(oneOf, required, properties);

    lambda(oneOf, required, properties, filtered);

    // recursive call


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
	    if(document.IsNull() || not document.IsObject() ) {
		error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
		message = "Root element shouldn't be NULL";
		return false;
	    }

	     //std::string element {"#/properties/imp/items/properties/native"};
	     std::string element {"#"};
	     SetProperties(document, element, ignoreSchemaRoot, filtered, html);

	     error = decoupleUserOutput::ParseErrorCode::OK;
	     message = to_string(error);
	     return true;

    } catch(...) {
	error = decoupleUserOutput::ParseErrorCode::ERROR_PARSING_SCHEMA_JSON;
	message = "Unexpected exception";
	return false;
    }
}
