#!/bin/bash

clear

EXAMPLES="ex_readmisc ex_callbacks_edm_wifi"
EXAMPLES_INET="ex_http_get ex_wifi_pos"
UNITTESTS="test_at_peer test_getgeneral test_getsystem test_mock_ati9 test_send test_at_wifi test_tokenizer"
MANUALTESTS="test_ati9 test_datamode test_edm_at test_edm_at_read"

FILES="cb_tokenizer.c cb_string.c cb_at_util.c ucxh_mode.c ucxh_poll.c ucxh_wait.c ucxh_parser.c ucxh_wifi.c ucxh_edm_parser.c ucxh_send.c ucxh_edm.c ucxh_urc.c"
SERIAL="windows/ucxh_serial_win.c"
SERIAL_MOCK="../test/ucxh_serial_mock.c"
TEST_FILES="../test/test_tools.c"
INCLUDES="-I../include -I../examples -I../resources"
# CFLAGS_OPT="-Os -flto -DNDEBUG"
CFLAGS_DEBUG="-Og -g"

PARAM=""
POSITIONAL=()
while [[ $# -gt 0 ]]
do
    key="$1"

    case $key in
        --no-examples)
            NO_EXAMPLES=y
        ;;
        --no-quick-tests)
            NO_QUICK_TESTS=y
        ;;
        --no-unit-tests)
            NO_UNIT_TESTS=y
        ;;
        --no-manual-tests)
            NO_MANUAL_TESTS=y
        ;;
        --run)
            NO_DOCUMENTATION=y
            NO_QUICK_TESTS=y
            PROGRAM="$2"
            shift
            PARAM="$2"
            shift
            NO_RUN=
            RUN=y
        ;;
        --help)
            echo "To build and run all examples, tests, and generate documentation:"
            echo "  runExamples.sh [<exclude-steps>]"
            echo "    exclude-steps:"
            echo "      --no-examples"
            echo "      --no-quick-tests"
            echo "      --no-unit-tests"
            echo "      --no-manual-tests"
            echo "      --no-documentation"
            echo
            echo "To build and run a specific example or test:"
            echo "  runExamples.sh --run <name> [param]"
            echo
            echo "To only build a specific example or test:"
            echo "  runExamples.sh --build <name>"
            echo
            echo "Available examples and test:"
            ls test examples | sed -e s/\\.c//g | grep -v "\." | grep -v makefile | grep -v ucxh | grep -v ":" | sed -e "s/^/  /g"
            echo
            echo "NOTE: These examples requires an internet connection:"
            echo "        $EXAMPLES_INET"
            exit
            
        ;;
        --build)
            NO_DOCUMENTATION=y
            NO_QUICK_TESTS=y
            if [ -z "$RUN" ]
            then 
                NO_RUN=y
            fi
            PROGRAM="$2"
            shift
        ;;
        --no-documentation)
            NO_DOCUMENTATION=y
        ;;
        *)    # unknown option
            POSITIONAL+=("$1") # save it in an array for later
            shift # past argument
        ;;
    esac
   shift # past argument
done
set -- "${POSITIONAL[@]}" # restore positional parameters


echo About to run example application and tests using the u-connectXpress Host Library.
echo
echo You will need to make some configurations:
echo 
echo "* Ensure there is a NINA-W15 or NINA-W13 connected to the COM port defined"
echo "  in include/ucxh_config.h"
echo "* Ensure your AP is configured with the SSID and password defined"
echo "  in examples/ex_config.h"
echo "* Make sure you have a TCP server listening at the IP adress and port"
echo "  also defined in examples/ex_config.h"


if [ -z "$NO_EXAMPLES" ] 
then
    for i in $EXAMPLES $EXAMPLES_INET
    do
        if [ -z "$PROGRAM" ] || [ "$PROGRAM" = "$i" ]
        then
            echo
            echo Running example $i $PARAM:
            rm -f $i.exe $i.exe.stackdump
            cd src && \
            gcc $CFLAGS_DEBUG $CFLAGS_OPT $CFLAGS $INCLUDES $COVERAGE $FILES $SERIAL \
                ../examples/$i.c -o ../$i
            cd -
            if [ -z "$NO_RUN" ]
            then
                ./$i.exe $PARAM
            fi
        fi
    done
fi


if [ -z "$NO_QUICK_TESTS" ] || [ "$PROGRAM" = "test_edm" ] 
then
    echo Quick unittests:
    cd test
    make test_edm && \
    ./test_edm.exe
    cd -
fi


if [ -z "$NO_UNIT_TESTS" ]
then
    for i in $UNITTESTS
    do
        if [ -z "$PROGRAM" ] || [ "$PROGRAM" = "$i" ]
        then
            echo
            echo Unit-testing $i:
            rm -f $i.exe $i.exe.stackdump
            cd src && \
            gcc $CFLAGS_DEBUG $CFLAGS -DCONFIG_USE_MOCKED_SERIAL $INCLUDES $COVERAGE $FILES $SERIAL_MOCK $TEST_FILES \
                ../test/$i.c -o ../$i
            cd -
            if [ -z "$NO_RUN" ]
            then
                ./$i.exe
            fi
        fi
    done
fi


# Needs manual verfification and access to module
if [ -z "$NO_MANUAL_TESTS" ]
then
    for i in $MANUALTESTS
    do
        if [ -z "$PROGRAM" ] || [ "$PROGRAM" = "$i" ]
        then
            if [ -z "$WARNING_EMITTED" ]
            then 
                echo
                echo
                echo
                echo
                echo
                echo
                echo
                echo
                echo
                echo
                echo "******************************************************************************"
                echo The following tests needs manual attention in one way or another:
                echo "******************************************************************************"
                echo
                echo
                WARNING_EMITTED=y
            fi
            echo
            echo Testing $i:
            rm -f $i.exe $i.exe.stackdump
            cd src && \
            gcc $CFLAGS_DEBUG $CFLAGS $INCLUDES $COVERAGE $FILES $SERIAL $TEST_FILES \
                ../test/$i.c -o ../$i
            cd -
            if [ -z "$NO_RUN" ]
            then
                ./$i.exe
            fi
        fi
    done
fi

if [ -z "$NO_DOCUMENTATION" ]
then
    # Re-generate the documentation
    echo
    echo Generating documentation...
    # Some false positives are generated from the autogenerated.h file.
    ./mkdocs.sh 2>&1 | grep -v autogenerated.h
fi
