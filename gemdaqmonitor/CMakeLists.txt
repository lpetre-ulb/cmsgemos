add_cmsgemos_module(
    gemdaqmonitor
    SOURCES
    version.cc
    DaqMonitor.cc
    ShelfMonitor.cc
    ShelfMonitorWeb.cc
)

target_link_libraries(gemdaqmonitor PUBLIC gembase)
target_link_libraries(gemdaqmonitor PUBLIC gemutils)

# FIXME: HACK, HACK, HACK
# target_link_libraries(gemdaqmonitor PUBLIC xHAL::xhal_core)
target_include_directories(gemdaqmonitor PUBLIC /afs/cern.ch/user/l/lpetre/GEM/develop/local_install/opt/xhal/include)

target_link_libraries(gemdaqmonitor PUBLIC Boost::iostreams)


target_link_libraries(gemdaqmonitor PRIVATE gemhwutils)
target_link_libraries(gemdaqmonitor PRIVATE gemhwdevices)

target_link_libraries(gemdaqmonitor PRIVATE xDAQ::toolbox)
target_link_libraries(gemdaqmonitor PRIVATE xDAQ::xcept)
target_link_libraries(gemdaqmonitor PRIVATE xDAQ::xdaq)
target_link_libraries(gemdaqmonitor PRIVATE xDAQ::xoap)

target_link_libraries(gemdaqmonitor PUBLIC Boost::boost) # for Boost.Algorithm
