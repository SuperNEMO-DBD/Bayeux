// -*- mode: c++; -*-
// test_genbb_help_serialization.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <boost/scoped_ptr.hpp>

#include <datatools/clhep_units.h>

// Serializable :
#include <datatools/things.h>
#include <datatools/properties.h>
#include <geomtools/geom_id.h>
#include <genbb_help/primary_event.h>


#include <datatools/io_factory.h>

int main (int /* argc_ */, char ** /* argv_ */)
{

  using namespace std;
  int error_code = EXIT_SUCCESS;

  bool with_dbio = true;
  bool with_gbio = true;
  bool with_gbbbio = true;

  try  {
    {
      // declare the 'bag1' instance as a 'things' container:
      datatools::things bag1 ("bag1", "A bag with things in it");


      if (with_gbio) {
        bag1.add<geomtools::geom_id> ("g0");

        geomtools::geom_id & gg1 = bag1.add<geomtools::geom_id> ("g1");
        gg1.set_type (666);
        gg1.set_address (0, 1, 2);

        geomtools::geom_id & gg2 = bag1.add<geomtools::geom_id> ("g2");
        gg2.set_type (999);
        gg2.set_address (4, 3, 2, 1, 0);
      }

      if (with_dbio) {
        datatools::properties & p1 = bag1.add<datatools::properties> ("p1", "A property store");
        p1.set_description ("A list of properties");
        p1.store_flag ("fake");
      }

      if (with_gbbbio) {
        genbb::primary_event & e1 = bag1.add<genbb::primary_event> ("e1", "An event");
        e1.set_label ("Se-82 0nbb decay");
        e1.set_time (1.23456 * CLHEP::ns);
      }

      string filename = "test_things.txt";
      namespace ds = datatools;
      ds::data_writer writer;
      writer.init (filename, ds::using_multi_archives);
      writer.store (bag1);
    }

    {
      // declare the 'bag1' instance as a 'things' container:
      datatools::things bag1;

      string filename = "test_things.txt";
      namespace ds = datatools;
      ds::data_reader reader;
      reader.init (filename, ds::using_multi_archives);
      reader.load (bag1);

      bag1.tree_dump (clog, "Bag (reload) : ");

      if (with_gbbbio) {
        const genbb::primary_event & e1 = bag1.get<genbb::primary_event> ("e1");
        e1.dump (clog, "Primary event : ");
      }
    }
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_genbb_help_serialization.cxx
