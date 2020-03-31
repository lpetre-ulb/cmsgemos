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

run_parseDef("AMC13Configuration")
run_parseDef("AMCConfiguration")
run_parseDef("GBTXConfiguration")
run_parseDef("OHv3Configuration")
run_parseDef("VFAT3ChannelConfiguration")
run_parseDef("VFAT3ChipConfiguration")

#
# Main target
#
add_cmsgemos_module(gemonlinedb
    SOURCES
    version.cc
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

    GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/AMC13ConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/AMCConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/GBTXConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/OHv3ConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/VFAT3ChannelConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/VFAT3ChipConfigurationGen.cc
)

target_link_libraries(gemonlinedb PUBLIC gembase)
target_link_libraries(gemonlinedb PUBLIC gemutils)

target_link_libraries(gemonlinedb PUBLIC xDAQ::config)
target_link_libraries(gemonlinedb PUBLIC xDAQ::xdata)
target_link_libraries(gemonlinedb PUBLIC xDAQ::xerces-c)
# target_link_libraries(gemonlinedb PUBLIC xDAQ::nlohmann) FindxDAQ does not support header only libraries

target_link_libraries(gemonlinedb PUBLIC Boost::thread)


target_link_libraries(gemonlinedb PRIVATE xDAQ::toolbox)
target_link_libraries(gemonlinedb PRIVATE xDAQ::xcept)
target_link_libraries(gemonlinedb PRIVATE xDAQ::xdaq)
target_link_libraries(gemonlinedb PRIVATE xDAQ::xoap)

target_link_libraries(gemonlinedb PRIVATE Boost::filesystem)
target_link_libraries(gemonlinedb PRIVATE Boost::boost) # for Boost.Algorithm

#
# Tests
#

# Copy all schemas to the binary directory
function(copy_schema schema)
    configure_file(xml/schema/${schema}.xsd xml/schema/${schema}.xsd COPYONLY)
endfunction()
function(copy_example file)
    configure_file(xml/examples/${file} xml/examples/${file} COPYONLY)
endfunction()
function(copy_test file)
    configure_file(xml/tests/${file}.json xml/tests/${file}.json COPYONLY)
endfunction()

copy_schema(system-topology)

copy_example(AMC13_Configuration.json)
copy_example(AMC_Configuration.json)
copy_example(GBTX_Configuration.json)
copy_example(OHv3_Configuration.json)
copy_example(system-topology.xml)
copy_example(VFAT3_Channel_Configuration.json)
copy_example(VFAT3_Chip_Configuration.json)

copy_test(NonCompliantFile)

# Test validity of example files
function(test_schema_example name schema example_file)
    add_test(NAME xmllint-${name}
             COMMAND xmllint --noout --schema
                ${CMAKE_CURRENT_BINARY_DIR}/xml/schema/${schema}.xsd
                ${CMAKE_CURRENT_BINARY_DIR}/xml/examples/${example_file}.xml)
endfunction()

test_schema_example(system-topology system-topology system-topology)

# C++ tests
function(test_cpp name)
    add_executable(test${name} test/test${name}.cc)
    target_link_libraries(test${name} PRIVATE gemonlinedb)
    target_link_libraries(test${name} PRIVATE xDAQ::tstore)
    target_link_libraries(test${name} PRIVATE xDAQ::xdaq)
    target_link_libraries(test${name} PRIVATE xDAQ::xdata)
    target_link_libraries(test${name} PRIVATE xDAQ::xerces-c)
    target_link_libraries(test${name} PRIVATE Boost::unit_test_framework)
    target_link_libraries(test${name} PRIVATE Boost::filesystem)
    add_test(NAME ${name}
             COMMAND ./test${name}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

test_cpp(AMC13Configuration)
test_cpp(ConfigurationLinker)
test_cpp(ConfigurationManager)
test_cpp(FileConfigurationProvider)
test_cpp(FileUtils)
test_cpp(SerializationData)
test_cpp(SystemTopology)
test_cpp(VFAT3ChannelConfiguration)
test_cpp(VFAT3ChipConfiguration)

# # C++ tests that connect to the DB (disabled by default)
set(ENABLE_DB_TESTS FALSE
    CACHE BOOL "Build and run tests that require a working database connexion")
if(ENABLE_DB_TESTS)
    test_cpp(TStoreSerialization)
endif()
