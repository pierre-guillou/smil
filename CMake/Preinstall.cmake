# Copyright (c) 2011-2016, Matthieu FAESSEL and ARMINES
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Matthieu FAESSEL, or ARMINES nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


FILE(READ license.txt LICENCE_TXT)
STRING(REGEX REPLACE "([^\n]*\n)" "// \\1" LICENCE_TXT_WITH_SLASH "${LICENCE_TXT}")
STRING(REGEX REPLACE "([^\n]*\n)" "# \\1" LICENCE_TXT_WITH_SHARP "${LICENCE_TXT}")

# Definitions
GET_DIRECTORY_PROPERTY(_USE_SMIL_DEFINITIONS COMPILE_DEFINITIONS)
LIST(REMOVE_DUPLICATES _USE_SMIL_DEFINITIONS)
SET(USE_SMIL_DEFINITIONS)
SET(USE_SMIL_DEFINES)
FOREACH(DEF ${_USE_SMIL_DEFINITIONS})
    IF(NOT ${DEF} MATCHES "\\$.*")
        LIST(APPEND USE_SMIL_DEFINITIONS "-D${DEF} ")
        LIST(APPEND USE_SMIL_DEFINES ${DEF})
    ENDIF()
ENDFOREACH(DEF ${_USE_SMIL_DEFINITIONS})
IF(USE_OPEN_MP)
    LIST(APPEND USE_SMIL_DEFINITIONS ${OpenMP_CXX_FLAGS})
ENDIF(USE_OPEN_MP)

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
SET(USE_SMIL_INCLUDE_DIRS_LOCAL ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${USE_SMIL_INCLUDE_DIRS})
LIST(INSERT USE_SMIL_INCLUDE_DIRS 0 ${SMIL_HEADERS_INSTALL_PATH})


# Libraries
SET(USE_SMIL_LIBS ${SMIL_LIBS})
SET(USE_SMIL_LIB_DIRS)
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

SET(USE_SMIL_LIB_DIRS_LOCAL ${LIBRARY_OUTPUT_PATH} ${USE_SMIL_LIB_DIRS})
SET(USE_SMIL_LIB_DIRS ${SMIL_LIBRARIES_INSTALL_PATH} ${USE_SMIL_LIB_DIRS})



# Generate main header Smil.h
SET(MAIN_HEADER_STR "${LICENCE_TXT_WITH_SLASH}\n")
LIST(APPEND MAIN_HEADER_STR "#ifndef SMIL_GLOBAL_HEADER\n#define SMIL_GLOBAL_HEADER\n\n\n")

FOREACH(DEF ${USE_SMIL_DEFINES})
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


# Generate cmake file UseSmil.cmake (local)

SET(CMAKE_USE_SMIL_STR "${LICENCE_TXT_WITH_SHARP}\n")
STRING(REPLACE ";" " " BUF "${USE_SMIL_INCLUDE_DIRS_LOCAL}")
LIST(APPEND CMAKE_USE_SMIL_STR "INCLUDE_DIRECTORIES(${BUF})\n\n")
STRING(REPLACE ";" " " BUF "${USE_SMIL_LIB_DIRS_LOCAL}")
LIST(APPEND CMAKE_USE_SMIL_STR "LINK_DIRECTORIES(${BUF})\n\n")
LIST(APPEND CMAKE_USE_SMIL_STR "ADD_DEFINITIONS(${USE_SMIL_DEFINITIONS})\n\n")
STRING(REPLACE ";" " " BUF "${USE_SMIL_LIBS}")
LIST(APPEND CMAKE_USE_SMIL_STR "SET(SMIL_LIBRARIES ${BUF})")

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/UseSmil.cmake ${CMAKE_USE_SMIL_STR})

# Generate cmake file UseSmil.cmake (for installation)

SET(CMAKE_USE_SMIL_STR "${LICENCE_TXT_WITH_SHARP}\n")
STRING(REPLACE ";" " " BUF "${USE_SMIL_INCLUDE_DIRS}")
LIST(APPEND CMAKE_USE_SMIL_STR "INCLUDE_DIRECTORIES(${BUF})\n\n")
STRING(REPLACE ";" " " BUF "${USE_SMIL_LIB_DIRS}")
LIST(APPEND CMAKE_USE_SMIL_STR "LINK_DIRECTORIES(${BUF})\n\n")
LIST(APPEND CMAKE_USE_SMIL_STR "ADD_DEFINITIONS(${USE_SMIL_DEFINITIONS})\n\n")
STRING(REPLACE ";" " " BUF "${USE_SMIL_LIBS}")
LIST(APPEND CMAKE_USE_SMIL_STR "SET(SMIL_LIBRARIES ${BUF})")

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/UseSmil.cmake.install ${CMAKE_USE_SMIL_STR})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/UseSmil.cmake.install
      DESTINATION include/Smil
      RENAME UseSmil.cmake
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

