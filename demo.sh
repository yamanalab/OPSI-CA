#!/bin/bash

BINDIR=`pwd`/build/demo
SRCDIR=`pwd`/demo
TMPDIR=/tmp/opsica
KEYOPT=-g

while getopts s OPT; do
    case $OPT in
	"s" ) KEYOPT="" ;;
    esac
done

CONFIG=${SRCDIR}/querier/config.txt
INFILE_A=${SRCDIR}/dataownerA/datasetA2_1.txt
INFILE_B=${SRCDIR}/dataownerB/datasetB2_1.txt
OUTDIR_A=${TMPDIR}/A
OUTDIR_B=${TMPDIR}/B

if [ ! -e ${OUTDIR_A} ]; then mkdir -p ${OUTDIR_A}; fi
if [ ! -e ${OUTDIR_B} ]; then mkdir -p ${OUTDIR_B}; fi

xterm -T "cloud"      -e "/bin/bash -c 'cd ${BINDIR}/cloud && ./cloud; exec /bin/bash -i'"&
xterm -T "dataownerA" -e "/bin/bash -c 'cd ${BINDIR}/dataownerA && ./dataownerA -i ${INFILE_A} -o ${OUTDIR_A}; exec /bin/bash -i'"&
xterm -T "dataownerB" -e "/bin/bash -c 'cd ${BINDIR}/dataownerB && ./dataownerB -i ${INFILE_B} -o ${OUTDIR_B}; exec /bin/bash -i'"&
xterm -T "querier"    -e "/bin/bash -c 'cd ${BINDIR}/querier && ./querier ${KEYOPT} -c ${CONFIG}; exec /bin/bash -i'"&
