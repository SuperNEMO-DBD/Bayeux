//! \file  datatools/introspection/testing/test-introspection-data_description.cxx
//! \brief Datatools introspection data description test
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Datatools.
//
// Datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Datatools. If not, see <http://www.gnu.org/licenses/>.

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/units.h>

// This project:
#include <datatools/datatools.h>
#include <datatools/introspection/data_description.h>

void test_data_description_1();
void test_data_description_2();

int main(int /* argc_ */, char ** /* argv_ */)
{
  datatools::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'datatools::introspection::data_description' class."
              << std::endl;

    datatools::units::registry::const_system_registry().tree_dump(std::clog,
                                                                  "Unit registry: ");
    std::clog << std::endl;

    test_data_description_1();
    test_data_description_2();

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  datatools::terminate();
  return (error_code);
}

void test_data_description_1()
{
  std::clog << "\ntest_data_description_1: basics" << std::endl;

  {
    datatools::introspection::data_description dd;
    dd.set_type(datatools::introspection::DATA_TYPE_INT32);
    dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
    dd.tree_dump(std::clog, "Data description: ");
    std::clog << std::endl;

    datatools::properties dd_config;
    dd.export_to_config(dd_config, datatools::introspection::data_description::DD_XC_DEFAULT, "test.");
    dd_config.tree_dump(std::clog, "Data description configuration: ");
    std::clog << std::endl;

  }


  try {
    datatools::introspection::data_description dd;
    dd.set_type(datatools::introspection::DATA_TYPE_FLOAT);
    dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
    datatools::introspection::unit_info uinfo;
    uinfo.make_implicit_unit("kilopotato");
    dd.set_unit_info(uinfo);
  } catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS,
                 "As expected, data description failed due to invalid implicit unit!");
  }
  std::clog << std::endl;

  {
    datatools::introspection::data_description dd;
    dd.set_type(datatools::introspection::DATA_TYPE_FLOAT);
    dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
    datatools::introspection::unit_info uinfo;
    uinfo.make_implicit_unit("volt");
    dd.set_unit_info(uinfo);
    dd.tree_dump(std::clog, "Data description: ");
    std::clog << std::endl;


    datatools::properties dd_config;
    dd.export_to_config(dd_config, datatools::introspection::data_description::DD_XC_DEFAULT, "test.");
    dd_config.tree_dump(std::clog, "Data description configuration: ");
    std::clog << std::endl;

  }

  try {
    datatools::introspection::data_description dd;
    dd.set_type(datatools::introspection::DATA_TYPE_FLOAT);
    dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
    datatools::introspection::unit_info uinfo;
    uinfo.make_explicit_unit_dimension("age", "year"); // Should be "time"
    dd.set_unit_info(uinfo);
  } catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS,
                 "As expected, data description failed due to invalid explicit unit dimension!");
  }
  std::clog << std::endl;

  {
    datatools::introspection::data_description dd;
    dd.set_type(datatools::introspection::DATA_TYPE_FLOAT);
    dd.set_layout(datatools::introspection::DATA_LAYOUT_VECTOR_WITH_FREE_SIZE);
    datatools::introspection::unit_info uinfo;
    uinfo.make_explicit_unit_dimension("pressure", "mbar");
    // uinfo.make_explicit_unit_dimension("temperature", "kelvin");
    dd.set_unit_info(uinfo);
    dd.tree_dump(std::clog, "Data description: ");
    std::clog << std::endl;
  }

  return;
}

void test_data_description_2()
{
  std::clog << "\ntest_data_description_2: configuration " << std::endl;

  {
    datatools::introspection::data_description dd;

    datatools::properties dd_config;
    dd_config.store_string("type",   "double");
    dd_config.store_string("layout", "vector_with_fixed_size");
    dd_config.store_integer("vector_fixed_size", 3);
    dd_config.store_string("unit.support", "explicit_unit_dimension");
    dd_config.store_string("unit.explicit_unit_dimension", "length");
    dd_config.store_string("unit.preferred_unit", "mm");
    dd.initialize(dd_config);
    dd.tree_dump(std::clog, "Data description: ");
    std::clog << std::endl;
  }

  return;
}
