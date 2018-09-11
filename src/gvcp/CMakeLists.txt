file(GLOB GVCP_SRCS
  discoveryhelper.cpp
  forceiphelper.cpp
  headerhelper.cpp
  packethelper.cpp
  readmemhelper.cpp
  readreghelper.cpp
  writememhelper.cpp
  writereghelper.cpp
  gvdevice.cpp
  bootstrapregisters.cpp
  gvcpclient.cpp)

set(GVCP_SRCS ${GVCP_SRCS} PARENT_SCOPE)
