include_directories (
${CMAKE_CURRENT_BINARY_DIR}/
${CMAKE_CURRENT_SOURCE_DIR}/../lib/libHPTimer/
${CMAKE_CURRENT_SOURCE_DIR}/../lib/libcipsocket/src
) 

SET(ltestSrcs
    Ltest.cpp
    Client.cpp
    Server.cpp
    config.cpp
    cstats.cpp
    DataSource.cpp
    SessionIf_impl.cpp
    TimeCorrect.cpp
    RingBuffer.cpp
    SecurityCheck.cpp
    WebGUI.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/soapltestService.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/soapltestProxy.cpp 
)

# soap stl wrappers includes
# pls adapt to your gsoap installation dir
SET(GSOAP_DIR "/opt/gsoap")
SET(GSOAP_COMMAND soapcpp2)
# set gsoap flags for command
SET(GSOAP_FLAGS -j -I${GSOAP_DIR}/share/gsoap/import:import)

# generate gsoap stab and proxies, output soapC
ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp
                   COMMAND ${GSOAP_COMMAND} ${GSOAP_FLAGS}
                   ${CMAKE_CURRENT_SOURCE_DIR}/ltestgsoap.h 
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/soapltestService.cpp
                   ${CMAKE_CURRENT_BINARY_DIR}/soapltestProxy.cpp 
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   COMMENT "generating soapC.{cpp h}"
)

# generate gsoap stab and proxies, output soapltestService
ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/soapltestService.cpp
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   COMMENT "generating soapltestService.{cpp h}"
)

# generate gsoap stab and proxies, output soapltestProxy
ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/soapltestProxy.cpp
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   COMMENT "generating soapltestProxy.{cpp h}"
)

# print out debug informations
#ADD_DEFINITIONS(-DDEBUG_OUTPUT)

SET( WT_LIBRARY_DEPENDENCE
  GraphicsMagick.a
  jbig.a
  lzma.a
  Xau.a
  Xdmcp.a
  ltdl.a
  wmflite.a
  lcms.a
  gomp.a
  jasper.a
  jpeg.a
  png12.a
  hpdfs.a
  tiff.a
  Xext.a
  X11.a
  bz2.a
  xcb.a
  freetype.a
  xml2.a
  lzma.a
  icuuc.a
  icudata.a
  boost_random.a
  boost_regex.a
  boost_signals.a
  boost_system.a
  boost_thread.a
  boost_filesystem.a
  boost_program_options.a
  boost_date_time.a
  ssl.a
  crypto.a 
  z.a
#  gobject-2.0.a
#  glib-2.0.a
#  ffi.a
#  pcre.a
#  fontconfig.a
#  expat.a
#  gmodule-2.0.a
#  thai.a
#  datrie.a
#  icule.a
#  rt.a
#  pango-1.0.a
)
    
ADD_EXECUTABLE( ltest ${ltestSrcs} )

TARGET_LINK_LIBRARIES( ltest 
                       cipsocket
                       HPTimer
                       gsoap++.a
                       wthttp.a
                       wt.a
                       ${WT_LIBRARY_DEPENDENCE}
                       dl
                       pthread
                     )
                    
