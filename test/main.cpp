#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE UnitTest

#include <string>
#include <fstream>
#include <boost/hana.hpp>
#include <boost/test/unit_test.hpp>
#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>

#include "boilerplateCodeDoc.h"

namespace hana = boost::hana;
namespace boiler = boilerplateCodeDoc;

// make CTEST_OUTPUT_ON_FAILURE=1 test
// just logging something ( --log_level=message )

struct GlobalInit {
  GlobalInit() : argc(boost::unit_test_framework::framework::master_test_suite().argc),
		argv(boost::unit_test_framework::framework::master_test_suite().argv){}
  ~GlobalInit() {}
  int argc {};
  char **argv {nullptr};
};
BOOST_GLOBAL_FIXTURE( GlobalInit );

BOOST_AUTO_TEST_CASE( test000 ) {
   BOOST_TEST_MESSAGE( "\ntest000: BASIC USAGE of HANA lib" );

   using namespace hana::literals;

   struct Fish { std::string name; };
   struct Cat  { std::string name; };
   struct Dog  { std::string name; };

     // Sequences capable of holding heterogeneous objects, and algorithms
  // to manipulate them.
  auto animals = hana::make_tuple(Fish{"Nemo"}, Cat{"Garfield"}, Dog{"Snoopy"});
  auto names = hana::transform(animals, [](auto a) {
    return a.name;
  });
  BOOST_CHECK(hana::reverse(names) == hana::make_tuple("Snoopy", "Garfield", "Nemo"));

  // No compile-time information is lost: even if `animals` can't be a
  // constant expression because it contains strings, its length is constexpr.
  BOOST_CHECK(hana::length(animals) == 3u);

  // Computations on types can be performed with the same syntax as that of
  // normal C++. Believe it or not, everything is done at compile-time.
  auto animal_types = hana::make_tuple(hana::type_c<Fish*>, hana::type_c<Cat&>, hana::type_c<Dog*>);
  auto animal_ptrs = hana::filter(animal_types, [](auto a) {
    return hana::traits::is_pointer(a);
  });
  BOOST_CHECK(animal_ptrs == hana::make_tuple(hana::type_c<Fish*>, hana::type_c<Dog*>));

  // And many other goodies to make your life easier, including:
  // 1. Access to elements in a tuple with a sane syntax.
  static_assert(animal_ptrs[0_c] == hana::type_c<Fish*>, "");
  static_assert(animal_ptrs[1_c] == hana::type_c<Dog*>, "");

  // 2. Unroll loops at compile-time without hassle.
  std::string s;
  hana::int_c<10>.times([&]{ s += "x"; });
  // equivalent to s += "x"; s += "x"; ... s += "x";

  // 3. Easily check whether an expression is valid.
  //    This is usually achieved with complex SFINAE-based tricks.
  auto has_name = hana::is_valid([](auto&& x) -> decltype((void)x.name) { });
  BOOST_CHECK(has_name(animals[0_c]));
  BOOST_CHECK(!has_name(1));
}

BOOST_AUTO_TEST_CASE( test001 ) {
   BOOST_TEST_MESSAGE( "\ntest001: BASIC USAGE of RapidJSON lib" );

   using namespace rapidjson;
   using namespace std;

   struct MyHandler {
       bool Null() { BOOST_TEST_MESSAGE("Null()"); return true; }
       bool Bool(bool b) { BOOST_TEST_MESSAGE("Bool(" << boolalpha << b << ")"); return true; }
       bool Int(int i) { BOOST_TEST_MESSAGE("Int(" << i << ")"); return true; }
       bool Uint(unsigned u) { BOOST_TEST_MESSAGE("Uint(" << u << ")"); return true; }
       bool Int64(int64_t i) { BOOST_TEST_MESSAGE("Int64(" << i << ")"); return true; }
       bool Uint64(uint64_t u) { BOOST_TEST_MESSAGE("Uint64(" << u << ")"); return true; }
       bool Double(double d) { BOOST_TEST_MESSAGE("Double(" << d << ")"); return true; }
       bool RawNumber(const char* str, SizeType length, bool copy) {
	   BOOST_TEST_MESSAGE("Number(" << str << ", " << length << ", " << boolalpha << copy << ")");
	   return true;
       }
       bool String(const char* str, SizeType length, bool copy) {
	   BOOST_TEST_MESSAGE("String(" << str << ", " << length << ", " << boolalpha << copy << ")");
	   return true;
       }
       bool StartObject() { BOOST_TEST_MESSAGE("StartObject()"); return true; }
       bool Key(const char* str, SizeType length, bool copy) {
	   BOOST_TEST_MESSAGE("Key(" << str << ", " << length << ", " << boolalpha << copy << ")");
	   return true;
       }
       bool EndObject(SizeType memberCount) { BOOST_TEST_MESSAGE("EndObject(" << memberCount << ")"); return true; }
       bool StartArray() { BOOST_TEST_MESSAGE("StartArray()"); return true; }
       bool EndArray(SizeType elementCount) { BOOST_TEST_MESSAGE("EndArray(" << elementCount << ")"); return true; }
   };

   const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";

   MyHandler handler;
   Reader reader;
   StringStream ss(json);

   BOOST_CHECK(kParseErrorNone == reader.Parse(ss, handler));
}

BOOST_AUTO_TEST_CASE( test002 ) {
   BOOST_TEST_MESSAGE( "\ntest002: Transform external Json Schema into HTML");

   // taken for granted that CMake copied default json schema file in the very directory where this test binary is generated
   std::string filename{"schema.json"};
   std::string binary{boost::unit_test::framework::master_test_suite().argv[0]};
   size_t found = binary.find_last_of("/\\");
   if( found > 0 ) {
	BOOST_TEST_MESSAGE( "found=" << found);
	filename = binary.substr(0,found+1) + filename;
   }

   int argc =boost::unit_test::framework::master_test_suite().argc;
   if( argc > 1) {
	filename = std::string(boost::unit_test::framework::master_test_suite().argv[1]);
   }
   BOOST_TEST_MESSAGE( "current binary=" << binary );
   BOOST_TEST_MESSAGE( "schema.json=" << filename );

   boiler::JsonSchema jsonSchema{filename};
   BOOST_TEST_MESSAGE( "Json Schema: " << jsonSchema.message);
   boiler::JsonSchema2HTML handler {};
   bool result = handler(jsonSchema);
   BOOST_TEST_MESSAGE( "JsonSchema2HTML: " << handler.message << "\n\n" << handler.filtered);
   BOOST_CHECK( result );
}
