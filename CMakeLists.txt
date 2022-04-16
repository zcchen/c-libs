cmake_minimum_required(VERSION 3.13)

SET(DEBUG TRUE)
SET(RELEASE FALSE)

project( c_toolings
        VERSION 0.1.0
        LANGUAGES C
    )

# ------------------ Compiler Settings ---------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (CMAKE_C_COMPILER_ID MATCHES "GNU")
    add_definitions(-D_POSIX_C_SOURCE=200112L)
    add_definitions(-D_DEFAULT_SOURCE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
# --------------- End of Compiler Settings -----------------


######################## Path Settings ###############################
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")
######################################################################

######################## Compiler Flags Settings #####################
IF (${DEBUG} AND ${RELEASE})
    MESSAGE(FATAL_ERROR "Flag DEBUG & RELEASE could not be both TRUE.")
ELSEIF (${DEBUG} AND NOT ${RELEASE})
    STRING(APPEND CMAKE_C_FLAGS " ${CMAKE_C_FLAGS_DEBUG}")
ELSEIF (NOT ${DEBUG} AND ${RELEASE})
    STRING(APPEND CMAKE_C_FLAGS " ${CMAKE_C_FLAGS_RELEASE}")
ENDIF ()
######################################################################

################ Options you will add here ###########################
######################################################################

######################## Building Dependanies Settings ###############
#INCLUDE(cmake/cpack.cmake)
#FIND_PACKAGE(Doxygen REQUIRED dot)
#SET(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
#SET(doxyfile ${PROJECT_SOURCE_DIR}/Doxyfile)
#SET(doxyfile_in ${PROJECT_SOURCE_DIR}/Doxyfile.in)
#CONFIGURE_FILE(${doxyfile_in} ${doxyfile} @ONLY)
#ADD_CUSTOM_TARGET(docs
                  #COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                  #WORKING_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY}
                  #COMMENT "Generating API documentation with Doxygen"
                  #DEPENDS build_docs_dirs
                  #VERBATIM)
#ADD_CUSTOM_TARGET(build_docs_dirs
                  #COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIRECTORY})
######################################################################

######################## Running Dependanies Settings ################
######################################################################
INCLUDE(CTest)
ADD_SUBDIRECTORY(ringbuf)
ADD_SUBDIRECTORY(enumkv)
ADD_SUBDIRECTORY(serialhex2struct)
ADD_SUBDIRECTORY(class)
ADD_SUBDIRECTORY(object)
ADD_SUBDIRECTORY(endian)
ADD_SUBDIRECTORY(dataframes)
ADD_SUBDIRECTORY(chain)

