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
					  UNABLE_OPEN_FILE
					};


    /// @brief Basic interface to process Json file similar.
    struct JsonFilter {
      virtual ~JsonFilter() = default;
      ParseErrorCode error {ParseErrorCode::OK};
      std::string message {}; // error message

      //rapidjosn filter
      virtual bool operator()(void* document = nullptr) = 0; // void to avoid rapidjson dependencies
      std::string filtered {}; // result of this filter
    };

    /// @brief Process external json file.
    /// @param [in] filename to process.
    /// @param [in,out] handler to be used.
    /// @return if success returns true, otherwise false.
    bool Parse(std::string filename, JsonFilter& handler);

    /// @brief Specific handler to generate HTML tables.
    /// @remark Style will be provided by CSS if needed.
    struct JsonSchema2HTML final: public JsonFilter {
      bool operator()(void* document = nullptr) override; // void to avoid rapidjson dependencies
    };

} // namespace

#endif // header
