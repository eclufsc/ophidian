################################################################################
#   This shell script is used to compile and install dependencies of the 
# Ophidian library
################################################################################

git submodule update --init --recursive

# Absolute path to project root
OPHIDIAN_PATH=$(pwd)

# Make dependencies root path
DEPENDENCIES_ROOT=$OPHIDIAN_PATH/dependencies
mkdir $DEPENDENCIES_ROOT

DEPENDENCIES_BIN_PATH=$DEPENDENCIES_ROOT/bin
mkdir $DEPENDENCIES_BIN_PATH
DEPENDENCIES_LIB_PATH=$DEPENDENCIES_ROOT/lib
mkdir $DEPENDENCIES_LIB_PATH
DEPENDENCIES_INCLUDE_PATH=$DEPENDENCIES_ROOT/include
mkdir $DEPENDENCIES_INCLUDE_PATH

cd $OPHIDIAN_PATH/3rdparty/DEF
make CXXFLAGS="-fPIC" all
cp bin/* $DEPENDENCIES_BIN_PATH
cp lib/* $DEPENDENCIES_LIB_PATH
cp include/* $DEPENDENCIES_INCLUDE_PATH
make clean

cd $OPHIDIAN_PATH/3rdparty/LEF
make CXXFLAGS="-fPIC" all
cp bin/* $DEPENDENCIES_BIN_PATH
cp lib/* $DEPENDENCIES_LIB_PATH
cp include/* $DEPENDENCIES_INCLUDE_PATH
make clean

cd $OPHIDIAN_PATH/3rdparty/Flute
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=""
make DESTDIR=$DEPENDENCIES_ROOT install
cd ..
rm -rf build

cd $OPHIDIAN_PATH/3rdparty/verilog-parser
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=""
make DESTDIR=$DEPENDENCIES_ROOT install
cd ..
rm -rf build

cd $OPHIDIAN_PATH/3rdparty/Lemon
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=""
make DESTDIR=$DEPENDENCIES_ROOT install
cd ..
rm -rf build

cd $OPHIDIAN_PATH/3rdparty/Lemon
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/
make DESTDIR=$DEPENDENCIES_ROOT install
cd ..
rm -rf build

cd $OPHIDIAN_PATH/3rdparty/units
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/
make DESTDIR=$DEPENDENCIES_ROOT install
cd ..
rm -rf build
