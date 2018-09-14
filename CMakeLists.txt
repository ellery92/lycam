cmake_minimum_required(VERSION 3.11)
project(lycam)

set(CMAKE_BUILD_TYPE Debug)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-DDEBUG)
  add_definitions(-D_DEBUG)
endif()

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#
# add_gmock_test(<target> <sources>...)
#
#  Adds a Google Mock based test executable, <target>, built from <sources> and
#  adds the test so that CTest will run it. Both the executable and the test
#  will be named <target>.
#
function(add_lym_test target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} lycam)

    add_test(${target} ${target})

    add_custom_command(TARGET ${target}
                       POST_BUILD
                       COMMAND ${target}
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                       COMMENT "Running ${target}" VERBATIM)
endfunction()

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

set(LYCAM_SRCS
  src/boolean.cpp
  src/bootstrapregisters.cpp
  src/command.cpp
  src/converter.cpp
  src/discoveryhelper.cpp
  src/floatjgv.cpp
  src/floatreg.cpp
  src/forceiphelper.cpp
  src/formula.cpp
  src/genicamxmlparser.cpp
  src/gvcpclient.cpp
  src/gvdevice.cpp
  src/headerhelper.cpp
  src/integer.cpp
  src/intreg.cpp
  src/intswissknife.cpp
  src/logger.cpp
  src/node.cpp
  src/nodemap.cpp
  src/packethelper.cpp
  src/readmemhelper.cpp
  src/readreghelper.cpp
  src/stringreg.cpp
  src/swissknife.cpp
  src/swissknifehelper.cpp
  src/token.cpp
  src/writememhelper.cpp
  src/writereghelper.cpp
  src/enumentry.cpp
  src/enumeration.cpp
  )


add_library(lycam ${LYCAM_SRCS})

find_package(Boost REQUIRED)

find_package(Poco REQUIRED Foundation Net XML Zip)
target_link_libraries(lycam Poco::Foundation Poco::Net Poco::XML Poco::Zip)

add_lym_test(feature_test src/feature_test.cpp)
