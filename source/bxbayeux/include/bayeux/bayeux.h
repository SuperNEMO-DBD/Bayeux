//! \file    bayeux/bayeux.h
//! \brief   Provide system init for Bayeux internals
//! \details
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2013-2017 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013-2017 by Université de Caen
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
//

#ifndef BAYEUX_BAYEUX_H
#define BAYEUX_BAYEUX_H

// Standard Library:
#include <string>

// Third Party:
// - Boost:
#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>

// This project:
#include <datatools/logger.h>
#include <bayeux/bayeux_init.h>

/// Top-level namespace of the Bayeux library
namespace bayeux {

  /// Check if Bayeux library core functionnalities are initialized
  bool is_initialized();

  /// Initialize the Bayeux library core functionnalities
  void initialize(int argc_ = 0,
                  char * argv_[] = nullptr,
                  uint32_t flags_ = 0);

  /// Terminate the Bayeux library core functionnalities
  void terminate();

}

#endif // BAYEUX_BAYEUX_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
