#!/bin/bash

# check if stdout is a terminal...
if test -t 1; then

    # see if it supports colors...
    ncolors=$(tput colors)

    if test -n "$ncolors" && test $ncolors -ge 8; then
        bold="$(tput bold)"
        underline="$(tput smul)"
        standout="$(tput smso)"
        normal="$(tput sgr0)"
        black="$(tput setaf 0)"
        red="$(tput setaf 1)"
        green="$(tput setaf 2)"
        yellow="$(tput setaf 3)"
        blue="$(tput setaf 4)"
        magenta="$(tput setaf 5)"
        cyan="$(tput setaf 6)"
        white="$(tput setaf 7)"
    fi
fi

build_dir_prefix="./build"
build_dir=$1

if [ "$1" == "release" ];
then
    build_dir=$build_dir_prefix"/release"
fi
if [ "$1" == "debug" ] || [ "$1" == "" ];
then
    build_dir=$build_dir_prefix"/debug"
fi

echo "${cyan}${bold}Building...${normal}"
./build.sh $1
echo "${cyan}${bold}Running unit tests...${normal}"
(
	cd $build_dir
	make run-unit-tests
)
