cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# if(POLICY CMP0074)
#     cmake_policy(SET CMP0074 NEW)
# endif()

include(FeatureSummary)

project(cmsgemos)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE PATH "Installation path" FORCE)
endif()

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXDAQ15 -DGEM_VARIANT=ge11 -Dx86_64_centos7 -Dlinux")

# Dependencies
find_package(CACTUS REQUIRED amc13_amc13 uhal_uhal)
find_package(xDAQ 3.4 REQUIRED toolbox log4cplus config xcept xoap xerces-c xdaq xdata xgi cgicc i2o xdaq2rc tstore)

# https://stackoverflow.com/a/27630120
function(prepend var prefix)
    set(listVar "")
    foreach(f ${ARGN})
        list(APPEND listVar "${prefix}/${f}")
    endforeach(f)
    set(${var} "${listVar}" PARENT_SCOPE)
endfunction(prepend)

# Python
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Boost
find_package(Boost REQUIRED COMPONENTS iostreams thread filesystem unit_test_framework)

# Git version
find_package(Git)
if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --dirty --always --tags
                    OUTPUT_VARIABLE GIT_VERSION
                    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    set(GIT_VERSION "unknown")
endif()
execute_process(COMMAND id --user --name OUTPUT_VARIABLE GEMDEVELOPER
                ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGIT_VERSION=\\\"${GIT_VERSION}\\\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGEMDEVELOPER=\\\"${GEMDEVELOPER}\\\"")

function(add_cmsgemos_module name)
    cmake_parse_arguments(ARG "NO_HEADERS" "" "SOURCES;GENERATED_SOURCES" "" ${ARGN})
    set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src/common/)
    prepend(SOURCES ${SRC_DIR} ${ARG_SOURCES})

    add_library(${name} SHARED ${SOURCES} ${ARG_GENERATED_SOURCES})
    target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include )
    install(TARGETS ${name} LIBRARY DESTINATION lib COMPONENT ${name})

    # Install HTML files
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/html)
        install(DIRECTORY html DESTINATION share/cmsgemos/htdocs/${name} COMPONENT ${name})
    endif()

    # Install XML files
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/xml)
        install(DIRECTORY xml DESTINATION share/${name} COMPONENT ${name})
    endif()
endfunction()

set(COMPONENTS gemutils gembase gemhardware gemdaqmonitor gemreadout gemsupervisor gemonlinedb)

foreach(c ${COMPONENTS})
    add_subdirectory(${c})
endforeach()

feature_summary(WHAT ALL)
