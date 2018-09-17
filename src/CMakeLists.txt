include_directories(${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(util)
add_subdirectory(gvcp)
add_subdirectory(genicam)
# add_subdirectory(gvsp)

set(lycam_HEADERS
  ${UTIL_HEADERS}
  ${GVCP_HEADERS}
  ${GENICAM_HEADERS}
  ${GVSP_HEADERS})

set(lycam_SRCS
  ${lycam_HEADERS}
  ${UTIL_SRCS}
  ${GVCP_SRCS}
  ${GENICAM_SRCS}
  ${GVSP_SRCS})

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

if(WIN32)
  add_executable(feature_test src/feature_test.cpp ${lycam_SRCS})
  target_sources(feature_test PRIVATE ${lycam_HEADERS})
  target_link_libraries(feature_test Poco::Foundation Poco::Net Poco::XML Poco::Zip)
else()
  add_library(lycam SHARED ${lycam_SRCS})
  target_sources(lycam PRIVATE ${lycam_HEADERS})
  target_link_libraries(lycam Poco::Foundation Poco::Net Poco::XML Poco::Zip)
endif()
