

FILE(READ license.txt LICENCE_TXT)
STRING(REGEX REPLACE "([^\n]*\n)" "// \\1" LICENCE_TXT_WITH_SLASH "${LICENCE_TXT}")
STRING(REGEX REPLACE "([^\n]*\n)" "# \\1" LICENCE_TXT_WITH_SHARP "${LICENCE_TXT}")

# Definitions
GET_DIRECTORY_PROPERTY(USE_SMIL_DEFINITIONS DEFINITIONS)
LIST(REMOVE_DUPLICATES USE_SMIL_DEFINITIONS)
STRING(REGEX MATCHALL "-D[^ \n]*" USE_SMIL_DEFINES ${USE_SMIL_DEFINITIONS})


# Include directories
GET_DIRECTORY_PROPERTY(USE_SMIL_INCLUDE_DIRS INCLUDE_DIRECTORIES)
LIST(REMOVE_DUPLICATES USE_SMIL_INCLUDE_DIRS)
# Remove build includes
FOREACH(DIR ${USE_SMIL_INCLUDE_DIRS})
	STRING(REGEX MATCH "${PROJECT_SOURCE_DIR}.*|${PROJECT_BINARY_DIR}.*" IS_SMIL_DIR ${DIR})
	IF(IS_SMIL_DIR)
	      LIST(REMOVE_ITEM USE_SMIL_INCLUDE_DIRS ${DIR})
	ENDIF()
ENDFOREACH()
LIST(INSERT USE_SMIL_INCLUDE_DIRS 0 ${SMIL_HEADERS_INSTALL_PATH})


# Libraries
SET(USE_SMIL_LIBS ${SMIL_LIBS})
SET(USE_SMIL_LIB_DIRS ${SMIL_LIBRARIES_INSTALL_PATH})
FOREACH(LIB ${SMIL_EXT_DEPS})
	GET_FILENAME_COMPONENT(LIB_NAME ${LIB} NAME_WE)
	GET_FILENAME_COMPONENT(LIB_DIR ${LIB} PATH)
	IF (NOT WIN32 OR NOT MSVC)
            STRING(REGEX REPLACE "^${CMAKE_FIND_LIBRARY_PREFIXES}" "" LIB_NAME ${LIB_NAME}) # remove lib prefix to lib name
        ENDIF (NOT WIN32 OR NOT MSVC)
	LIST(APPEND USE_SMIL_LIBS "${LIB_NAME}")
	LIST(APPEND USE_SMIL_LIB_DIRS "${LIB_DIR}")
ENDFOREACH()
LIST(REMOVE_DUPLICATES USE_SMIL_LIB_DIRS)
LIST(REMOVE_ITEM USE_SMIL_LIBS "debug" "optimized")



# Generate main header Smil.h
SET(MAIN_HEADER_STR "${LICENCE_TXT_WITH_SLASH}\n")
LIST(APPEND MAIN_HEADER_STR "#ifndef SMIL_GLOBAL_HEADER\n#define SMIL_GLOBAL_HEADER\n\n\n")

FOREACH(DEF ${USE_SMIL_DEFINES})
	STRING(REGEX REPLACE "-D(.*)" "\\1" DEF ${DEF})
	STRING(REGEX REPLACE "=.*" "" EMPTY_DEF ${DEF})
	STRING(REPLACE "=" " " DEF ${DEF})
	LIST(APPEND MAIN_HEADER_STR "#ifndef ${EMPTY_DEF}\n#define ${DEF}\n#endif\n")
ENDFOREACH()

LIST(APPEND MAIN_HEADER_STR "\n")
FOREACH(HEADER ${SMIL_GLOBAL_HEADERS})
	LIST(APPEND MAIN_HEADER_STR "#include \"${HEADER}\"\n")
ENDFOREACH()
LIST(APPEND MAIN_HEADER_STR "\n#endif\n")
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Smil.h ${MAIN_HEADER_STR})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/Smil.h 
      DESTINATION include/Smil 
      COMPONENT dev
      )


# Generate cmake file UseSmil.cmake


SET(CMAKE_USE_SMIL_STR "${LICENCE_TXT_WITH_SHARP}\n")
STRING(REPLACE ";" " " BUF "${USE_SMIL_INCLUDE_DIRS}")
LIST(APPEND CMAKE_USE_SMIL_STR "INCLUDE_DIRECTORIES(${BUF})\n\n")
STRING(REPLACE ";" " " BUF "${USE_SMIL_LIB_DIRS}")
LIST(APPEND CMAKE_USE_SMIL_STR "LINK_DIRECTORIES(${BUF})\n\n")
LIST(APPEND CMAKE_USE_SMIL_STR "ADD_DEFINITIONS(${USE_SMIL_DEFINITIONS})\n\n")
STRING(REPLACE ";" " " BUF "${USE_SMIL_LIBS}")
LIST(APPEND CMAKE_USE_SMIL_STR "SET(SMIL_LIBRARIES ${BUF})")

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/UseSmil.cmake ${CMAKE_USE_SMIL_STR})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/UseSmil.cmake
      DESTINATION include/Smil 
      COMPONENT dev
      )

# Generate Qt .pri file
SET(QT_PRI_SMIL_STR "${LICENCE_TXT_WITH_SHARP}\n")
LIST(APPEND QT_PRI_SMIL_STR "# QT SUBPROJECT FILE\n")
LIST(APPEND QT_PRI_SMIL_STR "# To be included in the main .pro file:\n#include(/usr/local/include/Smil/Smil.pri)\n\n")
FOREACH(DIR ${USE_SMIL_INCLUDE_DIRS})
      LIST(APPEND QT_PRI_SMIL_STR "INCLUDEPATH +=  ${DIR}\n")
ENDFOREACH()
LIST(APPEND QT_PRI_SMIL_STR "\n")
FOREACH(DIR ${USE_SMIL_LIB_DIRS})
      LIST(APPEND QT_PRI_SMIL_STR "LIBS +=  -L${DIR}\n")
ENDFOREACH()
LIST(APPEND QT_PRI_SMIL_STR "\n")
FOREACH(LIB ${USE_SMIL_LIBS})
      LIST(APPEND QT_PRI_SMIL_STR "LIBS +=  -l${LIB}\n")
ENDFOREACH()
LIST(APPEND QT_PRI_SMIL_STR "\n")
LIST(APPEND QT_PRI_SMIL_STR "QMAKE_CXXFLAGS +=  ${USE_SMIL_DEFINITIONS}\n")

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Smil.pri ${QT_PRI_SMIL_STR})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/Smil.pri
      DESTINATION include/Smil 
      COMPONENT dev
      )

