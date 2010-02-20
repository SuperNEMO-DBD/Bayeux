// -*- mode: c++; -*- 
/* line_3d.cc
 */

#include <geomtools/line_3d.h>

namespace geomtools {

  using namespace std;

  const string line_3d::SERIAL_TAG = "__geomtools::line_3d__";
  const string line_3d::LINE_3D_LABEL = "line_3d";

  const string & 
  line_3d::get_serial_tag () const
  {
    return line_3d::SERIAL_TAG;
  }

  string line_3d::get_shape_name () const
  {
    return line_3d::LINE_3D_LABEL;
  }

  bool 
  line_3d::is_valid () const
  {
    return (geomtools::is_valid (__first) && geomtools::is_valid (__last));
  }
  
  void 
  line_3d::invalidate ()
  {
    geomtools::invalidate (__first);
    geomtools::invalidate (__last);
  }
  
  const vector_3d & 
  line_3d::get_first () const
  {
    return __first;
  }
  
  void 
  line_3d::set_first (const vector_3d & new_value_)
  {
    __first = new_value_;
  }
  
  const vector_3d & 
  line_3d::get_last () const
  {
    return __last;
  }
  
  void 
  line_3d::set_last (const vector_3d & new_value_)
  {
    __last = new_value_;
  }
  
  double line_3d::get_length () const
  {
    double l = (__last - __first).mag ();
    return l;
  }
  
  // ctor/dtor:
  line_3d::line_3d () : i_shape_1d ()
  {
    invalidate ();
  }

  // ctor/dtor:
  line_3d::line_3d (const vector_3d & first_, const vector_3d & last_)
  {
    set_first (first_);
    set_last (last_);
  }
  
  line_3d::~line_3d ()
  {
  }

  vector_3d 
  line_3d::get_point (double t_) const
  {
    vector_3d p = __first + (__last - __first) * t_;
    return vector_3d (p);
  }

  void 
  line_3d::make_vertex_collection (basic_polyline_3d & bpl_) const
  {
    bpl_.clear ();
    bpl_.push_back (__first);
    bpl_.push_back (__last);
  }

  basic_polyline_3d 
  line_3d::make_vertex_collection () const
  {
    basic_polyline_3d bpl;
    make_vertex_collection (bpl);
    return bpl;
  }
  
  void line_3d::tree_dump (ostream &      out_, 
			   const string & title_,
			   const string & indent_,
			   bool           inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ())
      {
        indent = indent_;
      }
    i_object_3d::tree_dump (out_, title_, indent_, true);
    out_ << indent << du::i_tree_dumpable::tag 
	 << "First : " 
	 << __first 
	 << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
	 << "Last : " 
	 << __last
	 << endl;
    return;
  }

  bool line_3d::is_on_curve (const vector_3d & position_, 
			     double tolerance_) const
  {
    bool on_curve = false;
    throw runtime_error ("line_3d::is_on_curve: Not implemented yet !");
    return on_curve;
  }

  vector_3d line_3d::get_direction_on_curve (const vector_3d & position_) const
  {
    vector_3d dir;
    throw runtime_error ("line_3d::get_direction_on_curve: Not implemented yet !");
    return dir;
  }

  /*
  void 
  line_3d::print_xyz (ostream & out_, 
		      const line_3d & line_,
		      double step_)
  {
    double delta_t = 0.01;
    if (step_ > 0.0) delta_t = step_;
    for (double t = 0;
	 t < 1.0 + 0.001 * delta_t;
	 t += delta_t)
      {
	vector_3d v = line_.get_point (t);
	vector_3d::print_xyz (out_, v);
      }
    out_ << endl << endl;
  }
  */
  
} // end of namespace geomtools

// end of line_3d.cc
