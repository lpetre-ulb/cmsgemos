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
target_include_directories(gemhwdevices PUBLIC /afs/cern.ch/user/l/lpetre/GEM/develop/local_install/opt/xhal/include)
target_include_directories(gemhwdevices PUBLIC /afs/cern.ch/user/l/lpetre/GEM/develop/local_install/mnt/persistent/ctp7_modules/include)

target_link_libraries(gemhwdevices PUBLIC xDAQ::config)
target_link_libraries(gemhwdevices PUBLIC xDAQ::toolbox)


target_link_libraries(gemhwdevices PRIVATE xDAQ::xdaq)

target_link_libraries(gemhwutils PRIVATE Boost::boost) # for Boost.Algorithm
