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

    /// @brief Specific error code.
    enum class ParseErrorCode: unsigned { OK = 0,
					  EMPTY_FILENAME,
					  UNEXPECTED_ERROR_PROCESSING_SCHEMA_JSON_FILE,
					  ERROR_PARSING_SCHEMA_JSON,
					  UNABLE_OPEN_FILE
					};


    /// @brief Basic interface to process Json file similar to SAX xml way.
    struct JsonHandler {
      virtual ~JsonHandler() = default;
      ParseErrorCode error {ParseErrorCode::OK};
      std::string message {}; // error message

      //rapidjosn callbacks
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

    /// @brief Process external json file.
    /// @param [in] filename to process.
    /// @param [in,out] handler to be used.
    /// @return if success returns true, otherwise false.
    bool Parse(std::string filename, JsonHandler& handler);

    /// @brief Specific handler to generate Markdown tables for HTML pandoc final transformation.
    struct JsonSchema2GithubMarkdown final: public JsonHandler {
      bool Null() override;
      bool Bool(bool b) override;
      bool Int(int i) override;
      bool Uint(unsigned u) override;
      bool Int64(int64_t i) override;
      bool Uint64(uint64_t u) override;
      bool Double(double d) override;
      bool RawNumber(const char* str, /*rapidjson::SizeType*/ unsigned length, bool copy) override;
      bool String(const char* str, /*rapidjson::SizeType*/ unsigned length, bool copy) override;
      bool StartObject() override;
      bool Key(const char* str, /*rapidjson::SizeType*/ unsigned length, bool copy) override;
      bool EndObject(/*rapidjson::SizeType*/ unsigned memberCount) override;
      bool StartArray() override;
      bool EndArray(/*rapidjson::SizeType*/ unsigned elementCount) override;
    };

} // namespace

#endif // header
