# Project settings for bash.
#
# Using this is optional.
# Read and understand the whole script before using.
#
# Usage: . project [profile]
# Where the optional [profile] is one of the following: dev, optimized, valgrind
#

if [ ${0:(-4)} != "bash" ]; then
    echo "Error, this script must be run using 'source' in a bash shell. For example: . project.sh"
    exit 1
fi

MYHISTFILE=$(pwd)/.bash_history
if [ "${HISTFILE}" != ${MYHISTFILE} ]; then
    echo "project.sh: Replacing bash history file location to \"${MYHISTFILE}\"."
    export HISTFILE=${MYHISTFILE}
    history -r
fi

if [ ! -d src/images ]; then
    echo "project.sh: Creating symbolic link \"src/images\" to allow running here without installing."
    ln -s ../images src/
fi

PROFILE=dev

if [ -n "${1}" ]; then
    PROFILE=${1}
fi

if [ -f Makefile ]; then
    make distclean
fi

if [ -z "${MAKEFLAGS}" ]; then
    if [ -f /proc/cpuinfo ]; then
        JOBCOUNT=$(grep -E '^processor' /proc/cpuinfo | tail -1 | awk '{print $3 + 2}')
    else
        JOBCOUNT=2
    fi
    export MAKEFLAGS="-s -j${JOBCOUNT}"
fi

profileok=1
CONFIGUREFLAGS=""
case ${PROFILE} in
    dev)
        CXXFLAGS="-g"
        LDFLAGS="-g -fdiagnostics-color=always -Og -rdynamic"
        ;;
    optimized)
        CXXFLAGS="-O3 -DNDEBUG -flto"
        LDFLAGS="-O3 -DNDEBUG -flto"
        ;;
    valgrind)
        CXXFLAGS="-O3 -g -DNDEBUG -Wno-unused"
        LDFLAGS="-g -fdiagnostics-color=always -O3 -rdynamic -DNDEBUG"
        CONFIGUREFLAGS="--with-valgrind"
        ;;
    *)
        echo "project.sh: Error, unknown profile '${PROFILE}'. Valid profiles: dev, optimized, valgrind"
        profileok=0
        ;;
esac

if [ ${profileok} -ne 0 ]; then
    echo "project.sh: Using profile: ${PROFILE}"

    CXXFLAGS+=" -Wfatal-errors -W -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align=strict -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat -Wduplicated-branches -Wduplicated-cond -Wlogical-op -Wuseless-cast -fdiagnostics-color=always"
    
    USE_SANITIZERS=0
    if [ ${USE_SANITIZERS} -ne 0 ]; then
        CXXFLAGS+=" -fsanitize-address"
        LDFLAGS+=" -fsanitize-address"
    fi
    
    CPPFLAGS+=" -I/usr/local/include"
    
    export CPPFLAGS
    export CXXFLAGS
    export LDFLAGS
    

    autoreconf --include=m4 --install
    ./configure --prefix=$(pwd) --disable-shared ${CONFIGUREFLAGS}
    #./configure ${CONFIGUREFLAGS}
fi
