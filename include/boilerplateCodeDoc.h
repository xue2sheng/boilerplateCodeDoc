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

      /// @brief Error code
      ParseErrorCode error {ParseErrorCode::OK};

      ///@brief Error message
      std::string message {}; // error message

      ///@brief Internal pointer: void to avoid rapidjson dependencies
      void* document_ptr {nullptr};

      ///@brief Json Schema title
      ///@remarks for debugging
      std::string title {};

      ///@brief Json Schema description
      ///@remarks for debugging
      std::string description {};
    };


    /// @brief Basic interface to process Json file similar.
    struct JsonSchemaFilter {

      ///@brief typical virtual destructor just in case
      virtual ~JsonSchemaFilter() = default;

      ///@brief simplest consturctor
      JsonSchemaFilter(std::string header_ = "", std::string footer_ = "") : header{std::move(header_)}, footer{std::move(footer_)} {}

      ///@brief Error code
      ParseErrorCode error {ParseErrorCode::OK};

      ///@brief Message associated to previous eror code
      std::string message {};

      ///@brief rapidjosn filter
      ///@remarks void to avoid rapidjson dependencies
      virtual bool operator()(const JsonSchema& jsonSchema) = 0;

      ///@brief result of that filter
      std::string filtered {};

      ///@brief preambule
      /// @remark to override
      std::string header {};

      ///@brief ending
      /// @remark to override
      std::string footer {};
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
	 table#boilerplate {
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

      ///@brief simplest constructor
      JsonSchema2HTML(std::string header_ = HEADER, std::string footer_ = FOOTER, std::string css_id_ = CSS_ID) :
	      JsonSchemaFilter(header_, footer_),  css_id{std::move(css_id_)} {}

      ///@brief filter to apply
      bool operator()(const JsonSchema& jsonSchema) override;

      ///@brief css id to be added to generated tables
      std::string css_id {};
    };

} // namespace

#endif // header
