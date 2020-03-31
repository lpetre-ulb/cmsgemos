add_cmsgemos_module(gemhwutils
    SOURCES
    version.cc
    GEMCrateUtils.cc
)

target_link_libraries(gemhwutils PUBLIC gemutils)

target_link_libraries(gemhwutils PUBLIC xDAQ::config)


target_link_libraries(gemhwutils PRIVATE Boost::boost) # for Boost.Algorithm
