// bayeux/detail/bayeux_library.cc - Implementation of Bayeux Library Sytem Singleton
//
// Copyright (c) 2017 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 Université de Caen
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <bayeux/detail/bayeux_library.h>

// Submodules version & resource management
#include <bayeux/bayeux_config.h>
#include <bayeux/version.h>
#include <bayeux/reloc.h>
#include <bayeux/resource.h>
#include <datatools/exception.h>
#include <datatools/properties.h>
#include <datatools/version.h>
#include <datatools/resource.h>
#include <datatools/datatools_config.h>
#include <cuts/version.h>
#include <brio/version.h>
#include <dpp/version.h>
#include <materials/version.h>
#include <materials/resource.h>
#include <materials/materials_config.h>
#include <mygsl/version.h>
#include <geomtools/version.h>
#include <geomtools/resource.h>
#include <geomtools/geomtools_config.h>
#include <emfield/version.h>
#include <genbb_help/version.h>
#include <genbb_help/resource.h>
#include <genbb_help/genbb_help_config.h>
#include <genvtx/version.h>
#include <mctools/version.h>
#if BAYEUX_WITH_LAHAGUE == 1
#include <lahague/version.h>
#include <lahague/resource.h>
#include <lahague/lahague_config.h>
#endif // BAYEUX_WITH_LAHAGUE == 1
#include <datatools/kernel.h>
#include <datatools/library_info.h>
#include <datatools/urn_to_path_resolver_service.h>

namespace bayeux {

  namespace detail {

    // static
    bayeux_library * bayeux_library::_instance_ = nullptr;

    // static
    const std::string & bayeux_library::bx_resource_resolver_name()
    {
      static const std::string _n("bxLibraryResourceResolver");
      return _n;
    }

    datatools::logger::priority bayeux_library::process_logging_env()
    {
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      char * l = getenv("BAYEUX_SYS_LOGGING");
      if (l != nullptr) {
        std::string level_label(l);
        ::datatools::logger::priority prio
            = ::datatools::logger::get_priority(level_label);
        if (prio != ::datatools::logger::PRIO_UNDEFINED) {
          logging = prio;
        } else {
          // DT_LOG_WARNING(::datatools::logger::PRIO_WARNING,
          //                "Ignoring invalid BAYEUX_SYS_LOGGING=\"" << getenv("BAYEUX_SYS_LOGGING") << "\" environment!");
          logging = ::datatools::logger::PRIO_FATAL;
        }
      }
      return logging;
    }

    // static
    bool bayeux_library::is_instantiated()
    {
      return _instance_ != nullptr;
    }

    // static
    bayeux_library & bayeux_library::instance()
    {
      return *_instance_;
    }

    // static
    const bayeux_library & bayeux_library::const_instance()
    {
      return *_instance_;
    }

    // static
    bayeux_library & bayeux_library::instantiate()
    {
      if (! bayeux_library::is_instantiated()) {
        static std::unique_ptr<bayeux_library> _bxlib_handler;
        if (! _bxlib_handler) {
          // Allocate the Bayeux library singleton and initialize it:
          _bxlib_handler.reset(new bayeux_library);
        }
      }
      return bayeux_library::instance();
    }

    bool bayeux_library::is_initialized() const
    {
      return _initialized_;
    }

