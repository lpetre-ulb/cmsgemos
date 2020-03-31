add_cmsgemos_module(
    gemhwdevices
    SOURCES
    version.cc
    GEMHwDevice.cc
    amc/HwGenericAMC.cc
    optohybrid/HwOptoHybrid.cc
)

target_link_libraries(gemhwdevices PUBLIC gemhwutils)
target_link_libraries(gemhwdevices PUBLIC gemutils)

# FIXME: HACK, HACK, HACK
target_link_libraries(gemhwdevices PUBLIC xhal-client)
target_link_libraries(gemhwdevices PUBLIC ctp7_modules)

target_link_libraries(gemhwdevices PUBLIC xDAQ::config)
target_link_libraries(gemhwdevices PUBLIC xDAQ::toolbox)


target_link_libraries(gemhwdevices PRIVATE xDAQ::xdaq)

target_link_libraries(gemhwutils PRIVATE Boost::boost) # for Boost.Algorithm
