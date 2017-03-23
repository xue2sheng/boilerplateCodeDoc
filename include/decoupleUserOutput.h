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

struct Fish { std::string name; };
struct Cat  { std::string name; };
struct Dog  { std::string name; };

#endif // DECOUPLEUSEROUTPUT_H

