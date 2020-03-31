add_cmsgemos_module(
    gemhwmanagers
    SOURCES
    version.cc
    amc/AMCManager.cc
    amc/AMCManagerWeb.cc
    amc13/AMC13Manager.cc
    amc13/AMC13ManagerWeb.cc
    amc13/AMC13Readout.cc
    optohybrid/OptoHybridManager.cc
    optohybrid/OptoHybridManagerWeb.cc
)

target_link_libraries(gemhwmanagers PUBLIC gembase)
target_link_libraries(gemhwmanagers PUBLIC gemhwdevices)
target_link_libraries(gemhwmanagers PUBLIC gemreadout)
target_link_libraries(gemhwmanagers PUBLIC gemutils)

target_link_libraries(gemhwmanagers PUBLIC CACTUS::uhal_uhal)
target_link_libraries(gemhwmanagers PUBLIC xDAQ::config)
target_link_libraries(gemhwmanagers PUBLIC xDAQ::toolbox)


target_link_libraries(gemhwmanagers PRIVATE gemhwutils)

target_link_libraries(gemhwmanagers PRIVATE CACTUS::amc13_amc13)
target_link_libraries(gemhwmanagers PRIVATE xDAQ::xcept)
target_link_libraries(gemhwmanagers PRIVATE xDAQ::xdaq)
target_link_libraries(gemhwmanagers PRIVATE xDAQ::xoap)
