# * Find SWIG This module finds an installed SWIG.  It sets the following
#   variables: SWIG_FOUND - set to true if SWIG is found SWIG_DIR - the
#   directory where swig is installed SWIG_EXECUTABLE - the path to the swig
#   executable SWIG_VERSION   - the version number of the swig executable
#
# The minimum required version of SWIG can be specified using the standard
# syntax, e.g. FIND_PACKAGE(SWIG 1.1)
#
# All information is collected from the SWIG_EXECUTABLE so the version to be
# found can be changed from the command line by means of setting SWIG_EXECUTABLE
#

# =============================================================================
# Copyright 2004-2009 Kitware, Inc. Copyright 2011 Mathieu Malaterre
# <mathieu.malaterre@gmail.com>
#
# Distributed under the OSI-approved BSD License (the "License"); see
# accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# License for more information.
# =============================================================================
# (To distribute this file outside of CMake, substitute the full License text
# for the above reference.)

find_program(SWIG_EXECUTABLE NAMES swig2.0 swig)

if(SWIG_EXECUTABLE)
  execute_process(
    COMMAND ${SWIG_EXECUTABLE} -swiglib
    OUTPUT_VARIABLE SWIG_swiglib_output
    ERROR_VARIABLE SWIG_swiglib_error
    RESULT_VARIABLE SWIG_swiglib_result)

  if(SWIG_swiglib_result)
    if(SWIG_FIND_REQUIRED)
      message(
        SEND_ERROR
          "Command \"${SWIG_EXECUTABLE} -swiglib\" failed with output:\n${SWIG_swiglib_error}"
      )
    else(SWIG_FIND_REQUIRED)
      message(
        STATUS
          "Command \"${SWIG_EXECUTABLE} -swiglib\" failed with output:\n${SWIG_swiglib_error}"
      )
    endif(SWIG_FIND_REQUIRED)
  else(SWIG_swiglib_result)
    string(REGEX REPLACE "[\n\r]+" ";" SWIG_swiglib_output
                         ${SWIG_swiglib_output})
    # force the path to be computed each time in case SWIG_EXECUTABLE has
    # changed.
    set(SWIG_DIR SWIG_DIR-NOTFOUND)
    find_path(
      SWIG_DIR swig.swg
      PATHS ${SWIG_swiglib_output}
      NO_CMAKE_FIND_ROOT_PATH)
    if(SWIG_DIR)
      set(SWIG_USE_FILE ${CMAKE_ROOT}/Modules/UseSWIG.cmake)
      execute_process(
        COMMAND ${SWIG_EXECUTABLE} -version
        OUTPUT_VARIABLE SWIG_version_output
        ERROR_VARIABLE SWIG_version_output
        RESULT_VARIABLE SWIG_version_result)
      if(SWIG_version_result)
        message(
          SEND_ERROR
            "Command \"${SWIG_EXECUTABLE} -version\" failed with output:\n${SWIG_version_output}"
        )
      else(SWIG_version_result)
        string(REGEX REPLACE ".*SWIG Version[^0-9.]*\([0-9.]+\).*" "\\1"
                             SWIG_version_output "${SWIG_version_output}")
        set(SWIG_VERSION
            ${SWIG_version_output}
            CACHE STRING "Swig version" FORCE)
      endif(SWIG_version_result)
    endif(SWIG_DIR)
  endif(SWIG_swiglib_result)
endif(SWIG_EXECUTABLE)

# INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(
  SWIG
  REQUIRED_VARS SWIG_EXECUTABLE SWIG_DIR
  VERSION_VAR SWIG_VERSION)
