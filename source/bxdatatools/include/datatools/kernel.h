/// \file datatools/kernel.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-09-26
 * Last modified: 2014-09-28
 *
 * License:
 *
 * Copyright (C) 2013-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *  The datatools kernel singleton with embedded resources and components.
 *
 */

#ifndef DATATOOLS_KERNEL_H
#define DATATOOLS_KERNEL_H

// Standard Library:
#include <iostream>

// Third Party:
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>

// This project:
#include <datatools/logger.h>
#include <datatools/i_tree_dump.h>
#include <datatools/datatools_config.h>

namespace datatools {

  class library_info;

  namespace configuration {
    class variant_repository;
  }

  /// \brief The datatools kernel singleton with embedded resources and components.
  class kernel :
    private boost::noncopyable,
    public ::datatools::i_tree_dumpable
  {

  public:

    /// \brief The set of configuration parameters for the datatools' kernel
    struct param_type {
      bool                     help; //!< Flag to print the help about datatools' command line options
      std::string              logging_label; //!< Logging priority label
      std::string              library_info_logging_label; //!< Logging priority label for the embedded library information registry
      bool                     inhibit_library_info_register; //!< Flag to inhibit the use of the embedded library information registry
      bool                     inhibit_variant_repository; //!< Flag to inhibit the use of the variant repository
      std::vector<std::string> unrecognized_args; //!< List of unrecognized arguments
      std::vector<std::string> resource_paths; //!< List of resource paths directives
      std::vector<std::string> variant_configs; //!< List of configuration files for the variant
      std::string              variant_load; //!< Path to the file from which one loads the values of the variant parameters
      std::string              variant_store; //!< Path to the file where one stores the values of the variant parameters
      std::vector<std::string> variant_sets; //!< List of directives to set the values of the variant parameters

      bool                     splash; //!< Flag to display the datatools splash screen
#if DATATOOLS_WITH_QT_GUI == 1
      bool                     variant_gui; //!< Flag to run the variant GUI
#endif // DATATOOLS_WITH_QT_GUI == 1
      /// Constructor
      param_type();
      /// Reset
      void reset();
    };

    /// Default constructor
    kernel();

    /// Constructor with on-the-fly initialization
    kernel(int argc_, char * argv_[]);

    /// Destruction with on-the-fly shutdown if needed
    virtual ~kernel();

    /// Initialization from command line arguments
    void initialize(int argc_, char * argv_[]);

    /// Shutdown
    void shutdown();

    /// Check initialization status
    bool is_initialized() const;

    /// Return the logging priority threshold
    datatools::logger::priority get_logging() const;

    /// Set the logging priority threshold
    void set_logging(datatools::logger::priority);

    /// Check if the library/component info register is available
    bool has_library_info_register() const;

    /// Return a mutable reference to the library/component info register
    library_info & grab_library_info_register();

    /// Return a non mutable reference to the library/component info register
    const library_info & get_library_info_register() const;

    /// Check if the configuration variant repository is available
    bool has_variant_repository() const;

    /// Return a non mutable reference to the configuration variant repository
    const configuration::variant_repository & get_variant_repository() const;

    /// Return a mutable reference to the configuration variant repository
    configuration::variant_repository & grab_variant_repository();

    /// Return the name of the current application
    const std::string & get_application_name() const;

    /// Check if the name of the current application is defined
    bool has_application_name() const;

    /// Check if the kernel singleton is available
    static bool is_instantiated();

    /// Return a mutable reference to the kernel singleton
    static kernel & instance();

    /// Instantiate and return a mutable reference to the kernel singleton
    static kernel & instantiate();

    /// Return a non mutable reference to the kernel singleton
    static const kernel & const_instance();

    /// Build description of options
    static void build_opt_desc(boost::program_options::options_description &, param_type &);

    /// Print splash screen
    static void print_splash(std::ostream & = std::clog);

    /// Print description of options
    static void print_opt_desc(const boost::program_options::options_description &,
                               std::ostream & = std::clog,
                               const std::string & indent_ = "");

    /// Smart print
    void tree_dump(std::ostream& out_ = std::clog,
                   const std::string& title_ = "",
                   const std::string& indent_ = "",
                   bool inherit_ = false) const;

  protected:

    /// Initialization
    void _initialize(const param_type & params_);

  private:

    /// Basic construction
    void _construct_();

    bool             _initialized_; //!< Initialization flag
    logger::priority _logging_;     //!< Logging priority
    bool             _activate_library_info_register_; //!< Activation flag for Library/component information register
    bool             _activate_variant_repository_;    //!< Activation flag for configuration variant repository
    boost::scoped_ptr<library_info> _library_info_register_; //!< Library/component information register
    boost::scoped_ptr<configuration::variant_repository> _variant_repository_; //!< Variant repository
    std::string     _application_name_; //!< The name of the current application
    static kernel * _instance_;         //!< Singleton handle

  };

} // end of namespace datatools

#endif // DATATOOLS_KERNEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
