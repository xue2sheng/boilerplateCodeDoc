#include <iostream>
#include <fstream>
#include <string>
#include "boilerplateCodeDoc.h"
#include "version.h"

using namespace boilerplateCodeDoc;

static bool processFilter(const JsonSchema& schema, const std::string& filename, JsonSchemaFilter& filter)
{
    // process filter and save result
    bool result = filter(schema);

    if( not result ) {

		// if error at filtering, no need to continue
		std::cout << filter.message << std::endl << std::endl;

    } else {

	try {
		// maybe the target file already exists
		// in that case, if it already contains the filtered data, no need to overwrite it
		std::string contents;

		std::ifstream target {filename};
		if( target.is_open() ) {

		     // allocate all the memory up front
		     target.seekg(0, std::ios::end);
		     contents.reserve(target.tellg());
		     target.seekg(0, std::ios::beg);

		     // read data
		     contents.assign((std::istreambuf_iterator<char>(target)), std::istreambuf_iterator<char>());

		     // close file
		     target.close(); // maybe redundant
		}

		if( contents == filter.filtered ) {

			// no need to continue, target file contains already filtered data
			return true;
		}

		std::ofstream filterFile {filename};
		if(filterFile) {

			// write data
			filterFile.write(filter.filtered.c_str(), filter.filtered.size());

			// close file
			filterFile.close(); // maybe redundant

		} else {
			std::cout << "Error opening " << filename << std::endl;
			return false;
		}

	} catch(...) {
		std::cout << "Unexpected exception while writing to " << filename << std::endl;
		return false;
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
