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

add_cmsgemos_module(gembase SOURCES
    version.cc
    GEMApplication.cc
    GEMFSMApplication.cc
    GEMFSM.cc
    GEMWebApplication.cc
    GEMMonitor.cc
    utils/GEMInfoSpaceToolBox.cc
)
target_link_libraries(gembase gemutils)
