add_cmsgemos_module(
    gemsupervisor
    SOURCES
    version.cc
    GEMGlobalState.cc
    GEMSupervisor.cc
    GEMSupervisorMonitor.cc
    GEMSupervisorWeb.cc
)

target_link_libraries(gemsupervisor PUBLIC gembase)
target_link_libraries(gemsupervisor PUBLIC gemutils)

target_link_libraries(gemsupervisor PUBLIC xDAQ::config)
target_link_libraries(gemsupervisor PUBLIC xDAQ::toolbox)
target_link_libraries(gemsupervisor PUBLIC xDAQ::xdaq2rc)


target_link_libraries(gemsupervisor PRIVATE gemhwdevices)
target_link_libraries(gemsupervisor PRIVATE gemhwmanagers)
target_link_libraries(gemsupervisor PRIVATE gemreadout)

target_link_libraries(gemsupervisor PRIVATE xDAQ::xdata)
target_link_libraries(gemsupervisor PRIVATE xDAQ::xdaq)
target_link_libraries(gemsupervisor PRIVATE xDAQ::xcept)
target_link_libraries(gemsupervisor PRIVATE xDAQ::xoap)

target_link_libraries(gemsupervisor PRIVATE Boost::boost) # for Boost.Algorithm
