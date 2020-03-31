project(graal)

add_subdirectory(xhal)
add_subdirectory(ctp7_modules)

if(NOT "${PARENT_DIRECTORY}" STREQUAL "")
    # We are built as a sub-project
    # Only the client libraries should be built
    return()
endif()