    bayeux_library::bayeux_library()
    {
      _logging_ = bayeux_library::process_logging_env();
      bayeux::init_directories();
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(bayeux_library::_instance_ != nullptr, std::logic_error, "Bayeux library singleton is already set!");
      bayeux_library::_instance_ = this;
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    bayeux_library::~bayeux_library()
    {
      _logging_ = bayeux_library::process_logging_env();
      DT_LOG_TRACE_ENTERING(_logging_);
      if (is_initialized()) {
        DT_LOG_TRACE(_logging_, "Shuting down Bayeux library system singleton...");
        shutdown();
      }
      bayeux_library::_instance_ = nullptr;
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    datatools::logger::priority bayeux_library::get_logging() const
    {
      return _logging_;
    }

    void bayeux_library::initialize()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Bayeux library system singleton is already initialized!");

      // Register library informations in the Bayeux/datatools' kernel:
      _libinfo_registration_();

      // Setup services:
      DT_LOG_TRACE(_logging_, "Starting Bayeux library services...");
      DT_LOG_TRACE_ENTERING(_logging_);
      _services_.set_name("bxLibrarySystemServices");
      _services_.set_description("Bayeux Library System Singleton Services");
      _services_.set_allow_dynamic_services(true);
      _services_.initialize();

      // Start URN services:
      _initialize_urn_services_();

      _initialized_ = true;
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void bayeux_library::shutdown()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Bayeux library system singleton is not initialized!");
      _initialized_ = false;

      // Terminate services:
      if (_services_.is_initialized()) {
        _shutdown_urn_services_();
        DT_LOG_TRACE(_logging_, "Terminating Bayeux library system singleton services...");
        _services_.reset();
      }

      // Deregister library informations from the Bayeux/datatools' kernel:
      _libinfo_deregistration_();

      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void bayeux_library::_libinfo_registration_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);

      DT_THROW_IF(!datatools::kernel::is_instantiated(),
                  std::runtime_error,
                  "The Bayeux/datatools' kernel is not instantiated !");
      datatools::kernel & krnl = datatools::kernel::instance();
      DT_THROW_IF(!krnl.has_library_info_register(),
                  std::runtime_error,
                  "The Bayeux/datatools' kernel has no library infor register!");

      // Populate the library info register, basically dumb if we don't
      // have it so assume it exists and hope for an exception if
      // it doesn't
      datatools::library_info & lib_info_reg = krnl.grab_library_info_register();

      // Bundled submodules:
      DT_LOG_TRACE(_logging_, "Populating the kernel library info register...");
      {

        // datatools:
        datatools::properties & datatools_lib_infos =
          lib_info_reg.registration("bxdatatools",
                                    "Serializable data structures based on Boost "
                                    "and general purpose utility classes and macros.",
                                    datatools::version::get_version());
        datatools_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                         datatools::get_resource_dir(false)
                                         );
        datatools_lib_infos.store_string(datatools::library_info::keys::env_resource_dir(),
                                         std::string(DATATOOLS_ENV_RESOURCE_DIR)
                                         );
        lib_info_reg.add_alias("datatools", "bxdatatools");
        DT_LOG_TRACE(_logging_, "Module bxdatatools is registered.");

        // cuts:
        lib_info_reg.registration("bxcuts",
                                  "Generic tools for making arbitrary data selections.",
                                  cuts::version::get_version());
        lib_info_reg.add_alias("cuts", "bxcuts");

        // materials:
        datatools::properties & materials_lib_infos
          = lib_info_reg.registration("bxmaterials",
                                      "Description of isotopes, elements and materials plus tools "
                                      "for input to simulation applications (i.e. GDML/Geant4).",
                                      materials::version::get_version()
                                      );
        materials_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                         materials::get_resource_dir(false)
                                         );
        materials_lib_infos.store_string(datatools::library_info::keys::env_resource_dir(),
                                         std::string(MATERIALS_ENV_RESOURCE_DIR)
                                         );
        lib_info_reg.add_alias("materials", "bxmaterials");
        DT_LOG_TRACE(_logging_, "Module bxmaterials is registered.");

        // mygsl:
        lib_info_reg.registration("bxmygsl",
                                  "C++ wrapper and extensions to the GNU Scientific Library.",
                                  mygsl::version::get_version());
        lib_info_reg.add_alias("mygsl", "bxmygsl");
        DT_LOG_TRACE(_logging_, "Module bxmygsl is registered.");

        // geomtools:
        datatools::properties & geomtools_lib_infos
          = lib_info_reg.registration("bxgeomtools",
                                      "Generic tools for working with experiment geometries and "
                                      "provide input to simulation tools (i.e. GDML/Geant4).",
                                      geomtools::version::get_version()
                                      );
        geomtools_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                         geomtools::get_resource_dir(false)
                                         );
        geomtools_lib_infos.store_string(datatools::library_info::keys::env_resource_dir(),
                                         std::string(GEOMTOOLS_ENV_RESOURCE_DIR)
                                         );
        lib_info_reg.add_alias("geomtools", "bxgeomtools");
        DT_LOG_TRACE(_logging_, "Module bxgeomtools is registered.");

        // emfield:
        lib_info_reg.registration("bxemfield",
                                  "Electromagnetic field modelling and management.",
                                  emfield::version::get_version());
        lib_info_reg.add_alias("emfield", "bxemfield");
        DT_LOG_TRACE(_logging_, "Module bxemfield is registered.");

        // brio:
        lib_info_reg.registration("bxbrio",
                                  "A Boost over ROOT I/O serialization system (extends datatools).",
                                  brio::version::get_version());
        lib_info_reg.add_alias("brio", "bxbrio");
        DT_LOG_TRACE(_logging_, "Module bxbrio is registered.");

        // dpp:
        lib_info_reg.registration("bxdpp",
                                  "A basic data processing pipeline API.",
                                  dpp::version::get_version());
        lib_info_reg.add_alias("dpp", "bxdpp");
        DT_LOG_TRACE(_logging_, "Module bxdpp is registered.");

