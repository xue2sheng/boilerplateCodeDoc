# code under test
set(LIB_PATH "${CMAKE_BINARY_DIR}/src")

# boost
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_TEST_DYN_LINK")
find_package( Boost 1.54.0 REQUIRED COMPONENTS unit_test_framework )
if(Boost_FOUND)

 file(GLOB SOURCES_MAIN main.cpp)
 include_directories(../include)
 link_directories(${LIB_PATH} ${STATIC_LIB_PATH})
 add_executable(${TEST_NAME} ${SOURCES_MAIN})
 add_dependencies(${TEST_NAME} ${LIB_STATIC_NAME})
 target_link_libraries(${TEST_NAME} ${LIB_STATIC_NAME} ${STATIC_LIB_LINKER} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}) 
 add_test(UnitTest ${TEST_NAME})

 install(TARGETS ${TEST_NAME} RUNTIME DESTINATION ${TEST_INSTALL_DIR})

else()
  message(STATUS "Missing required Boost UNIT TEST framework library. Test building skipped")
endif(Boost_FOUND)

