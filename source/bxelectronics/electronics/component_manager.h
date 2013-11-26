// -*- mode: c++; -*-
/* component_manager.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-11-12
 * Last modified : 2013-11-13
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A component manager.
 *
 * History:
 *
 */
#ifndef ELECTRONICS_COMPONENT_MANAGER_H_
#define ELECTRONICS_COMPONENT_MANAGER_H_

// Standard Library
#include <string>
#include <sstream>
#include <typeinfo>
#include <stdexcept>

// Third Party
// - Boost
#include <boost/cstdint.hpp>

// - Bayeux/datatools
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/bit_mask.h>

// This project
#include <electronics/component_utils.h>
#include <electronics/component_base.h>

namespace datatools {

  class properties;
  class multi_properties;

}

namespace electronics {

  /*! \brief Electronics component management class
   *
   */
  class component_manager : public datatools::i_tree_dumpable {
  public:
    enum flag_type {
      BLANK              = 0,
      NO_PRELOAD         = datatools::bit_mask::bit00,
      FORCE_INITIALIZATION_AT_LOAD = datatools::bit_mask::bit01,
      FACTORY_VERBOSE    = datatools::bit_mask::bit02
    };

  public:
    /// Constructor
    component_manager(const std::string& name = "",
                      const std::string & description = "",
                      uint32_t flag = BLANK);

    /// Destructor
    ~component_manager();

    /// Set the name of the component manager
    void set_name(const std::string& name);

    /// Set the description of the component manager
    void set_description(const std::string& description);

    /// Set the version of the component manager
    void set_version(const std::string& version);

    /// Get the name of the component manager
    const std::string& get_name() const;

    /// Get the description of the component manager
    const std::string& get_description() const;

    /// Get the version of the component manager
    const std::string& get_version() const;

    /// Check the debug flag
    bool is_debug() const;

    /// Set the debug flag
    void set_debug(bool debug = true);

    /// Check the initialization flag
    bool is_initialized() const;

    /// Initialize the manager
    void initialize();

    /// Initialize the manager from a container of properties
    void initialize(const datatools::properties& config);

    /// Reset the manager
    void reset();

    bool has_component_type(const std::string& id) const;

    template <class ComponentClass>
    void register_component_type(const std::string& id);

    void unregister_component_type(const std::string& id);

    /**  @param name The name of the component to be checked
     *   @return true if the manager hosts the component requested by name
     */
    bool has(const std::string& name) const;

    /**  @param name The name of the component to be checked
     *   @return true if the component is initialized
     */
    bool is_initialized(const std::string& name) const;

    /**  @param name The name of the component to be checked
     *   @return true if the component is of the requested type
     */
    template <class T>
    bool is_a(const std::string& name) const;

    /** Deprecated, please use the 'grab' method.
     *  @param name The name of the component to be checked
     *  @return a mutable reference to the component instance requested by name and type
     */
    template<class T>
    T& get(const std::string& name);

    /**  Same as the mutable 'get' method
     *   @param name The name of the component to be checked
     *   @return a mutable reference to the component instance requested by name and type
     */
    template<class T>
    T& grab(const std::string& name);

    /**  @param name The name of the component to be checked
     *   @return a const reference to the component instance requested by name and type
     */
    template<class T>
    const T& get(const std::string& name) const;

    void load(const std::string& name,
              const std::string& id,
              const datatools::properties& config);

    void load(const datatools::multi_properties& config);

    const component_pool_type& get_components() const;

    component_pool_type& grab_components();

    void dump_components(std::ostream& out = std::clog,
                         const std::string& title  = "",
                         const std::string& indent = "") const;

    /// Smart print
    virtual void tree_dump(std::ostream& out         = std::clog,
                           const std::string& title  = "",
                           const std::string& indent = "",
                           bool inherit              = false) const;

    void set_logging_priority(datatools::logger::priority);

    datatools::logger::priority get_logging_priority() const;

    void load_component(const std::string& name,
                        const std::string& id,
                        const datatools::properties& config);

    void create_component(component_entry& entry);

    void initialize_component(component_entry& entry);

    void reset_component(component_entry& entry);

  protected:

    void preload_global_dict();

  private:
    /// Set the factory preload flag
    void set_preload(bool preload);

  private:

    datatools::logger::priority _logging_priority; /// Logging priority threshold
    bool         initialized_; /// Initialization flag
    std::string  name_; /// Manager's name
    std::string  description_; /// Manager's description
    bool         preload_; /// Factory preload flag
    bool         force_initialization_at_load_; /// Flag for triggering component  initialization at load (rather than first use)

    // 2012-04-09 FM : support for datatools::factory system :
    component_base::factory_register_type  factory_register_;
    component_pool_type                    components_; /// Dictionary of components
  };

} // end of namespace electronics

#include <electronics/component_manager-inl.h>

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(electronics::component_manager)

#endif // ELECTRONICS_COMPONENT_MANAGER_H_