        // genbb_help:
        datatools::properties & genbb_help_lib_infos
          = lib_info_reg.registration("bxgenbb_help",
                                      "C++ port and extension to the Decay0/GENBB program by Vladimir Tretyak, "
                                      "for input to simulation applications.",
                                      genbb::version::get_version()
                                      );
        genbb_help_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                          genbb::get_resource_dir(false)
                                          );
        genbb_help_lib_infos.store_string(datatools::library_info::keys::env_resource_dir(),
                                          std::string(GENBB_HELP_ENV_RESOURCE_DIR)
                                          );
        lib_info_reg.add_alias("genbb_help", "bxgenbb_help");
        lib_info_reg.add_alias("genbb", "bxgenbb_help");
        DT_LOG_TRACE(_logging_, "Module bxgenbb_help is registered.");

        // genvtx:
        lib_info_reg.registration("bxgenvtx",
                                  "Vertex random generator tools for input to simulation applications.",
                                  genvtx::version::get_version());
        lib_info_reg.add_alias("genvtx", "bxgenvtx");
        DT_LOG_TRACE(_logging_, "Module bxgenvtx is registered.");

        // mctools::
        lib_info_reg.registration("bxmctools",
                                  "Monte-Carlo utilities with a Geant4 interface.",
                                  mctools::version::get_version());
        lib_info_reg.add_alias("mctools", "bxmctools");
        DT_LOG_TRACE(_logging_, "Module bxmctools is registered.");

#if BAYEUX_WITH_LAHAGUE == 1
        // LaHague:
        datatools::properties & lahague_lib_infos
          = lib_info_reg.registration("bxlahague",
                                      "Dosimetry and radioprotection tools.",
                                      lahague::version::get_version()
                                      );
        lahague_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                       lahague::get_resource_dir(false)
                                       );
        lahague_lib_infos.store_string(datatools::library_info::keys::env_resource_dir(),
                                       std::string(LAHAGUE_ENV_RESOURCE_DIR)
                                       );
        lib_info_reg.add_alias("lahague", "bxlahague");
        DT_LOG_TRACE(_logging_, "Module bxlahague is registered.");
