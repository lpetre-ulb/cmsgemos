add_cmsgemos_module(gemhwutils
                    SOURCES
                    GEMCrateUtils.cc
                    version.cc
)

target_link_libraries(gemhwutils PUBLIC gemutils)
# FIXME: Boost?
