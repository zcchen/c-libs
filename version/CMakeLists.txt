function(generate_version_header version_file)
    set(options)
    set(one_value_kw MAJOR MINOR PATCH)
    set(multi_value_kw)
    cmake_parse_arguments(version "${options}"
        "${one_value_kw}" "${multi_value_kw}" ${ARGN})

    set(currdir ${CMAKE_CURRENT_SOURCE_DIR})
    set(script_file ${CMAKE_CURRENT_BINARY_DIR}/version.cmake)
    file(WRITE ${script_file} "")
    #foreach(v IN LISTS version_MAJOR version_MINOR version_PATCH)
        #file(APPEND ${script_file} "SET(${v} ${${ver}}")
    #endforeach
    file(APPEND ${script_file} "configure_file (${currdir}/version.h.in ${version_file} @ONLY)")

    #get_filename_component(currdir ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
    add_custom_command(OUTPUT ${version_file}
        COMMAND ${CMAKE_COMMAND}
            -D version_MAJOR=${version_MAJOR}
            -D version_MINOR=${version_MINOR}
            -D version_PATCH=${version_PATCH}
            -P "${script_file}"
    )
endfunction()

generate_version_header(version.h
    MAJOR 0
    MINOR 2
    PATCH 1
)


# lib
add_library(${PROJECT_NAME}-version SHARED src/version.c ${CMAKE_CURRENT_BINARY_DIR}/version.h)
set_target_properties(${PROJECT_NAME}-version PROPERTIES
    OUTPUT_NAME "version$<$<CONFIG:Debug>:${DEBUG_LIBRARY_SUFFIX}>"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION}
)
target_include_directories(${PROJECT_NAME}-version
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/
        include/
)
install(TARGETS ${PROJECT_NAME}-version
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# tests
FIND_PROGRAM(MEMORYCHECK_COMMAND valgrind )
#SET(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full" )
SET(MEMORYCHECK_COMMAND_OPTIONS --error-exitcode=1 )
SET(MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/valgrind_suppress.txt" )
FILE(GLOB src_tests tests/*.c tests/*.cpp)
FOREACH(_test_file IN LISTS src_tests)
    GET_FILENAME_COMPONENT(_prg_filename ${_test_file} NAME_WE)
    GET_FILENAME_COMPONENT(_src_filename ${_test_file} NAME)

    ADD_EXECUTABLE(test_version_${_prg_filename} ${_test_file})
    #TARGET_INCLUDE_DIRECTORIES(test_${libName}_${_prg_filename}
        #PUBLIC include/
    #)
    TARGET_LINK_LIBRARIES(test_version_${_prg_filename}
        PUBLIC ${PROJECT_NAME}-version
    )
    TARGET_COMPILE_OPTIONS(test_version_${_prg_filename} PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)
    ADD_TEST(NAME test_version_${_prg_filename}
             COMMAND test_version_${_prg_filename}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    if(NOT MEMORYCHECK_COMMAND)
        # skip the memory check.
    else()
        SET(MEMCHECK_BIN ${MEMORYCHECK_COMMAND} ${MEMORYCHECK_COMMAND_OPTIONS})
        ADD_TEST(NAME memtest_version_${_prg_filename}
                 COMMAND ${MEMCHECK_BIN} ./test_version_${_prg_filename}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
ENDFOREACH()
