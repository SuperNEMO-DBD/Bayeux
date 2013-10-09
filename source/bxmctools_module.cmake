# - Module definition for mctools submodule of Bayeux
#
# To configure the module headers, the variable
# MODULE_HEADER_ROOT must be set before including this file

foreach(_modulevar MODULE_HEADER_ROOT MODULE_RESOURCE_ROOT)
  if(NOT ${_modulevar})
    message(FATAL_ERROR "${_modulevar} not specified")
  endif()
endforeach()

# - Module
set(module_name mctools)
set(module_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/bx${module_name}")
set(module_include_dir "${module_root_dir}/include")
set(module_source_dir  "${module_root_dir}/src")
set(module_test_dir    "${module_root_dir}/testing")
set(module_app_dir     "${module_root_dir}/programs")
set(module_resource_dir "${module_root_dir}/resources")

foreach(dir root_dir include_dir source_dir test_dir app_dir resource_dir)
  set(${module_name}_${dir} ${module_${dir}})
endforeach()

# - In place defs for module CMake variables...
# - Versioning
set(mctools_VERSION_MAJOR 1)
set(mctools_VERSION_MINOR 0)
set(mctools_VERSION_PATCH 0)
set(mctools_VERSION "${mctools_VERSION_MAJOR}.${mctools_VERSION_MINOR}.${mctools_VERSION_PATCH}")

# - Boost I/O, CAMP Reflection
set(MCTOOLS_WITH_BIO 1)
set(MCTOOLS_WITH_REFLECTION 1)

# - Raw Headers and Sources
set(${module_name}_MODULE_HEADERS
  ${module_include_dir}/${module_name}/base_step_hit.h
  ${module_include_dir}/${module_name}/simulated_data.h
  ${module_include_dir}/${module_name}/utils.h
  ${module_include_dir}/${module_name}/base_step_hit_processor.h
  ${module_include_dir}/${module_name}/step_hit_processor_factory.h
  ${module_include_dir}/${module_name}/calorimeter_step_hit_processor.h
  ${module_include_dir}/${module_name}/simulated_data_reader.h
  ${module_include_dir}/${module_name}/simulated_data_input_module.h
  ${module_include_dir}/${module_name}/base_step_hit.ipp
  ${module_include_dir}/${module_name}/simulated_data.ipp
  ${module_include_dir}/${module_name}/the_serializable.ipp
  ${module_include_dir}/${module_name}/the_serializable.h
  ${module_include_dir}/${module_name}/bio_guard.h
  ${module_include_dir}/${module_name}/base_step_hit-reflect.h
  ${module_include_dir}/${module_name}/simulated_data-reflect.h
  ${module_include_dir}/${module_name}/the_introspectable.h
  ${module_include_dir}/${module_name}/reflection_guard.h
  ${module_include_dir}/${module_name}/mctools_config.h.in
  ${module_include_dir}/${module_name}/version.h.in
  ${module_include_dir}/${module_name}/detail/bio_link_guard.h
  ${module_include_dir}/${module_name}/detail/reflection_link_guard.h
  ${module_include_dir}/${module_name}/detail/step_hit_processor_macros.h
  )

set(${module_name}_MODULE_SOURCES
  ${module_source_dir}/base_step_hit.cc
  ${module_source_dir}/base_step_hit_processor.cc
  ${module_source_dir}/step_hit_processor_factory.cc
  ${module_source_dir}/calorimeter_step_hit_processor.cc
  ${module_source_dir}/utils.cc
  ${module_source_dir}/simulated_data.cc
  ${module_source_dir}/simulated_data_reader.cc
  ${module_source_dir}/simulated_data_input_module.cc
  ${module_source_dir}/version.cc
  ${module_source_dir}/the_serializable.cc
  )

# - Published headers
foreach(_hdrin ${${module_name}_MODULE_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  string(REGEX REPLACE "^${module_include_dir}" "${MODULE_HEADER_ROOT}" _hdrout 
"${_hdrout}")
  configure_file(${_hdrin} ${_hdrout} @ONLY)
endforeach()


# - Unit tests
set(${module_name}_TEST_ENVIRONMENT "MCTOOLS_DATA_DIR=${module_root_dir}")

set(${module_name}_MODULE_TESTS
  ${module_test_dir}/test_base_step_hit.cxx
  ${module_test_dir}/test_simulated_data_1.cxx
  ${module_test_dir}/test_simulated_data_reader_1.cxx
  ${module_test_dir}/test_step_hit_processor_factory.cxx
  ${module_test_dir}/test_simulated_data_input_module_1.cxx
  )

