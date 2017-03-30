/** @file boilerplateExample.h
	* @brief Example of automatically generated code.
	* @remark Avoid including any rapidjson header in this file.
	*
	* @bug Abuse of JSON Schema validation leads to crushes if an invalid json is queried (missing redundant checks to speed up).
	*
	*/
      #pragma once
      #include <string>
      #include <vector>
      #include <sstream>

       namespace boilerplate::Example {

struct Deal {

int at {};
double bidfloor {};
std::string bidfloorcur {};
std::string id {};
std::vector<std::string> wadomain {};
std::vector<std::string> wseat {};

}; // struct Deal

} // namespace boilerplate::Example

namespace boilerplate::Example {

struct PMP {

std::vector<Deal> deals {};
unsigned int private_auction {};

}; // struct PMP

} // namespace boilerplate::Example

namespace boilerplate::Example {

struct Native {

std::vector<unsigned int> api {};
std::string request {};
std::string ver {};

}; // struct Native

} // namespace boilerplate::Example

namespace boilerplate::Example {

struct Extension {

unsigned int strictbannersize {};

}; // struct Extension

} // namespace boilerplate::Example

namespace boilerplate::Example {

struct Impression {

// audio: not implemented
// banner: not implemented
double bidfloor {};
std::string bidfloorcur {};
unsigned int clickbrowser {};
std::string displaymanager {};
std::string displaymanagerver {};
int exp {};
struct Extension ext {};
std::string id {};
std::vector<std::string> iframabuster {};
unsigned int instl {};
struct Native native {};
struct PMP pmp {};
unsigned int secure {};
std::string tagid {};
// video: not implemented

}; // struct Impression

} // namespace boilerplate::Example

namespace boilerplate::Example {

struct Data {

std::string id {};
///@ brief Impressions rock
std::vector<Impression> imp {};
unsigned int test {};

}; // struct Data

} // namespace boilerplate::Example



// END: automatically generated code
