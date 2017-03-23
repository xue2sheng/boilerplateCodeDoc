#include <boost/hana.hpp>
#include <rapidjson/rapidjson.h>
#include <cassert>
#include <string>

// automatically generate version info
#include "version.h"

#include "decoupleUserOutput.h"

namespace decouple = decoupleUserOutput;

decouple::ParseErrorCode Parse(const char* const filename, decouple::JsonHandler& handler) {

	return decoupleUserOutput::ParseErrorCode::OK;
}
