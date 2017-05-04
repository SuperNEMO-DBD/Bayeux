/// \file geomtools/i_shape_2d.ipp

#ifndef GEOMTOOLS_I_SHAPE_2D_IPP
#define GEOMTOOLS_I_SHAPE_2D_IPP 1

// Ourselves:
#include <geomtools/i_shape_2d.h>

// Third party:
// - Boost:
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools:
#include <geomtools/i_object_3d.ipp>

namespace geomtools {

  /// Boost serialization template method
  template<class Archive>
  void i_shape_2d::serialize (Archive & ar_ , const unsigned int /* version_ */)
  {
    ar_ & boost::serialization::make_nvp("geomtools__i_object_3d",
                                          boost::serialization::base_object<geomtools::i_object_3d>(*this));
    return;
  }

} // end of namespace geomtools

#endif // GEOMTOOLS_I_SHAPE_2D_IPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
