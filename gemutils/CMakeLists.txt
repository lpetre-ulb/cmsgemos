add_cmsgemos_module(gemutils
    SOURCES
    version.cc
    GEMRegisterUtils.cc
    Lock.cc
    soap/GEMSOAPToolBox.cc
)

target_link_libraries(gemutils PUBLIC xDAQ::config)
target_link_libraries(gemutils PUBLIC xDAQ::log4cplus)
target_link_libraries(gemutils PUBLIC xDAQ::toolbox)
target_link_libraries(gemutils PUBLIC xDAQ::xcept)
target_link_libraries(gemutils PUBLIC xDAQ::xdaq)
target_link_libraries(gemutils PUBLIC xDAQ::xdata)
target_link_libraries(gemutils PUBLIC xDAQ::xerces-c)
target_link_libraries(gemutils PUBLIC xDAQ::xoap)
