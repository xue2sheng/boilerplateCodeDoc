#include <iostream>
#include <fstream>
#include <string>
#include "boilerplateCodeDoc.h"
#include "version.h"

using namespace boilerplateCodeDoc;

static bool processFilter(const JsonSchema& schema, const std::string& filename, JsonSchemaFilter& filter)
{
    bool result = filter(schema);

    if( not result ) {

		std::cout << filter.message << std::endl << std::endl;

    } else {

	try {
		std::ofstream filterFile {filename};
		if(filterFile) {

			filterFile.write(filter.filtered.c_str(), filter.filtered.size());
			filterFile.close(); // maybe redundant

		} else {
			std::cout << "Error opening " << filename << std::endl;
			return false;
		}

	} catch(...) {
		std::cout << "Unexpected exception while writing to " << filename << std::endl;
		return 1;
	}
    }
    return result;
}

int main(int argc, char** argv)
{
   if( argc != 4 ) {
     std::cout << "Usage:\n\n" << argv[0] << " <Json Schema Input File Name> <HTML Output File Name> <C++ header Output File Name>\n\n";
     return 1;
   }

   std::string schema{argv[1]};
   std::string html{argv[2]};
   std::string h{argv[3]};

   JsonSchema jsonSchema{schema};
   if(jsonSchema.error != ParseErrorCode::OK) { std::cout << jsonSchema.message << std::endl; return 1; }

   JsonSchema2H hFilter{};
   bool resultH = processFilter(jsonSchema, h, hFilter);

   JsonSchema2HTML htmlFilter{};
   bool resultHTML = processFilter(jsonSchema, html, htmlFilter);

   return (resultH && resultHTML) ? 0 : 1;
}
