//! \file    datatools/urn_to_path_resolver_service.h
//! \brief   URN to path resolver service
//! \details
//
// Copyright (c) 2017 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen
//
// This file is part of Bayeux/datatools.
//
// Bayeux/datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_URN_TO_PATH_RESOLVER_SERVICE_H
#define DATATOOLS_URN_TO_PATH_RESOLVER_SERVICE_H

// Standard Library
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <set>

// Third Party
// - Bayeux
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/base_service.h>
#include <datatools/service_manager.h>

namespace datatools {

  //! \brief URN to file path resolver service
  //!
  //! A service which associated a file path to its unique URN.
  //! URN format:
  //! \code
  //! urn:<SEGMENTS>
  //! \endcode
  //! Examples:
  //! \code
  //! urn:snemo:demonstrator:geometry:4.0:variants
  //! urn:bipo:geometry:2.3:plugins
  //! \endcode
  //!
  class urn_to_path_resolver_service
    : public ::datatools::base_service
  {
  public:

    //! Return the default list of known categories
    static const std::set<std::string> & default_known_categories();

    //! \brief Resolver map entry
    struct resolver_entry {
      std::string urn;        //!< URN representation
      std::string category;   //!< Resource file category
      std::string path;       //!< Path
      std::string mime;       //!< MIME type
    };

    //! \brief URN to path lookup table
    typedef std::map<std::string, resolver_entry> urn_lookup_table_type;

    //! Constructor
    urn_to_path_resolver_service();

    //! Destructor
    virtual ~urn_to_path_resolver_service();

    //! Add a knonw catagory
    void add_known_category(const std::string & cat_);

    //! Return the known categories
    const std::set<std::string> & get_known_categories() const;

    //! Check if URN overloading is allowed
    bool is_allow_overloading() const;

    //! Set the URN overloading flag
    void set_allow_overloading(bool);

    //! Add a map file
    void add_map(const std::string &);

    //! Check initialization status
    virtual bool is_initialized() const;

    //! Initialize the service from a list of properties
    virtual int initialize(const datatools::properties & config_,
                           datatools::service_dict_type & services_);

    //! Reset
    virtual int reset();

    /// Smart print
    virtual void tree_dump(std::ostream& out = std::clog,
                           const std::string & title = "",
                           const std::string & indent = "",
                           bool inherit = false) const;

    //! Load a URN/path map file
    void load_map(const std::string & map_filename_);

    //! Check is a given URN is known
    bool urn_is_known(const std::string & urn_) const;

    //! Return the path associated to a given URN
    std::string resolve(const std::string & urn_) const;

    //! Add entry in the URN path lockup table
    void add_entry(const std::string & urn_,
                   const std::string & category_,
                   const datatools::properties & infos_);

    //! Add entry in the URN path lockup table
    void add_entry(const std::string & urn_,
                   const std::string & category_,
                   const std::string & path_,
                   const std::string & mime_ = "");

    //! Remove an entry
    void remove(const std::string & urn_);

    //! Return the size of the lookup table
    std::size_t size() const;

    //! Return the entry associated to a given URN
    const resolver_entry & get_entry(const std::string & urn_) const;

    //! Return the category associated to a given URN
    const std::string & get_category(const std::string & urn_) const;

    //! Return the path associated to a given URN
    const std::string & get_path(const std::string & urn_) const;

    //! Return the MIME associated to a given URN
    const std::string & get_mime(const std::string & urn_) const;

    //! Print the URN lookup table
    void print_lookup_table(std::ostream & = std::clog, const uint32_t flags_ = 0) const;

    //! Return the URN lookup table
    const urn_lookup_table_type & get_urn_lookup_table() const;

  private:

    void _init_();

    void _reset_();

  private:

    // Management:
    bool _initialized_ = false;

    // Configuration:
    bool _allow_overloading_ = false;
    std::vector<std::string> _map_filenames_;
    std::set<std::string> _known_categories_;

    // Private data:
    urn_lookup_table_type _urn_lookup_table_;

    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(urn_to_path_resolver_service)

  };

}

#endif // DATATOOLS_URN_TO_PATH_RESOLVER_SERVICE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
