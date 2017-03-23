#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE UnitTest

#include <string>
#include <cassert>
#include <boost/test/unit_test.hpp>
#include "../include/decoupleUserOutput.h"

// make CTEST_OUTPUT_ON_FAILURE=1 test
// just logging something ( --log_level=message )

struct GlobalInit {
  GlobalInit() {}
  ~GlobalInit() {}
};
BOOST_GLOBAL_FIXTURE( GlobalInit );

BOOST_AUTO_TEST_CASE( test000 ) {
   BOOST_TEST_MESSAGE( "\ntest000: BASIC USAGE" );
}
