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

    if(UNIX)
      add_custom_command(TARGET ${target}
        POST_BUILD
        COMMAND ${target}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running ${target}" VERBATIM)
    endif()
endfunction()

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

set(lycam_SRCS
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

list(APPEND lycam_HEADERS
  src/boolean.h
  src/bootstrapregisters.h
  src/bootstrapregisters_p.h
  src/category.h
  src/command.h
  src/converter.h
  src/discoveryhelper.h
  src/enumentry.h
  src/enumeration.h
  src/floatjgv.h
  src/floatreg.h
  src/forceiphelper.h
  src/formula.h
  src/genicamxmlparser.h
  src/gvcp.h
  src/gvcpclient.h
  src/gvdevice.h
  src/headerhelper.h
  src/iboolean.h
  src/icategory.h
  src/icommand.h
  src/ienumeration.h
  src/ifloat.h
  src/iinteger.h
  src/iinterface.h
  src/inode.h
  src/integer.h
  src/intreg.h
  src/intswissknife.h
  src/iport.h
  src/iregister.h
  src/istring.h
  src/logger.h
  src/node.h
  src/nodemap.h
  src/packethelper.h
  src/readmemhelper.h
  src/readreghelper.h
  src/register.h
  src/stringreg.h
  src/swissknife.h
  src/swissknifehelper.h
  src/token.h
  src/writememhelper.h
  src/writereghelper.h
  )


add_library(${CMAKE_PROJECT_NAME} SHARED ${lycam_SRCS})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${lycam_HEADERS})

if (WIN32)
  set(BOOST_INCLUDES "" CACHE FILEPATH "Boost include path")
  if (NOT x${BOOST_INCLUDES} STREQUAL "x")
    message("include boost directories: ${BOOST_INCLUDES}")
    include_directories(${BOOST_INCLUDES})
  endif()
else()
  find_package(Boost REQUIRED)
endif()

find_package(Poco REQUIRED Foundation Net XML Zip)
target_link_libraries(lycam Poco::Foundation Poco::Net Poco::XML Poco::Zip)

add_lym_test(feature_test src/feature_test.cpp)
