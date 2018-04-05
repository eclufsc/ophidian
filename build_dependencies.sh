################################################################################
#   This shell script is used to compile and install dependencies of the 
# Ophidian library
################################################################################

# Make shell stop at any command that not return 0
set -e

# Fetch git submodules
git submodule update --init --recursive

# Get script directory
CURRENT_DIR=$(pwd)
SCRIPT=$(readlink -f "$0")
SOURCE_ROOT=$(dirname "$SCRIPT")

# Set default Dependencies Root
DEPENDENCIES_ROOT=$SOURCE_ROOT/dependencies

# Check if user wants to install elsewhere
AUX=$2
if [ "$1" = "--install_to" ] && [ "${AUX:0:1}" != "/" ]
then
    DEPENDENCIES_ROOT=$CURRENT_DIR/$2
elif [ "$1" = "--install_to" ] && [ "${AUX:0:1}" = "/" ]
then
    DEPENDENCIES_ROOT=$2
fi

# Check if user wants to install elsewhere
AUX=$2
if [ "$1" = "--install_to" ] && [ "${AUX:0:1}" != "/" ]
then
    DEPENDENCIES_ROOT=$CURRENT_DIR/$2
elif [ "$1" = "--install_to" ] && [ "${AUX:0:1}" = "/" ]
then
    DEPENDENCIES_ROOT=$2
fi

# Useful variables
DEPENDENCIES_BIN_PATH=$DEPENDENCIES_ROOT/bin
DEPENDENCIES_LIB_PATH=$DEPENDENCIES_ROOT/lib
DEPENDENCIES_INCLUDE_PATH=$DEPENDENCIES_ROOT/include
DEPENDENCIES_SHARE_PATH=$DEPENDENCIES_ROOT/share

# isntall dependencies directory
echo "Install dependencies to: ${DEPENDENCIES_ROOT}"
while true; do
    read -p "Do you wish to continue? (y/n):  " yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer y (yes) or n (no).";;
    esac
done

install_DEF()
{
    echo "installing DEF"
    cd $SOURCE_ROOT/3rdparty/DEF
    make CXXFLAGS="-fPIC -O3 -w" all
    install -D -m 644 bin/* $DEPENDENCIES_BIN_PATH
    install -D -m 644 lib/* $DEPENDENCIES_LIB_PATH
    install -D -m 644 include/* $DEPENDENCIES_INCLUDE_PATH
    make clean
}

install_LEF()
{
    echo "installing LEF"
    cd $SOURCE_ROOT/3rdparty/LEF
    make CXXFLAGS="-fPIC -O3 -w" all
    install -D -m 644 bin/* $DEPENDENCIES_BIN_PATH
    install -D -m 644 lib/* $DEPENDENCIES_LIB_PATH
    install -D -m 644 include/* $DEPENDENCIES_INCLUDE_PATH
    make clean
}

install_FLUTE()
{
    echo "installing Flute"
    cd $SOURCE_ROOT/3rdparty/Flute
    mkdir build
    cd build
    cmake -DCMAKE_CXX_FLAGS="-w" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="" ..
    make DESTDIR=$DEPENDENCIES_ROOT install
    cd ..
    rm -rf build
}

install_VERILOG_PARSER()
{
    echo "installing verilog-parser"
    cd $SOURCE_ROOT/3rdparty/verilog-parser
    mkdir build
    cd build
    cmake -DCMAKE_C_FLAGS="-w" -DCMAKE_CXX_FLAGS="-w" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="" ..
    make DESTDIR=$DEPENDENCIES_ROOT install
    cd ..
    rm -rf build
}

install_LEMON()
{
    echo "installing lemon"
    cd $SOURCE_ROOT/3rdparty/Lemon
    mkdir build_static
    cd build_static
    cmake -DCMAKE_CXX_FLAGS="-w" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="" ..
    make DESTDIR=$DEPENDENCIES_ROOT install
    cd ..
    rm -rf build_static
    mkdir build_shared
    cd build_shared
    cmake -DCMAKE_CXX_FLAGS="-w" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="" -DBUILD_SHARED_LIBS=TRUE ..
    make DESTDIR=$DEPENDENCIES_ROOT install
    cd ..
    rm -rf build_shared
}

install_UNITS()
{
    echo "installing units"
    cd $SOURCE_ROOT/3rdparty/units
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="" ..
    make DESTDIR=$DEPENDENCIES_ROOT install
    cd ..
    rm -rf build
}

install_LIBERTY_PARSER()
{
    echo "installing libertyParser"
    cd $SOURCE_ROOT/3rdparty/libertyParser
    make CXXFLAGS="-fPIC -O3 -w" all
    install -D -m 644 bin/* $DEPENDENCIES_BIN_PATH
    install -D -m 644 lib/* $DEPENDENCIES_LIB_PATH
    install -D -m 644 include/* $DEPENDENCIES_INCLUDE_PATH
    make clean
}

run_install()
{
    install -d $DEPENDENCIES_ROOT
    install -d $DEPENDENCIES_BIN_PATH
    install -d $DEPENDENCIES_LIB_PATH
    install -d $DEPENDENCIES_INCLUDE_PATH

    echo "Starting installation"
    install_DEF
    install_LEF
    install_FLUTE
    install_VERILOG_PARSER
    install_LEMON
    install_UNITS
    install_LIBERTY_PARSER
}

run_install
