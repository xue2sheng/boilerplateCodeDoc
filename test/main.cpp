#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE UnitTest

#include <string>
#include <boost/hana.hpp>
#include <boost/test/unit_test.hpp>
#include <string>

#include "decoupleUserOutput.h"

namespace hana = boost::hana;
using namespace hana::literals;

// make CTEST_OUTPUT_ON_FAILURE=1 test
// just logging something ( --log_level=message )

struct GlobalInit {
  GlobalInit() {}
  ~GlobalInit() {}
};
BOOST_GLOBAL_FIXTURE( GlobalInit );

BOOST_AUTO_TEST_CASE( test000 ) {
   BOOST_TEST_MESSAGE( "\ntest000: BASIC USAGE" );

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
