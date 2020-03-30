add_cmsgemos_module(
    gemhwmanagers

    SOURCES
    amc/AMCManager.cc
    amc/AMCManagerWeb.cc
    amc13/AMC13Manager.cc
    amc13/AMC13ManagerWeb.cc
    amc13/AMC13Readout.cc
    optohybrid/OptoHybridManager.cc
    optohybrid/OptoHybridManagerWeb.cc
)
target_link_libraries(gemhwmanagers PUBLIC gemhwdevices)
target_link_libraries(gemhwmanagers PUBLIC gemhwutils)
target_link_libraries(gemhwmanagers PUBLIC gemreadout)
target_link_libraries(gemhwmanagers PUBLIC CACTUS::amc13_amc13)

# install(DIRECTORY include/gem DESTINATION include COMPONENT gemhardware-devel PATTERN *.h)