#endif // BAYEUX_WITH_LAHAGUE == 1

        // bayeux:
        datatools::properties& bayeux_lib_infos
          = lib_info_reg.registration("bxbayeux",
                                      "A C++ foundation library for particle/nuclear physics experiments.",
                                      bayeux::version::get_version());

        bayeux_lib_infos.store_string(datatools::library_info::keys::libname(),
                                      "Bayeux");
        bayeux_lib_infos.store_string(datatools::library_info::keys::install_lib_dir(),
                                      bayeux::get_library_dir());
        bayeux_lib_infos.store_string(datatools::library_info::keys::install_include_dir(),
                                      bayeux::get_prefix_dir() + "/@CMAKE_INSTALL_INCLUDEDIR@");
        bayeux_lib_infos.store_string(datatools::library_info::keys::install_bin_dir(),
                                      bayeux::get_binary_dir());
        bayeux_lib_infos.store_string(datatools::library_info::keys::install_data_dir(),
                                      bayeux::get_data_dir());
        bayeux_lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                                      bayeux::get_bayeux_resource_dir());
        bayeux_lib_infos.store_string(datatools::library_info::keys::env_resource_dir(),
                                      std::string(BAYEUX_ENV_RESOURCE_DIR));
        bayeux_lib_infos.store_integer(datatools::library_info::keys::version_major(),
                                       bayeux::version::get_major());
        bayeux_lib_infos.store_integer(datatools::library_info::keys::version_minor(),
                                       bayeux::version::get_minor());
        bayeux_lib_infos.store_integer(datatools::library_info::keys::version_patch(),
                                       bayeux::version::get_patch());
        bayeux_lib_infos.store_string(datatools::library_info::keys::modules(),
                                      "@BAYEUX_SUBMODULES@");
        bayeux_lib_infos.store_string(datatools::library_info::keys::url(),
                                      "https://nemo.lpc-caen.in2p3.fr/wiki/Software/Bayeux");
        bayeux_lib_infos.store_string(datatools::library_info::keys::authors(),
                                      "François Mauger, Xavier Garrido, Ben Morgan, Benoit Guillon, Arnaud Chapon");
        bayeux_lib_infos.store_string(datatools::library_info::keys::copyright(),
                                      "GNU General Public License version 3 or later");
        lib_info_reg.add_alias("bayeux", "bxbayeux");
        DT_LOG_TRACE(_logging_, "Module bxbayeux is registered.");

        if (datatools::logger::is_trace(_logging_) || datatools::logger::is_trace(lib_info_reg.get_logging())) {
          std::ofstream fli("bayeux_libinfo.txt");
          fli << "Status of the library info register: " << std::endl;
          lib_info_reg.tree_dump(fli, "", "[trace] ");
        }
        krnl.register_resource_paths();
        krnl.register_configuration_variant_registries();
        DT_LOG_TRACE(_logging_, "Kernel library info register has been populated.");

      }

      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void bayeux_library::_libinfo_deregistration_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);

      if (datatools::kernel::is_instantiated()) {
        datatools::kernel & krnl = datatools::kernel::instance();
        if (krnl.has_library_info_register()) {
          // Access to the datatools kernel library info register:
          datatools::library_info & lib_info_reg = krnl.grab_library_info_register();
          // Unregistration of all registered submodules
          // from the kernel's library info register:
          DT_LOG_TRACE(_logging_, "Deregistration of the Bayeux library modules from the Bayeux/datatools' kernel...");
          if (lib_info_reg.is_library("bxbayeux")) lib_info_reg.unregistration("bxbayeux");

#if BAYEUX_WITH_LAHAGUE == 1
          if (lib_info_reg.is_library("bxlahague")) lib_info_reg.unregistration("bxlahague");
#endif // BAYEUX_WITH_LAHAGUE == 1

#if BAYEUX_WITH_MCTOOLS == 1
          if (lib_info_reg.is_library("bxmctools")) lib_info_reg.unregistration("bxmctools");
#endif // BAYEUX_WITH_MCTOOLS == 1

          if (lib_info_reg.is_library("bxgenvtx")) lib_info_reg.unregistration("bxgenvtx");

          if (lib_info_reg.is_library("bxgenbb_help")) lib_info_reg.unregistration("bxgenbb_help");

          if (lib_info_reg.is_library("bxdpp")) lib_info_reg.unregistration("bxdpp");

          if (lib_info_reg.is_library("bxbrio")) lib_info_reg.unregistration("bxbrio");

          if (lib_info_reg.is_library("bxemfield")) lib_info_reg.unregistration("bxemfield");

          if (lib_info_reg.is_library("bxgeomtools")) lib_info_reg.unregistration("bxgeomtools");

          if (lib_info_reg.is_library("bxmygsl")) lib_info_reg.unregistration("bxmygsl");

          if (lib_info_reg.is_library("bxmaterials")) lib_info_reg.unregistration("bxmaterials");

          if (lib_info_reg.is_library("bxcuts")) lib_info_reg.unregistration("bxcuts");

          if (lib_info_reg.is_library("bxdatatools")) lib_info_reg.unregistration("bxdatatools");

          DT_LOG_TRACE(_logging_, "Bayeux library modules have been deregistered from the Bayeux/datatools' kernel.");
        }
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void bayeux_library::_initialize_urn_services_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      if (_services_.is_initialized()) {
        // Activate an URN resolver service:
        {
          datatools::urn_to_path_resolver_service & urnResourceResolver =
            dynamic_cast<datatools::urn_to_path_resolver_service &>(_services_.load_no_init(bx_resource_resolver_name(),
                                                                                            "datatools::urn_to_path_resolver_service"));
          urnResourceResolver.add_known_category("data");
          urnResourceResolver.add_known_category("graphics");
          urnResourceResolver.add_known_category("configuration");
          urnResourceResolver.add_map("@bxmaterials:urn/resolvers/bxmaterials_data_urn_map.conf");
          urnResourceResolver.add_map("@bxbayeux:urn/resolvers/bayeux_logos_urn_map.conf");
          urnResourceResolver.initialize_simple();
          DT_LOG_TRACE(_logging_, "Adding the URN path resolver '" << urnResourceResolver.get_name() << "' to the kernel...");
          urnResourceResolver.kernel_push();
        }
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void bayeux_library::_shutdown_urn_services_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);

      if (_services_.is_initialized()) {

        // Deactivate the URN resolver:
        if (_services_.has(bx_resource_resolver_name())) {
          DT_LOG_TRACE(_logging_, "Accessing URN path resolver...");
          datatools::urn_to_path_resolver_service & urnResourceResolver =
            _services_.grab<datatools::urn_to_path_resolver_service &>(bx_resource_resolver_name());
          DT_LOG_TRACE(_logging_, "Removing URN path resolver '" << urnResourceResolver.get_name() << "' from the kernel...");
          urnResourceResolver.kernel_pop();
          DT_LOG_TRACE(_logging_, "URN path resolver has been removed from the kernel.");
          urnResourceResolver.reset();
        }

      }

      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

  } // end of namespace detail

} // end of namespace bayeux
