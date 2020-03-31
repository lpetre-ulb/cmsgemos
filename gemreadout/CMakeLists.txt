add_cmsgemos_module(
    gemreadout
    SOURCES
    version.cc
    GEMReadoutApplication.cc
    GEMReadoutWebApplication.cc
)

target_link_libraries(gemreadout PUBLIC gembase)
target_link_libraries(gemreadout PUBLIC gemutils)

target_link_libraries(gemreadout PUBLIC xDAQ::config)
target_link_libraries(gemreadout PUBLIC xDAQ::i2o)
target_link_libraries(gemreadout PUBLIC xDAQ::toolbox)
target_link_libraries(gemreadout PUBLIC xDAQ::xoap)

target_link_libraries(gemreadout PRIVATE xDAQ::xcept)
target_link_libraries(gemreadout PRIVATE xDAQ::xdaq)
