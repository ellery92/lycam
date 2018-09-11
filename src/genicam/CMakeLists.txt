file(GLOB GENICAM_SRCS
  genicamxmlparser.cpp
  nodemap.cpp
  node.cpp
  integer.cpp
  # floatjgv.cpp
  token.cpp
  formula.cpp)

set(GENICAM_SRCS ${GENICAM_SRCS} PARENT_SCOPE)
