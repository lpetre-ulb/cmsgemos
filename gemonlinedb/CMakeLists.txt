#
# Run parseDef.py to generate configuration classes
#
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/gem/onlinedb/detail)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/common)

function(run_parseDef file)
    add_custom_command(OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/include/gem/onlinedb/detail/${file}Gen.h
        ${CMAKE_CURRENT_BINARY_DIR}/src/common/${file}Gen.cc
        COMMAND ${Python2_EXECUTABLE}
            ${CMAKE_CURRENT_LIST_DIR}/parseDef.py
            ${CMAKE_CURRENT_LIST_DIR}/defs/${file}.json
        DEPENDS parseDef.py defs/${file}.json
        COMMENT "Running parseDef.py: ${file}"
    )
endfunction(run_parseDef)

run_parseDef("AMCConfiguration")
run_parseDef("AMC13Configuration")
run_parseDef("OHv3Configuration")
run_parseDef("GBTXConfiguration")
run_parseDef("VFAT3ChipConfiguration")
run_parseDef("VFAT3ChannelConfiguration")

#
# Main target
#
add_cmsgemos_module(gemonlinedb SOURCES
    AMC13Configuration.cc
    AMCConfiguration.cc
    ConfigurationLinker.cc
    ConfigurationManager.cc
    DBInterface.cc
    FileConfigurationProvider.cc
    FileUtils.cc
    GEMOnlineDBManager.cc
    GEMOnlineDBManagerWeb.cc
    OHv3Configuration.cc
    Run.cc
    SystemTopology.cc
    XMLUtils.cc
    version.cc

    GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/AMC13ConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/AMCConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/OHv3ConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/GBTXConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/VFAT3ChipConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/VFAT3ChannelConfigurationGen.cc
)
target_link_libraries(gemonlinedb gembase)
target_link_libraries(gemonlinedb xDAQ::tstore)
target_link_libraries(gemonlinedb ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries(gemonlinedb ${Boost_SYSTEM_LIBRARY})

#
# Tests
#

# Copy all schemas to the binary directory
# function(copy_schema schema)
#     configure_file(xml/schema/${schema}.xsd xml/schema/${schema}.xsd COPYONLY)
# endfunction()
# function(copy_example_xml file)
#     configure_file(xml/examples/${file}.xml xml/examples/${file}.xml COPYONLY)
# endfunction()

# copy_schema(common)
# copy_schema(AMC13Configuration)
# copy_schema(system-topology)
# copy_schema(VFAT3ChannelConfiguration)

# copy_example_xml(AMC13_Configuration)
# copy_example_xml(AMC_Configuration)
# copy_example_xml(OHv3_Configuration)
# copy_example_xml(system-topology)
# copy_example_xml(VFAT3_Channel_Configuration)
# copy_example_xml(VFAT3_Chip_Configuration)

# Test validity of example files
# function(test_schema_example name schema example_file)
#     add_test(NAME xmllint-${name}
#              COMMAND xmllint --noout --schema
#                 ${CMAKE_CURRENT_BINARY_DIR}/xml/schema/${schema}.xsd
#                 ${CMAKE_CURRENT_BINARY_DIR}/xml/examples/${example_file}.xml)
# endfunction()

# test_schema_example(amc13 AMC13Configuration AMC13_Configuration)
# test_schema_example(amc AMCConfiguration AMC_Configuration)
# test_schema_example(oh OHv3Configuration OHv3_Configuration)
# test_schema_example(vfat VFAT3ChipConfiguration VFAT3_Chip_Configuration)

# C++ tests
# function(test_cpp name)
#     add_executable(test${name} test/test${name}.cc)
#     target_link_libraries(test${name} gemonlinedb)
#     target_link_libraries(test${name} ${Boost_SYSTEM_LIBRARY})
#     target_link_libraries(test${name} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
#     add_test(NAME ${name}
#              COMMAND ./test${name}
#              WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
# endfunction()

# test_cpp(AMC13Configuration)
# test_cpp(ConfigurationLinker)
# test_cpp(FileUtils)
# test_cpp(SystemTopology)
# test_cpp(VFAT3ChannelConfiguration)
# test_cpp(XMLConfigurationProvider)
# test_cpp(XMLSerialization)

# # C++ tests that connect to the DB (disabled by default)
# set(ENABLE_DB_TESTS FALSE
#     CACHE BOOL "Build and run tests that require a working database connexion")
# if(ENABLE_DB_TESTS)
#     test_cpp(TStoreSerialization)
# endif()
