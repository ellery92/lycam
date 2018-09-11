add_executable(listInterface listInterface.cpp)
add_executable(genicamtest genicam/driver.cpp)

include_directories(
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        "../src"
    )

target_link_libraries(listInterface Poco::Net lycam)
target_link_libraries(genicamtest lycam Poco::CppUnit)
