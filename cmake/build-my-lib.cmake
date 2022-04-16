function(make_mylib_folder libName)
    set(libType SHARED)
    # file settings
    file(GLOB src_files src/*.c src/*.*.cpp)
    FILE(GLOB src_tests tests/*.c tests/*.cpp)

    # headers
    INSTALL(DIRECTORY include/
            DESTINATION include/${libName}
            FILES_MATCHING PATTERN "*.h"
    )

    # library
    add_library(${PROJECT_NAME}-${libName} ${libType} ${src_files})
    set_target_properties(${PROJECT_NAME}-${libName} PROPERTIES
        OUTPUT_NAME "${libName}$<$<CONFIG:Debug>:${DEBUG_LIBRARY_SUFFIX}>"
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION}
        #BUILD_WITH_INSTALL_RPATH ON
        #LINK_FLAGS "-Wl,-rpath,$ORIGIN/../lib"
        #LINK_FLAGS "-Wl,-rpath,$ORIGIN/"
    )
    target_include_directories(${PROJECT_NAME}-${libName}
        PRIVATE src/
        PUBLIC include/
    )
    target_link_libraries(${PROJECT_NAME}-${libName} PUBLIC
        ${ARGN}
    )
    install(TARGETS ${PROJECT_NAME}-${libName}
            RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
            LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
            ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    )

    # tests
    FIND_PROGRAM(MEMORYCHECK_COMMAND valgrind )
    #SET(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full" )
    SET(MEMORYCHECK_COMMAND_OPTIONS --error-exitcode=1 )
    SET(MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/valgrind_suppress.txt" )
    FOREACH(_test_file IN LISTS src_tests)
        GET_FILENAME_COMPONENT(_prg_filename ${_test_file} NAME_WE)
        GET_FILENAME_COMPONENT(_src_filename ${_test_file} NAME)

        ADD_EXECUTABLE(test_${libName}_${_prg_filename} ${_test_file})
        TARGET_INCLUDE_DIRECTORIES(test_${libName}_${_prg_filename}
            PUBLIC include/
        )
        TARGET_LINK_LIBRARIES(test_${libName}_${_prg_filename}
            PUBLIC ${PROJECT_NAME}-${libName}
        )
        TARGET_COMPILE_OPTIONS(test_${libName}_${_prg_filename} PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)
        ADD_TEST(NAME test_${libName}_${_prg_filename}
                 COMMAND test_${libName}_${_prg_filename}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        if(NOT MEMORYCHECK_COMMAND)
            # skip the memory check.
        else()
            SET(MEMCHECK_BIN ${MEMORYCHECK_COMMAND} ${MEMORYCHECK_COMMAND_OPTIONS})
            ADD_TEST(NAME memtest_${libName}_${_prg_filename}
                     COMMAND ${MEMCHECK_BIN} ./test_${libName}_${_prg_filename}
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            )
        endif()
    ENDFOREACH()

endfunction()
