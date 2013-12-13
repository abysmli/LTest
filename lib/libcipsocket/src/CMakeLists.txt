

SET( libSrcs 
     RtpExtHeader.cpp 
     network.cpp   )


INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} 
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../libHPTimer/
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../fastmemoplib/includes 
                     #/usr/include/libxml2)
/usr/share/gsoap)

ADD_LIBRARY (
          cipsocket
          STATIC
          ${libSrcs}
)




