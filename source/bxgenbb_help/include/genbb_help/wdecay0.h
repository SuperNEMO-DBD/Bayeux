/// \file genbb_help/wdecay0.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-04
 * Last modified: 2013-05-05
 *
 * License:
 * Copyright 2013 F. Mauger
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
 *   C++ wrapper GENBB/Decay0 C++ port generator.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_WDECAY0_H
#define GENBB_HELP_WDECAY0_H

// Standard library:
#include <iostream>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <genbb_help/i_genbb.h>

namespace genbb {

  /// Nested namespace of the Bayeux/genbb_help module library (decay0 C++ port)
  namespace decay0 {
    //struct bkgpars;
    struct bbpars;
  }

  /// \brief Decay0/GENBB (C++ port) generator wrapper
  class wdecay0 : public i_genbb
  {
  public:

    /// \brief Type of decay
    enum decay_type {
      DECAY_TYPE_UNDEFINED  = 0, //!< Undefined decay type
      DECAY_TYPE_DBD        = 1, //!< Double beta decay
      DECAY_TYPE_BACKGROUND = 2  //!< Radioactive background decay
    };

    /// Invalid decay mode
    static const int DBD_MODE_INVALID = -1;

  public:

    virtual bool can_external_random() const;

    const mygsl::rng & get_random() const;

    mygsl::rng & grab_random();

    size_t get_event_count() const;

    double get_to_all_events() const;

    wdecay0();

    virtual ~wdecay0();

    virtual void tree_dump(std::ostream& out = std::clog,
                           const std::string& title  = "",
                           const std::string& indent = "",
                           bool inherit = false) const;

    void dump(std::ostream & = std::clog) const;

    virtual void initialize(const datatools::properties & setup_,
                            datatools::service_manager & service_manager_,
                            detail::pg_dict_type & dictionary_);

    virtual void reset();

    virtual bool has_next();

    virtual bool is_initialized() const;

    genbb::decay0::bbpars & bb_params();

  protected:

    virtual void _load_next(primary_event & event_,
                            bool compute_classification_ = true);

  private:

    void _init_();

    void _reset_();

    void _set_decay_isotope_(const std::string & di_);

    void _set_decay_version_(int ver_);

  private:

    bool   _initialized_; //!< Initialization flag

    int    _decay_type_;  //!< Type of the decay
    std::string _decay_isotope_; //!< Decaying isotope
    int    _decay_version_;
    int    _decay_dbd_level_;
    int    _decay_dbd_mode_;
    size_t _event_count_;

    double _energy_min_;
    double _energy_max_;

    unsigned long _seed_;   //!< PRNG seed (local or global)
    mygsl::rng    _random_; //!< Local PRNG

    boost::scoped_ptr<genbb::decay0::bbpars> _bb_params_;

    GENBB_PG_REGISTRATION_INTERFACE(wdecay0)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::wdecay0)

#endif // GENBB_HELP_WDECAY0_H

// Local Variables: --
// mode: c++ --
// End: --
