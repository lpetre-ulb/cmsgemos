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

target_link_libraries(gemdaqmonitor PUBLIC xhal-client)

target_link_libraries(gemdaqmonitor PUBLIC Boost::iostreams)


target_link_libraries(gemdaqmonitor PRIVATE gemhwutils)
target_link_libraries(gemdaqmonitor PRIVATE gemhwdevices)

target_link_libraries(gemdaqmonitor PRIVATE xDAQ::toolbox)
target_link_libraries(gemdaqmonitor PRIVATE xDAQ::xcept)
target_link_libraries(gemdaqmonitor PRIVATE xDAQ::xdaq)
target_link_libraries(gemdaqmonitor PRIVATE xDAQ::xoap)

target_link_libraries(gemdaqmonitor PUBLIC Boost::boost) # for Boost.Algorithm
