/// file datatools/real_range.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-09-25
 * Last modified: 2014-08-31
 *
 * License:
 *
 * Copyright (C) 2011-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Real range/interval class.
 *
 */

#ifndef DATATOOLS_REAL_RANGE_H
#define DATATOOLS_REAL_RANGE_H

// Standard Library:
#include <iostream>
#include <string>
#include <set>

// This Project:
#include <datatools/range_tools.h>

namespace datatools {

  //! \brief A class representing an interval of real (double precision) values with support of embedded units.
  class real_range
  {
  public:

    /// Default constructor
    real_range();

    /// Constructor
    real_range(double from_, double to_,
               range_bound_info_type from_policy_ = range_bound_included,
               range_bound_info_type to_policy_ = range_bound_included);

    /// Check the validity of the interval
    bool is_valid() const;

    /// Invalidate the interval
    void invalidate();

    /// Reset lower bound
    void reset_lower();

    /// Reset upper bound
    void reset_upper();

    /// Reset the interval
    void reset();

    /// Set the lower bound
    void set_lower(double from_, range_bound_info_type policy_ = range_bound_included);

    /// Set the upper bound
    void set_upper(double to_, range_bound_info_type policy_ = range_bound_included);

    /// Set the bounds
    void set(double from_,
             double to_,
             range_bound_info_type from_policy_ = range_bound_included,
             range_bound_info_type to_policy_   = range_bound_included);

    /// Check if lower bound is defined
    bool is_lower_bounded() const;

    /// Check if upper bound is defined
    bool is_upper_bounded() const;

    /// Check if the intervalis half-bounded
    bool is_half_bounded() const;

    /// Check if both bounds are defined
    bool is_bounded() const;

    /// Check if lower bound is included
    bool is_lower_included() const;

    /// Check if upper bound is included
    bool is_upper_included() const;

    /// Return lower bound
    double get_lower() const;

    /// Return upper bound
    double get_upper() const;

    /// Return the width of the interval
    double width(double tolerance_ = -1.0) const;

    /// Check if interval is empty
    bool is_empty(double tolerance_ = -1.0) const;

    /// Check if the interval is degenerated (one unique value)
    bool is_singleton(double tolerance_ = -1.0) const;

    /// Return the singleton value
    double singleton(double tolerance_ = -1.0) const;

    // Collections of 'make' methods :

    /// Build a singleton
    // "[2;2]"
    void make_singleton(double);

    /// Build an empty interval
    // "(0;0)"
    void make_empty();

    /// Build a fully bounded positive interval starting at 0
    // "[0;)"
    void make_positive_unbounded();

    /// Build a fully bounded  negative interval ending at 0
    // "(;0]"
    void make_negative_unbounded();

    /// Build a half bounded interval with no upper bound
    // "[lower;)" or "(lower;[
    void make_upper_unbounded(double from_, bool inclusive_ = true);

    /// Build a half bounded interval with no lower bound
    // "(;upper]" or "(;upper)"
    void make_lower_unbounded(double to_, bool inclusive_ = true);

    /// Build a full unbounded interval with no lower or upper bounds
    // "(;)"
    void make_unbounded();

    /// Build a fully bounded interval with specific lower and upper bounds
    // "(lower;upper]" or "(lower;upper)"
    // "[lower;upper]" or "[lower;upper)"
    void make_bounded(double from_, double to_,
                      bool lower_included_ = true,
                      bool upper_included_ = true);

    /// Check if a value belongs to the interval
    bool has(double value_, double tolerance_ = -1.0) const;

    /// Check if an real interval belongs to the interval
    bool has(const real_range & rr_, double tolerance_ = -1.0) const;

    /// Basic print
    void dump(std::ostream & out_ = std::clog, double tolerance_ = -1.0) const;

    /// Return the first value belonging to the half lower bounded interval
    double first(double tolerance_ = -1.0) const;

    /// Return the last value belonging to the half upper bounded interval
    double last(double tolerance_ = -1.0) const;

    /// Check if unit label is set
    bool has_unit_label() const;

    /// Set the unit label
    void set_unit_label(const std::string &);

    /// Reset the unit label
    void reset_unit_label();

    /// Return the unit label
    const std::string & get_unit_label() const;

    /// Check if the preferred unit label is set
    bool has_preferred_unit() const;

    /// Reset the preferred unit symbol
    void reset_preferred_unit();

    /// Set the preferred unit symbol
    void set_preferred_unit(const std::string & pus_);

    /// Reset the preferred unit symbol
    const std::string & get_preferred_unit() const;

    /// Print operator using the ISO_31-11 standard
    friend std::ostream & operator<<(std::ostream & out_,
                                    const real_range & range_);

    /// Input operator using the ISO_31-11 standard
    friend std::istream & operator>>(std::istream & in_,
                                    real_range & range_);

    /// Given a relative tolerance, compute the absolute tolerance associated to bounds
    static double compute_tolerance(double lower_, double upper_,
                                    double relative_tolerance_ = -1.0);

    /// Compare intervals
    int compare(const real_range & range_) const;

    /// Comparison operator
    bool operator<(const real_range & range_) const;

    /// Comparison operator
    bool operator>(const real_range & range_) const;

    /// Comparison operator
    bool operator==(const real_range & range_) const;

  protected:

    /// Return an effective absolute tolerance
    double _effective_tolerance(double tolerance_) const;

    /// Set default attribute values
    void _set_defaults();

  private:

    std::string            _unit_label_;     //!< Unit label
    std::string            _preferred_unit_; //!< Symbol of the preferred unit
    range_bound_info_type  _lower_flag_;     //!< The flag associated to the lower bound
    double                 _lower_;          //!< The lower bound
    range_bound_info_type  _upper_flag_;     //!< The flag associated to the upper bound
    double                 _upper_;          //!< The upper bound
  };

} // end of namespace datatools

#endif // DATATOOLS_REAL_RANGE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
