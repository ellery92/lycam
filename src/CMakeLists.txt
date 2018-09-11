
add_subdirectory(gvcp)
add_subdirectory(genicam)

file(GLOB COMMON_SRCS
  common/*.cpp)

add_library(${CMAKE_PROJECT_NAME} SHARED
  ${GVCP_SRCS}
  ${COMMON_SRCS}
  ${GENICAM_SRCS})

target_include_directories(${CMAKE_PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        3rdparty/boost_1_68_0
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_link_libraries(${CMAKE_PROJECT_NAME} Poco::Net Poco::Zip Poco::XML)
