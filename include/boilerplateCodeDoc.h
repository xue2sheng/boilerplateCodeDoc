/** @file boilerplateCodeDoc.h
 * @brief Example of decoupling User Output from the rest of the app.
 *
 * @remark Avoid including any rapidjson header in this file.
 *
 * @bug Abuse of JSON Schema validation leads to crushes if an invalid json is queried (missing redundant checks to speed up).
 *
 */

#ifndef BOILERPLATECODEDOC_H
#define BOILERPLATECODEDOC_H

#include <string>
#include <vector>

/// @brief Decouple User Output for the simple example.
/// @remark long name on propose in order to avoid name collisions.
/// @remark an alias might be defined by users as: namespace decouple = boilerplateCodeDoc;
namespace boilerplateCodeDoc {

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

      ///@brief typical virtual destructor just in case
      virtual ~JsonSchemaFilter() = default;

      ///@brief Error code
      ParseErrorCode error {ParseErrorCode::OK};

      ///@brief Message associated to previous eror code
      std::string message {};

      ///@brief rapidjosn filter
      ///@remarks void to avoid rapidjson dependencies
      virtual bool operator()(const JsonSchema& jsonSchema) = 0;

      ///@brief result of that filter
      std::string filtered {};
    };


    /// @brief Specific handler to generate HTML tables.
    /// @remark Style will be provided by CSS if needed.
    struct JsonSchema2HTML final: public JsonSchemaFilter {

      ///@brief Default CSS id
      static constexpr const char* const CSS_ID {"boilerplate"};

      ///@brief Default HTML head+body header
      static constexpr const char* const HEADER {R"(
	 <!DOCTYPE html>
	 <html>
	 <head>
	 <style>
	 table, th, td {
	     border: 1px solid black;
	     border-collapse: collapse;
	 }
	 th, td {
	     padding: 5px;
	     text-align: left;
	 }
	 table#t01 {
	     width: 100%;
	     background-color: #f1f1c1;
	 }
	 </style>
	 </head>
	 <body>
      )"};

      ///@brief Default HTML body footer
      static constexpr const char* const FOOTER {R"(
	 </body>
	 </html>
      )"};

      ///@brief filter to apply
      bool operator()(const JsonSchema& jsonSchema) override;

      ///@brief css id to be added to generated tables
      std::string css_id {CSS_ID};

      ///@brief HTML page header
      std::string header {HEADER};

      ///@brief HTML page footer
      std::string footer {FOOTER};
    };

} // namespace

#endif // header
