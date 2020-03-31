add_cmsgemos_module(gembase
    SOURCES
    version.cc
    GEMApplication.cc
    GEMFSM.cc
    GEMFSMApplication.cc
    GEMMonitor.cc
    GEMWebApplication.cc
    utils/GEMInfoSpaceToolBox.cc
)

target_link_libraries(gembase PUBLIC gemutils)

target_link_libraries(gembase PUBLIC xDAQ::cgicc)
target_link_libraries(gembase PUBLIC xDAQ::log4cplus)
target_link_libraries(gembase PUBLIC xDAQ::toolbox)
target_link_libraries(gembase PUBLIC xDAQ::xcept)
target_link_libraries(gembase PUBLIC xDAQ::xdaq)
target_link_libraries(gembase PUBLIC xDAQ::xdata)
target_link_libraries(gembase PUBLIC xDAQ::xgi)
target_link_libraries(gembase PUBLIC xDAQ::xoap)

target_link_libraries(gembase PUBLIC Boost::boost) # for Boost.Assignment

# Create a directory to be used as XDAQ_DOCUMENT_ROOT
# file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/htdocs)
# file(GLOB htdocs_subdirs
#      LIST_DIRECTORIES TRUE
#      RELATIVE ${xDAQ_HTML_DIR}
#      CONFIGURE_DEPENDS
#      ${xDAQ_HTML_DIR}/*)
# foreach(subdir IN LISTS htdocs_subdirs)
#     add_custom_target(
#         htdocs-symlink-${subdir}
#         ALL cmake -E create_symlink ${xDAQ_HTML_DIR}/${subdir} htdocs/${subdir})
# endforeach()
# install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/htdocs
#         DESTINATION share/cmsgemos
#         COMPONENT gembase)
