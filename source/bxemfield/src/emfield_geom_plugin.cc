// -*- mode: c++ ; -*-
/* emfield_geom_plugin.cc
 */

#include <emfield/emfield_geom_plugin.h>

#include <stdexcept>
#include <sstream>
#include <fstream>

namespace emfield {

  GEOMTOOLS_PLUGIN_REGISTRATION_IMPLEMENT(emfield_geom_plugin,"emfield::emfield_geom_plugin");

  const electromagnetic_field_manager & emfield_geom_plugin::get_manager () const
  {
    return _manager_;
  }

  emfield_geom_plugin::emfield_geom_plugin ()
  {
    _initialized_ = false;
    return;
  }

  emfield_geom_plugin::~emfield_geom_plugin ()
  {
    if (is_initialized ())
      {
        reset ();
      }
    return;
  }

  bool emfield_geom_plugin::is_initialized () const
  {
    return _initialized_;
  }

  int emfield_geom_plugin::initialize (const datatools::properties & config_,
                                       const geomtools::manager::plugins_dict_type & /*plugins_*/,
                                       const datatools::service_dict_type & /*services_*/)
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Plugin is already initialized !");

    geomtools::manager::base_plugin::_basic_initialize(config_);

    if (config_.has_key("manager.config")) {
      std::string mgr_config_file = config_.fetch_string("manager.config");
      datatools::fetch_path_with_env(mgr_config_file);
      datatools::properties mgr_config;
      datatools::properties::read_config(mgr_config_file, mgr_config);
      _build_manager(mgr_config);
    }
    else {
      datatools::properties mgr_config;
      config_.export_and_rename_starting_with(mgr_config, "manager.", "");
      // std::clog << "NOTICE: "
      //           << "emfield::emfield_geom_plugin::initialize: "
      //           << "Building the embeded EM field manager..." << std::endl;
      _build_manager(mgr_config);
    }

    _initialized_ = true;
    return 0;
  }

  void emfield_geom_plugin::_build_manager (const datatools::properties & manager_config_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    // std::vector<std::string> input_files;
    // if (manager_config_.has_key ("emfield.files"))
    //   {
    //     manager_config_.fetch("emfield.files", input_files);
    //   }
    // for (std::vector<std::string>::const_iterator i = input_files.begin ();
    //      i != input_files.end ();
    //      i++)
    //   {
    //     std::string filename = *i;
    //     datatools::fetch_path_with_env (filename);
    //     std::clog << "NOTICE: "
    //               << "emfield::emfield_geom_plugin::_build_manager: "
    //               << "Loading EM fields file '" << filename << "'..." << std::endl;
    //     _manager_.load (filename);
    //   }

    _manager_.set_geometry_manager (get_geo_manager());
    _manager_.initialize (manager_config_);

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  int emfield_geom_plugin::reset()
  {
    _manager_.reset();
    _initialized_ = false;
    return 0;
  }

} // end of namespace emfield

// end of emfield.h
