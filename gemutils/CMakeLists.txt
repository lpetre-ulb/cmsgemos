add_cmsgemos_module(gemutils SOURCES
    version.cc
    Lock.cc
    # LockGuard.cc # Does not compile
    GEMRegisterUtils.cc
    soap/GEMSOAPToolBox.cc
)
target_link_libraries(gemutils PUBLIC xDAQ::xdaq)

#install(DIRECTORY include/gem DESTINATION include COMPONENT gemutils-devel PATTERN *.h)
