add_cmsgemos_module(
    gemreadout

    SOURCES
    version.cc
    GEMReadoutApplication.cc
    GEMReadoutWebApplication.cc
)
# target_link_libraries(gemreadout PUBLIC gemutils)
target_link_libraries(gemreadout PUBLIC gembase)
target_link_libraries(gemreadout PUBLIC xDAQ::i2o)

# install(DIRECTORY include/gem DESTINATION include COMPONENT gemreadout-devel PATTERN *.h)
