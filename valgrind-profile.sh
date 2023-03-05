#!/bin/bash

TOOL=callgrind
#if [ -n "${1}" ]; then
#    TOOL=${1}
#fi

if [ -z "${1}" -o ! -f "${1}" ]; then
    echo "File '${1}' not found error."
    exit 1
fi

case ${TOOL} in
    
    callgrind)
        OPTIONS="--time-stamp=yes --dump-instr=yes --collect-jumps=yes --collect-systime=nsec"
        #OPTIONS+=" --track-fds=yes --collect-bus=yes --branch-sim=yes --cache-sim=yes"
        ;;

    *)
        echo "${0}: Error, unknown tool '${TOOL}'."
        exit 1
        ;;
esac

make || exit 1
/usr/bin/time valgrind --tool=${TOOL} ${OPTIONS} ./src/fmsbench ${1} || exit 1