# Generate pkg-config .pc file
LIST(APPEND PKG_CONFIG_SMIL_DEPS "-lstdc++" "-lm")
# Caution: library order is important for dependency resolution
LIST(APPEND PKG_CONFIG_SMIL_LIBS
  " -lsmilBase -lsmilCore -lsmilGui"
  " -lsmilIO -lsmilMorpho -lsmilRGB"
  )
IF(USE_OPEN_MP)
  LIST(APPEND PKG_CONFIG_SMIL_CFLAGS " -fopenmp")
  LIST(APPEND PKG_CONFIG_SMIL_LIBS " -fopenmp")
ENDIF(USE_OPEN_MP)
IF(USE_OPTIMIZATION)
  LIST(APPEND PKG_CONFIG_SMIL_CFLAGS "${CMAKE_CXX_FLAGS}")
ENDIF(USE_OPTIMIZATION)
IF(USE_PNG)
  LIST(APPEND PKG_CONFIG_SMIL_DEPS ${PNG_LIBRARIES})
ENDIF(USE_PNG)
IF(USE_JPEG)
  LIST(APPEND PKG_CONFIG_SMIL_DEPS ${JPEG_LIBRARIES})
ENDIF(USE_JPEG)
IF(USE_TIFF)
  LIST(APPEND PKG_CONFIG_SMIL_DEPS ${TIFF_LIBRARIES})
ENDIF(USE_TIFF)
IF(USE_CURL)
  LIST(APPEND PKG_CONFIG_SMIL_DEPS ${CURL_LIBRARIES})
ENDIF(USE_CURL)
IF(USE_FREETYPE)
  LIST(APPEND PKG_CONFIG_SMIL_DEPS ${FREETYPE_LIBRARIES})
  # Hopefully get Freetype include directory...
  LIST(APPEND PKG_CONFIG_SMIL_CFLAGS " -I${FREETYPE_INCLUDE_DIR_FTHEADER}")
ENDIF(USE_FREETYPE)
IF(USE_QWT)
  LIST(APPEND PKG_CONFIG_SMIL_DEPS "-l${QWT_LIBRARIES}")
ENDIF(USE_QWT)
IF(USE_AALIB)
  LIST(APPEND PKG_CONFIG_SMIL_DEPS "-l${AALIB_LIBRARY}")
ENDIF(USE_AALIB)
IF(USE_QT)
  IF(USE_QT_VERSION EQUAL 5)
    # Qt libs are needed by Smil header files...
    LIST(APPEND PKG_CONFIG_SMIL_LIBS " -lQt5Widgets -lQt5Gui -lQt5Core")
  ELSE()
    LIST(APPEND PKG_CONFIG_SMIL_LIBS " -lQtGui -lQtCore")
    FOREACH(INC ${QT_INCLUDES})
      LIST(APPEND PKG_CONFIG_SMIL_CFLAGS " -I${INC}")
    ENDFOREACH()
  ENDIF(USE_QT_VERSION EQUAL 5)
ENDIF(USE_QT)
# Generate spaces-separated strings
FOREACH(LIB ${PKG_CONFIG_SMIL_DEPS})
  LIST(APPEND PKG_CONFIG_SMIL_LIBS_PRIV " ${LIB}")
ENDFOREACH()

LIST(APPEND PKG_CONFIG_SMIL_STR
  "prefix=${CMAKE_INSTALL_PREFIX}\n"
  "libdir=\${prefix}/lib/Smil\n"
  "includedir=\${prefix}/include\n"
  "\n"
  "Name: Smil\n"
  "Description: Simple (but efficient) Morphological Image Library\n"
  "URL: http://smil.cmm.mines-paristech.fr/doc/index.html\n"
  "Version: ${SMIL_VERSION}\n"
  "Libs: -L\${libdir} ${PKG_CONFIG_SMIL_LIBS}\n"
  "Libs.private: ${PKG_CONFIG_SMIL_LIBS_PRIV}\n"
  "Cflags: -I\${includedir}/Smil ${PKG_CONFIG_SMIL_CFLAGS}\n"
  )
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/smil.pc ${PKG_CONFIG_SMIL_STR})
INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/smil.pc
  DESTINATION lib/pkgconfig
  COMPONENT dev
  )
