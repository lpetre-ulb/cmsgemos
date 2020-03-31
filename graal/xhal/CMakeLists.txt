add_library(xhal-common SHARED
    src/common/rpc/exceptions.cpp
    src/common/utils/XHALXMLParser.cpp
)
target_include_directories(xhal-common PUBLIC include)
target_link_libraries(xhal-common PUBLIC xDAQ::log4cplus)
target_link_libraries(xhal-common PUBLIC xDAQ::xerces-c)
target_link_libraries(xhal-common PUBLIC Boost::boost) # for Boost.Optional


add_library(xhal-client SHARED
    src/client/XHALInterface.cpp
)
target_include_directories(xhal-client PUBLIC include)
target_link_libraries(xhal-client PUBLIC xhal-common)
target_link_libraries(xhal-client PUBLIC xDAQ::log4cplus)
