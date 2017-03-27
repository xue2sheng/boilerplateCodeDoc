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
#include <vector>

/// @brief Decouple User Output for the simple example.
/// @remark long name on propose in order to avoid name collisions.
/// @remark an alias might be defined by users as: namespace decouple = decoupleUserOutput;
namespace decoupleUserOutput {

    /// @brief Specific error code.
    enum class ParseErrorCode: unsigned { OK = 0,
					  EMPTY_FILENAME,
					  UNEXPECTED_ERROR_PROCESSING_SCHEMA_JSON_FILE,
					  ERROR_PARSING_SCHEMA_JSON,
                      UNABLE_OPEN_FILE,
                      ERROR_FILTERING_DOCUMENT
					};

    /// @brief minimum wrapper for json schemas read by rapidjson
    struct JsonSchema {

      // get rid of json schema document
      ~JsonSchema();

      /// @brief Process external json file.
      /// @param [in] filename to process.
      /// @return if success returns true, otherwise false.
      explicit JsonSchema(std::string filename);

      ParseErrorCode error {ParseErrorCode::OK};
      std::string message {}; // error message
      void* document_ptr {nullptr}; // void to avoid rapidjson dependencies
      std::string title {};
      std::string description {};
    };


    /// @brief Basic interface to process Json file similar.
    struct JsonSchemaFilter {
      virtual ~JsonSchemaFilter() = default;
      ParseErrorCode error {ParseErrorCode::OK};
      std::string message {}; // error message

      //rapidjosn filter
      virtual bool operator()(const JsonSchema& jsonSchema) = 0; // void to avoid rapidjson dependencies
      std::string filtered {}; // result of this filter
    };

    /// @brief Specific handler to generate HTML tables.
    /// @remark Style will be provided by CSS if needed.
    struct JsonSchema2HTML final: public JsonSchemaFilter {
      bool operator()(const JsonSchema& jsonSchema) override;
      std::string css_id {};
      std::string css {};
    };

} // namespace

#endif // header
