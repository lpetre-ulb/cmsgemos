add_cmsgemos_module(
    gemdaqmonitor

    SOURCES
    DaqMonitor.cc
    ShelfMonitor.cc
    ShelfMonitorWeb.cc
    version.cc
)
target_link_libraries(gemdaqmonitor PUBLIC gembase)
target_link_libraries(gemdaqmonitor PUBLIC gemutils)
target_link_libraries(gemdaqmonitor PUBLIC gemhwutils)
target_link_libraries(gemdaqmonitor PUBLIC gemhwdevices)
# target_link_libraries(gemdaqmonitor PUBLIC xHAL::xhal_core)
target_link_libraries(gemdaqmonitor PUBLIC ${boost_IOSTREAMS_LIBRARY})
target_include_directories(gemdaqmonitor PUBLIC /afs/cern.ch/user/l/lpetre/GEM/develop/local_install/opt/xhal/include)

# install(DIRECTORY include/gem DESTINATION include COMPONENT gemdaqmonitor-devel PATTERN *.h)
