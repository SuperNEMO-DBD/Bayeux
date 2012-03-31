// -*- mode: c++; -*- 
/* i_placement.cc 
 */

#include <geomtools/i_placement.h>
#include <geomtools/placement.h>

namespace geomtools {

  using namespace std;  

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(i_placement, "geomtools::i_placement")

  // ctor:
  i_placement::i_placement ()
  {
    return;
  }

  // dtor:
  i_placement::~i_placement ()
  {
    return;
  }
 
  bool i_placement::has_only_one_rotation () const
  {
    return false;
  }

  bool i_placement::is_multiple () const
  {
    return get_number_of_items () > 0;
  }
    
  placement i_placement::get_placement (int item_) const
  {
    placement p;
    get_placement (item_, p);
    return p;
  }

  void i_placement::tree_dump (ostream & out_, 
                               const string & title_, 
                               const string & indent_, 
                               bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) 
      {
        out_ << indent << title_ << std::endl;
      }
    out_ << indent << i_tree_dumpable::inherit_tag (inherit_)  
         << "Number of items  = " << get_number_of_items () << std::endl;
    return;
  }
 
} // end of namespace geomtools

// end of i_placement.cc
