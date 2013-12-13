CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

#this project named GsoapLtest, for testing internet performance 
PROJECT ( Ltest )

SET ( CMAKE_COLOR_MAKEFILE ON )
SET ( CMAKE_VERBOSE_MAKEFILE ON )

SET(GSOAP_DIR "/opt/gsoap")
INCLUDE_DIRECTORIES( ${GSOAP_DIR}/include )
LINK_DIRECTORIES( ${GSOAP_DIR}/lib )

SET (CMAKE_CXX_FLAGS "-Wall -g3 -O0 -std=c++0x -static-libgcc -static-libstdc++ ")

ADD_SUBDIRECTORY ( lib/libcipsocket/src )
ADD_SUBDIRECTORY ( lib/libHPTimer/)
ADD_SUBDIRECTORY ( src )

#-Xlinker -zmuldefs
