#!/bin/bash


cmakebin=cmake
build_type="debug"
makeflags=""


function result {
echo
echo finished with result $1
echo
}

printhelp() {
    echo -e "--debug\t\tBuilds datapumpvx with debug symbols"
    echo -e "--release\tBuilds datapumpvx with no symbols & optimized"
    echo -e " "
    exit 0
}


try () {
  $*
  if [ $? != "0" ]; then
      echo -e "\n***ERROR in build script\nThe failed command was:\n$*\n"
      exit 1
  fi
}


configure_bin() {

    if [ ! -d "$DIR/$build_type" ]; then
            echo "try mkdir $DIR/$build_type"
            try mkdir $DIR/$build_type
    fi
    echo "try cd $DIR/$build_type"
    try cd $DIR/$build_type
echo "CC=$c_comp && CXX=$cpp_comp $cmakebin ../  -DCMAKE_BUILD_TYPE=$build_type  -D$compiler_name=TRUE -D$purify_name=TRUE $dummy $nowibu $soap_https $cmakeargs"
CC=$c_comp CXX=$cpp_comp $cmakebin ../  -DCMAKE_BUILD_TYPE=$build_type  -D$compiler_name=TRUE  $cmakeargs

}

build_bin() {
    echo "Building $build_type version with $compiler_type compiler..."
         try cd $DIR/$build_type$compiler_id$purify_id
         trybuild make $makeflags
}
trybuild () {
  $*
  if [ $? != "0" ]; then
      echo -e "Building returned an error\n"
      echo -e "Either the code failed to build properly or\n"
      echo -e "the testing programs failed to complete without\n"
      echo -e "every single test program passing the test.\n"
      exit 1
  fi
}

export LD_LIBRARY_PATH=:/usr/local/lib/
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/boost/lib:/opt/gsoap/lib:/opt/poco/lib
#echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/boost/lib:/opt/gsoap/lib:/opt/poco/lib"

#export LIBRARY_PATH=$LIBRARY_PATH:/opt/boost/lib:/opt/gsoap/lib:/opt/poco/lib
#echo "export LIBRARY_PATH=$LIBRARY_PATH:/opt/boost/lib:/opt/gsoap/lib:/opt/poco/lib"

#export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/opt/boost/include:/opt/gsoap/include:/opt/poco/include/
#echo "export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/opt/boost/include:/opt/poco/include"

export DIR=`pwd`

echo "Parsing arguments..."
while [ "$1" != "" ]; do
    case "$1" in
        --debug)
            build_type="debug";;
        --release)
            build_type="release";;
	 --cmake-args=*)
            cmakeargs=`echo $1 | cut -c 14-`;;
        -j*)
            makeflags="${makeflags} $1";;
        -?|--?|-help|--help)
            printhelp;;
        *)
            echo \`$1\' parameter ignored;;
    esac
    shift 1
done
compiler_id=""
compiler_name="GCC"
c_comp="gcc"
cpp_comp="g++"

configure_bin

build_bin


#if [ -d "build" ]; 
#	then
#	echo "build exists";
#		cd build;
#		if [ $? == 0 ]; 	
#		then 
#			rm -rf *;
#			cmake ../;
#			make;
#			result $?
#		fi
#
#	else 
#		echo "creating of build folder";
#		mkdir build;
#		cd build;
#		cmake ../;
#		make;
#		result $?	
#	fi
