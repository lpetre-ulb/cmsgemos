add_cmsgemos_module(
    gemsupervisor

    SOURCES
    version.cc
    GEMSupervisor.cc
    GEMSupervisorWeb.cc
    GEMSupervisorMonitor.cc
    GEMGlobalState.cc
)
target_link_libraries(gemsupervisor PUBLIC gemhwdevices)
target_link_libraries(gemsupervisor PUBLIC gemhwmanagers)
target_link_libraries(gemsupervisor PUBLIC gemreadout)
target_link_libraries(gemsupervisor PUBLIC xDAQ::xdaq2rc)

# install(DIRECTORY include/gem DESTINATION include COMPONENT gemsupervisor-devel PATTERN *.h)
