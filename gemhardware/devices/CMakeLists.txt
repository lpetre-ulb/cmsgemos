add_cmsgemos_module(
    gemhwdevices

    SOURCES
    GEMHwDevice.cc
    amc/HwGenericAMC.cc
    optohybrid/HwOptoHybrid.cc
)
target_link_libraries(gemhwdevices PUBLIC gemutils)
target_include_directories(gemhwdevices PUBLIC /afs/cern.ch/user/l/lpetre/GEM/develop/local_install/opt/xhal/include)
target_include_directories(gemhwdevices PUBLIC /afs/cern.ch/user/l/lpetre/GEM/develop/local_install/mnt/persistent/ctp7_modules/include)
target_link_libraries(gemhwdevices PUBLIC gemhwutils)
