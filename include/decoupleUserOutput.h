/** @file decoupleUserOutput.h
 * @brief Example of decoupling User Output from the rest of the app.
 *
 * @remark Avoid including any rapidjson header in this file.
 *
 * @bug Abuse of JSON Schema validation leads to crushes if an invalid json is queried (missing redundant checks to speed up).
 *
 */

#ifndef DECOUPLEUSEROUTPUT_H
#define DECOUPLEUSEROUTPUT_H

#include <string>

/// @brief Decouple User Output for the simple example.
/// @remark long name on propose in order to avoid name collisions.
/// @remark an alias might be defined by users as: namespace decouple = decoupleUserOutput;
namespace decoupleUserOutput {

    /// @brief Basic interface to process Json file similar to SAX xml way.
    struct JsonHandler {
      virtual ~JsonHandler() = default;
      virtual bool Null() = 0;
      virtual bool Bool(bool b) = 0;
      virtual bool Int(int i) = 0;
      virtual bool Uint(unsigned u) = 0;
      virtual bool Int64(int64_t i) = 0;
      virtual bool Uint64(uint64_t u) = 0;
      virtual bool Double(double d) = 0;
      virtual bool RawNumber(const char* str, /*rapidjson::SizeType*/ unsigned length, bool copy) = 0;
      virtual bool String(const char* str, /*rapidjson::SizeType*/ unsigned length, bool copy) = 0;
      virtual bool StartObject() = 0;
      virtual bool Key(const char* str, /*rapidjson::SizeType*/ unsigned length, bool copy) = 0;
      virtual bool EndObject(/*rapidjson::SizeType*/ unsigned memberCount) = 0;
      virtual bool StartArray() = 0;
      virtual bool EndArray(/*rapidjson::SizeType*/ unsigned elementCount) = 0;
    };

    /// @brief Specific error code.
    enum class ParseErrorCode: unsigned { OK = 0,
					  EMPTY_FILENAME,
					  UNEXPECTED_ERROR_PROCESSING_SCHEMA_JSON_FILE,
					  ERROR_PARSING_SCHEMA_JSON,
					  UNABLE_OPEN_FILE
					};

    /// @brief Translate into string the error code.
    /// @param [in] error to be transformed.
    /// @remark For dubugging.
    static inline std::string to_String(const ParseErrorCode& error) {
	    switch(error) {
		default: return "Unknown ParseErrorCode";
		case ParseErrorCode::OK: return "OK";
		case ParseErrorCode::EMPTY_FILENAME: return "EMPTY_FILENAME";
		case ParseErrorCode::UNEXPECTED_ERROR_PROCESSING_SCHEMA_JSON_FILE: return "UNEXPECTED_ERROR_PROCESSING_SCHEMA_JSON_FILE";
		case ParseErrorCode::ERROR_PARSING_SCHEMA_JSON: return "ERROR_PARSING_SCHEMA_JSON";
		case ParseErrorCode::UNABLE_OPEN_FILE: return "UNABLE_OPEN_FILE";
	    }
    }

    /// @brief Process external json file.
    /// @param [in] filename to process.
    /// @param [in,out] handler to be used.
    /// @return error code if any.
    ParseErrorCode Parse(std::string filename, JsonHandler& handler);
};


#endif // DECOUPLEUSEROUTPUT_H

