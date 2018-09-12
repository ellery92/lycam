file(GLOB GENICAM_SRCS
  genicamxmlparser.cpp
  nodemap.cpp
  node.cpp
  reg.cpp
  integer.cpp
  intreg.cpp
  token.cpp
  formula.cpp
  intswissknife.cpp
  swissknifehelper.cpp
  floatjgv.cpp
  floatreg.cpp
  swissknife.cpp
  converter.cpp
  )

set(GENICAM_SRCS ${GENICAM_SRCS} PARENT_SCOPE)
