// test_single_particle_generator.cxx
/*
 * Usage:
 *
 * shell$ .../test_single_particle_generator \
 *   | gsl-histogram 0 2 100 > ke.his
 *
 * gnuplot> plot './ke.his' using 1:3 with histep
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>

// This project:
#include <genbb_help/single_particle_generator.h>
#include <genbb_help/primary_event.h>


int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'single_particle_generator'!" << endl;

    bool debug = false;
    int many = 0;
    bool randomized_direction = false;
    bool cone_direction = false;
    bool energy_range_mode = false;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug"))  {
          debug = true;
        } else if ((option == "-m") || (option == "--many")) {
          many = 1;
        } else if ((option == "-M") || (option == "--many-many")) {
          many = 2;
        } else if ((option == "-r") || (option == "--randomized-direction")) {
          randomized_direction = true;
        } else if ((option == "-c") || (option == "--cone-direction"))  {
          cone_direction = true;
        } else if ((option == "-e") || (option == "--energy-range"))  {
          energy_range_mode = true;
        } else {
          clog << "warning: ignoring option '" << option << "'!" << endl;
        }
      } else {
        string argument = token;
        {
          clog << "warning: ignoring argument '" << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    genbb::single_particle_generator SPG;

    // Build a configuration properties container:
    datatools::properties config;
    if (debug) config.store_flag ("debug");
    config.store ("assign_generation_ids", true);

    /* The seed of the random number generator */
    config.store ("seed", 314159);

    /* Default momentum direction is along the Z axis */
    if (randomized_direction) {
      config.store_flag ("randomized_direction");
    } else if (cone_direction) {
      config.store_flag ("cone_direction");
    }
    config.store ("angle_unit", "degree");
    config.store ("cone.min_angle", 25.0);
    config.store ("cone.max_angle", 35.0);
    //config.update ("cone.angle", 10.0);
    config.store ("cone.axis", "z");
    config.change ("cone.axis", "30 45 degree");
    //config.update ("cone.axis", "30 45 degree");
    config.tree_dump(std::clog, "Event generator configuration : ");

    /* Possible values are:
     *   "electron" or "e-"
     *   "positron" or "e+"
     *   "gamma"
     *   "alpha"
     *   "neutron"
     *   "protron"
     *   a few others...
     */
    config.store ("particle_name", "electron");

    /* Possible values are:
     *  - "gaussian_energy" and required properties are:
     *    - "mean_energy"
     *    - "sigma_energy"
     *  - "energy_range" and required properties are:
     *    - "min_energy"
     *    - "max_energy"
     *  - "monokinetic" and required propertie is:
     *    - "energy"
     */
    config.store ("mode", "gaussian_energy");

    /* Possible values are: "keV", "MeV"... */
    config.store ("energy_unit", "keV");
    config.store ("mean_energy", 1000.0);
    config.store ("sigma_energy", 50.0);

    if (energy_range_mode) {
      config.change ("mode", "energy_range");
      config.store ("min_energy", 950.0);
      config.store ("max_energy", 1050.0);
    }

    if (debug) config.tree_dump (clog, "Configuration: ", "debug: ");

    // Configure the event generator:
    SPG.initialize_standalone (config);

    // Shoot events:
    size_t max_count = 10;
    if (many == 1) max_count = 100;
    if (many == 2) max_count = 1000;
    genbb::primary_event pe; // working primary event
    for (int i = 0; i < (int) max_count; i++) {
      clog << "Count : " << i << endl;
      SPG.load_next (pe);
      if (debug) pe.dump ();
      // Print the kinetic energy from the only particle in each event:
      cout << pe.get_particles ().front ().get_kinetic_energy ()
           << ' ' << pe.get_particles ().front ().get_momentum().x()
           << ' ' << pe.get_particles ().front ().get_momentum().y()
           << ' ' << pe.get_particles ().front ().get_momentum().z()
           << endl;
    }

    if (debug) clog << "debug: " << "The end." << endl;

  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
