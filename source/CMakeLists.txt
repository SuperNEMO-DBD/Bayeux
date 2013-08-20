# - CMake script for building Bayeux from its submodules
#

#-----------------------------------------------------------------------
# Copyright 2012,2013 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012,2013 University of Warwick
#
# This file is part of Bayeux.
#
# Bayeux is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Bayeux is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Configure build of Bayeux library
# - Packages
# - Boost
find_package(Boost 1.48.0 REQUIRED filesystem system serialization iostreams)

# - CLHEP
find_package(CLHEP REQUIRED)

# - GSL
find_package(GSL QUIET NO_MODULE)

# - ROOT
find_package(ROOT 5.34.00 REQUIRED RIO Tree)


# - Core Includes for Bayeux + Third Party
include_directories(${Bayeux_BUILDPRODUCT_DIR}/include)
include_directories(${Bayeux_BUILDPRODUCT_DIR}/include/bayeux)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CLHEP_INCLUDE_DIRS})
include_directories(${GSL_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})

# - Configure Bayeux submodule first
set(MODULE_HEADER_ROOT "${Bayeux_BUILDPRODUCT_DIR}/include")
include(bxbayeux_module.cmake)
set(Bayeux_HEADERS ${bayeux_MODULE_HEADERS})
set(Bayeux_SOURCES ${bayeux_MODULE_SOURCES})
set(Bayeux_TESTS)

# - The LPC Modules
# The processing order should be from least to most dependent
# That's to ensure that any needed CMake variables are set in the correct
# order, and that ordering of source files (and hence object files)
# is correct.
# NB: Order may need reviewing because may need least dependent object
# files last because of the statics...
set(Bayeux_SUBMODULES
  datatools 
  cuts
  materials
  mygsl
  geomtools
  emfield
  genbb_help
  genvtx
  trackfit
  brio
  dpp
  )

set(MODULE_HEADER_ROOT "${Bayeux_BUILDPRODUCT_DIR}/include/bayeux")
foreach(_m ${Bayeux_SUBMODULES})
  include(bx${_m}_module.cmake)
  list(APPEND Bayeux_HEADERS ${${_m}_MODULE_HEADERS})
  list(APPEND Bayeux_SOURCES ${${_m}_MODULE_SOURCES})
endforeach()

# Hack for genbb private headers
include_directories(${genbb_help_include_dir})

#-----------------------------------------------------------------------
# Build/Link the library
#
add_library(Bayeux SHARED ${Bayeux_HEADERS} ${Bayeux_SOURCES})
target_link_libraries(Bayeux ${CMAKE_DL_LIBS})
target_link_libraries(Bayeux ${Boost_LIBRARIES})
target_link_libraries(Bayeux ${GSL_LIBRARIES})
target_link_libraries(Bayeux ${CLHEP_LIBRARIES})
target_link_libraries(Bayeux ${ROOT_LIBRARIES})

#-----------------------------------------------------------------------
# Install targets and resources
#
install(TARGETS Bayeux
  EXPORT  BayeuxTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )

install(DIRECTORY ${Bayeux_BUILDPRODUCT_DIR}/include/
  DESTINATION include
  )

export(TARGETS Bayeux FILE ${Bayeux_BUILDPRODUCT_DIR}/lib/cmake/Bayeux-${Bayeux_VERSION}/BayeuxTargets.cmake)
install(EXPORT BayeuxTargets DESTINATION lib/cmake/Bayeux-${Bayeux_VERSION})

#-----------------------------------------------------------------------
# Configure/Install support files
# Done here so that external package variables are visible
#
# - Versioning (build and install trees)
configure_file(${PROJECT_SOURCE_DIR}/cmake/BayeuxVersionConfig.cmake.in
  ${Bayeux_BUILDPRODUCT_DIR}/lib/cmake/Bayeux-${Bayeux_VERSION}/BayeuxVersionConfig.cmake
  @ONLY
  )

# - Config (build and install trees *at present*)
configure_file(${PROJECT_SOURCE_DIR}/cmake/BayeuxConfig.cmake.in
  ${Bayeux_BUILDPRODUCT_DIR}/lib/cmake/Bayeux-${Bayeux_VERSION}/BayeuxConfig.cmake
  @ONLY
  )

install(
  FILES 
    ${Bayeux_BUILDPRODUCT_DIR}/lib/cmake/Bayeux-${Bayeux_VERSION}/BayeuxVersionConfig.cmake
    ${Bayeux_BUILDPRODUCT_DIR}/lib/cmake/Bayeux-${Bayeux_VERSION}/BayeuxConfig.cmake
  DESTINATION 
    lib/cmake/Bayeux-${Bayeux_VERSION}
  )

#-----------------------------------------------------------------------
# Build unit tests if required
# - NB assumes each test .cxx file generates a single executable
if(Bayeux_ENABLE_TESTING)
  foreach(_m ${Bayeux_SUBMODULES})
    foreach(_testsource ${${_m}_MODULE_TESTS})
      get_filename_component(_testname "${_testsource}" NAME_WE)
      set(_testname "${_m}-${_testname}")

      add_executable(${_testname} ${_testsource})
      set_property(TARGET ${_testname}
        APPEND PROPERTY INCLUDE_DIRECTORIES ${${_m}_test_dir}
        )

      target_link_libraries(${_testname} Bayeux)
      
      add_test(NAME ${_testname} COMMAND ${_testname})
      set_property(TEST ${_testname}
        APPEND PROPERTY ENVIRONMENT ${${_m}_TEST_ENVIRONMENT}
        )
    endforeach()
  endforeach()
endif()

