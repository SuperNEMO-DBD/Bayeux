# - Module definition for materials submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

if(NOT MODULE_HEADER_ROOT)
  message(FATAL_ERROR "MODULE_HEADER_ROOT not specified")
endif()

# - Module
set(module_name materials)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")

foreach(dir root_dir include_dir source_dir test_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(materials_VERSION_MAJOR 4)
set(materials_VERSION_MINOR 0)
set(materials_PATCH_VERSION 0)
set(materials_VERSION "${materials_VERSION_MAJOR}.${materials_VERSION_MINOR}.${materials_PATCH_VERSION}")


# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/chemical_symbol.h
  ${module_include_dir}/${module_name}/detail/tools.h
  ${module_include_dir}/${module_name}/element.h
  ${module_include_dir}/${module_name}/factory.h
  ${module_include_dir}/${module_name}/isotope.h
  ${module_include_dir}/${module_name}/manager.h
  ${module_include_dir}/${module_name}/material.h
  ${module_include_dir}/${module_name}/materials_config.h.in
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/chemical_symbol.cc
  ${module_source_dir}/element.cc
  ${module_source_dir}/factory.cc
  ${module_source_dir}/isotope.cc
  ${module_source_dir}/manager.cc
  ${module_source_dir}/material.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout "${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()

# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "MATERIALS_DATA_DIR=${module_root_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_chemical_symbol.cxx
  ${module_test_dir}/test_element.cxx
  ${module_test_dir}/test_factory.cxx
  ${module_test_dir}/test_isotope.cxx
  ${module_test_dir}/test_manager_2.cxx
  ${module_test_dir}/test_manager.cxx
  ${module_test_dir}/test_material.cxx
  ${module_test_dir}/test_materials.cxx
  )
