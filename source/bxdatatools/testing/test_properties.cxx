// test_properties.cxx

// Ourselves:
#include <datatools/properties.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <cmath>

// This project:
#include <datatools/io_factory.h>
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

void test_more();

int main(int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test of the 'properties' class..." << endl;

    bool debug = false;
    bool use_validator = true;
    long seed = 12345;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];
      if ( arg == "-d" || arg == "--debug" ) debug = true;
      if ( arg == "-v" || arg == "--no-validator" ) use_validator = false;
      iarg++;
    }

    srand48 (seed);

    clog << "========================================" << endl;

    datatools::properties::data a_data ('I', 3);
    a_data.dump (clog);
    clog << endl;

    if (int error = a_data.set_value (666)) {
      clog << "1 - datatools::properties::data::set_value: "
           << datatools::properties::data::get_error_message (error)
           << "!" << endl;
    }
    a_data.lock ();
    a_data.dump (clog);
    clog << endl;

    if (int error = a_data.set_value (666, 2)) {
      clog << "2 - datatools::properties::data::set_value: "
           << datatools::properties::data::get_error_message (error)
           << "!" << endl;
    }
    a_data.unlock ();
    a_data.dump (clog);
    clog << endl;

    if (int error = a_data.set_value (666, 2)) {
      clog << "3 - datatools::properties::data::set_value: "
           << datatools::properties::data::get_error_message (error)
           << "!" << endl;
    }
    a_data.dump (clog);
    clog << endl;

    if (int error = a_data.set_value (666, 8)) {
      clog << "4 - datatools::properties::data::set_value: "
           << datatools::properties::data::get_error_message (error)
           << "!" << endl;
    }
    a_data.dump (clog);
    clog << endl;

    if (int error = a_data.set_value ("bad value"))
      {
        clog << "5 - datatools::properties::data::set_value: "
             << datatools::properties::data::get_error_message (error)
             << "!" << endl;
      }
    a_data.dump (clog);
    clog << endl;

    clog << "========================================" << endl;

    datatools::properties my_dict("A list of user properties; group=Foo; date=01-02-2016");
    if (! use_validator) {
      my_dict.unset_key_validator();
    }
    my_dict.dump(clog);
    clog << endl;
    clog << "========================================" << endl;

    //datatools::properties::g_debug=true;
    my_dict.store("name","my name");
    clog << "========================================" << endl;
    my_dict.store("firstname","my firstname");
    clog << "========================================" << endl;
    my_dict.store(datatools::properties::make_private_key ("private"),
                  "a private data", "Confidential stuff");

    try {
      my_dict.store("#number", 666);
    } catch (exception & x) {
      clog << "ERROR: " << x.what() << endl;
    }
    my_dict.store("age", 24, "the age of the captain");

    try {
      my_dict.store ("007", "James Bond");
    } catch (std::exception & x) {
      clog << "ERROR: " << x.what () << endl;
    }

    try {
      my_dict.store ("", "Money Penny");
    } catch (std::exception & x) {
      clog << "ERROR: " << x.what () << endl;
    }

    try {
      my_dict.store ("bad_token", "a bad \"char\"");
    } catch (std::exception & x) {
      clog << "ERROR: " << x.what () << endl;
    }

    my_dict.store_with_explicit_unit ("weight", 23.4 * CLHEP::kg);
    my_dict.set_unit_symbol("weight", "kg");

    my_dict.store ("pi", 3.14159 );

    my_dict.store ("dummy", 6);

    my_dict.store ("male", true);

    std::vector<double> coeffs;
    coeffs.push_back (1.0);
    coeffs.push_back (2.0);
    coeffs.push_back (3.0);
    my_dict.store ("coeffs", coeffs, "undimensioned coefficients");
    my_dict.dump (clog);
    clog << endl;

    std::vector<double> vals;
    vals.push_back (1.0 * CLHEP::cm);
    vals.push_back (2.0 * CLHEP::cm);
    vals.push_back (3.0 * CLHEP::cm);
    my_dict.store ("position", vals, "the position of an object");
    my_dict.set_explicit_unit("position", true);
    my_dict.set_unit_symbol("position", "cm");
    my_dict.dump (clog);
    clog << endl;

    my_dict.change ("position", 3.14 * CLHEP::m , 1);
    my_dict.change ("age", 77);
    my_dict.lock ("age");
    my_dict.change ("male", false);
    my_dict.update ("phantom", 4);

    my_dict.erase ("dummy");
    my_dict.dump (clog);
    bool male;
    my_dict.fetch ("male", male);
    int age;
    my_dict.fetch ("age", age);
    vector<double> vals1;
    my_dict.fetch ("position", vals1);
    clog << endl;

    std::vector<double> vals2;
    double s = M_PI;
    for (int i = 1; i < 13; i++) {
      s /= i;
      vals2.push_back (s * CLHEP::radian);
    }
    my_dict.change ("position", vals2);
    my_dict.set_explicit_unit("position", true);
    my_dict.set_unit_symbol("position", "degree");
    if (debug) my_dict.tree_dump (clog, "datatools::properties", "DEBUG: ***** ");
    clog << endl;

    std::vector<bool> bits;
    for (int i = 0; i < 23; i++) bits.push_back (drand48 () > 0.6? true: false);
    my_dict.store ("bits", bits);
    clog << endl;

    std::vector<std::string> tokens;
    for (int i = 0; i < 12; i++) {
      std::ostringstream tok;
      tok << "token_" << i;
      tokens.push_back (tok.str ());
    }
    tokens.push_back (">DEBUG<");
    my_dict.store ("tokens", tokens);

    std::vector<int> counts;
    my_dict.store ("counts", counts, "Event counts");
    my_dict.update ("phantom", 5);

    my_dict.dump (clog);
    if (debug) my_dict.tree_dump (clog, "datatools::properties", "DEBUG: ");
    clog << endl;

    double pinf, minf, xnan;
    datatools::plus_infinity(pinf);
    datatools::minus_infinity(minf);
    datatools::invalidate(xnan);
    my_dict.store ("plus_inf", pinf);
    my_dict.store ("minus_inf", minf);
    my_dict.store ("NaN", xnan);

    clog << "========================================" << endl;

    string filename = "test_properties.xml";
    clog << "serialize: writing to '" << filename << "' ..." << endl;
    {
      datatools::data_writer writer (filename);
      writer.store (my_dict);
    }
    clog << "serialize: writing done." << endl;
    clog << endl;

    my_dict.clear ();
    my_dict.tree_dump (clog,"Dict after clear:");
    clog << endl;

    clog << "serialize: reading from '" << filename << "'..."
         << endl;
    {
      datatools::data_reader reader(filename);
      if (reader.has_record_tag()) {
        if (reader.get_record_tag() == datatools::properties::SERIAL_TAG) {
          datatools::properties a_dict;
          reader.load(a_dict);
          my_dict = a_dict;
        } else {
          clog << "Record tag : '"<< reader.get_record_tag() << "'" << endl;
        }
      }
    }

    clog << "serialize: reading done." << endl;
    clog << endl;
    // my_dict.tree_dump(clog, "Dict after reading:");
    {
      boost::property_tree::ptree poptions;
      poptions.put(datatools::i_tree_dumpable::base_print_options::title_key(), "Dict after reading: ");
      // poptions.put(base_print_options::indent_key(), "");
      my_dict.print_tree(clog, poptions);
    }
    clog << "========================================" << endl;

    string filename_cfg = "test_properties.conf";

    datatools::properties::write_config(filename_cfg, my_dict);
    my_dict.clear();
    my_dict.tree_dump(clog, "Cleared dict:");
    clog << endl;

    clog << "========================================" << endl;

    datatools::properties::read_config(filename_cfg, my_dict);

    my_dict.tree_dump(clog, "Loaded dict:");
    clog << endl;

    clog << "========================================" << endl;

    clog << "Enter a list of double-quoted strings (ex: \"my\" \"favorite\" \"color\" \"is\" \"blue\"): "
         << endl;
    std::string as;
    std::istringstream s_in("\"my\" \"favorite\" \"color\" \"is\" \"blue\"");
    std::getline(s_in, as);

    std::string s2;
    std::istringstream iss (as);

    do {
      iss >> std::ws;
      if (iss.eof()) break;
      s2 = "";
      uint32_t reader_flags = datatools::io::reader_allow_trailing_characters;
      if (datatools::io::read_quoted_string(iss, s2, reader_flags)) {
        clog << "String = '" << s2 << "'" << endl;
      } else {
        clog << "Cannot parse quoted string from '" << as
             << "'!" << endl;
      }
    } while (iss);

    clog << "========================================" << endl;

    clog << "Example of an invalid string: " << endl;
    std::string s3 = "aze\"rty";
    clog << "s3='" << s3 << "'" << endl;
    if (datatools::properties::data::has_forbidden_char (s3)) {
      clog << "Oops! As expected, there are forbidden chars found in s3='" << s3 << "' !"
           << endl;
    } else {
      clog << "'s3' is ok !" << endl;
    }

    clog << "Example of a valid string: " << endl;
    std::string s4 = "azerty";
    clog << "s4='" << s4 << "'" << endl;
    if (datatools::properties::data::has_forbidden_char (s4)) {
      clog << "Oops! As expected, there are forbidden chars found in s4='" << s4 << "' !"
           << endl;
    } else {
      clog << "'s4' is ok!" << endl;
    }

    datatools::properties cfg("Configuration set;author=John Doe;date=2016-11-15;group=admin");
    cfg.tree_dump(std::clog, "Cfg");

    std::clog << "\n";
    test_more();
    std::clog << "\n";

  } catch (std::exception & x) {
    std::clog << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}


void test_more()
{
  std::clog << "Test the datatools::properties::fetch_real_with_explicit_dimension method...\n";
  datatools::properties setup;

  std::string setup_file = "${DATATOOLS_TESTING_DIR}/config/test_properties_sample.conf";
  datatools::fetch_path_with_env(setup_file);
  uint32_t read_options = datatools::properties::config::LOG_WARNING;
  datatools::properties::read_config(setup_file, setup, read_options);
  {
    boost::property_tree::ptree options;
    options.put("indent", "[debug] ");
    options.put("title", "Properties sample : ");
    setup.print_tree(std::cout, options);
  }
  if (setup.has_key("x")) {
    double x = setup.fetch_real_with_explicit_dimension("x", "length");
    std::clog << "x (naked) = "<< x << std::endl;
    std::clog << "x (mm)    = "<< x / CLHEP::mm << std::endl;
    std::clog << "x (cm)    = "<< x / CLHEP::cm << std::endl;
    std::clog << "x (m)     = "<< x / CLHEP::m << std::endl;
  }

  return;
}
