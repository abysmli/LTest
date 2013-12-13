SET(libSrcs 
    my_cerrno.cpp 
    strtok.cpp
    luLogger.cpp
    SplitStreamBuf.cpp
    TimeHardware.cpp
    HighPerTimer.cpp
)

ADD_LIBRARY(
          HPTimer
          STATIC
          ${libSrcs}
)



