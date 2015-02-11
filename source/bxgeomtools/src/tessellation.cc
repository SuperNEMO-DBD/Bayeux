// tessellation.cc

/*
http://www.blackpawn.com/texts/pointinpoly/default.html
http://realtimecollisiondetection.net/books/rtcd/
*/

// Ourselves:
#include <geomtools/tessellation.h>

// Standard library:
#include <sstream>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <string>

// Third party:
// - Boost:
#include <boost/spirit/include/support_istream_iterator.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/box.h>
#include <geomtools/stl_tools.h>

namespace geomtools {

  /* facet_vertex */

  bool facet_vertex::is_valid () const
  {
    return geomtools::is_valid (position) && ref_facets.size () > 2;
  }

  void facet_vertex::reset ()
  {
    geomtools::invalidate (position);
    ref_facets.clear ();
    return;
  }

  facet_vertex::facet_vertex ()
  {
    geomtools::invalidate (position);
    return;
  }

  facet_vertex::facet_vertex (double x_,
                              double y_,
                              double z_)
  {
    position.set (x_, y_, z_);
    return;
  }

  const vector_3d & facet_vertex::get_position () const
  {
    return position;
  }

  void facet_vertex::add_ref_facet (int facet_index_, int facet_node_)
  {
    ref_facets[facet_index_] = facet_node_;
    return;
  }

  void facet_vertex::print_xyz (std::ostream & out_, int color_) const
  {
    out_ << position.x () << ' ' << position.y () << ' ' << position.z () << ' ' << color_ << std::endl;
    return;
  }

  void facet_vertex::print (std::ostream & out_) const
  {
    out_ << '(' << position.x () << ' ' << position.y () << ' ' << position.z () << ')'
         << " #ref=" << ref_facets.size();
    if ( ref_facets.size()) {
        out_ << " facets=";
        for (std::map<int32_t, int32_t>::const_iterator ifr = ref_facets.begin ();
             ifr != ref_facets.end ();
             ifr++) {
            out_ << " " << ifr->first;
          }
      }
    out_ << std::endl;
    return;
  }

  std::ostream & operator<< (std::ostream & out_, const facet_vertex & vtx_)
  {
    out_ << "position=" << '(' << vtx_.position.x () << ';' << vtx_.position.y () << ';' << vtx_.position.z () << ')'
         << ";facets={";
    if ( vtx_.ref_facets.size  ()) {
        for (std::map<int32_t, int32_t>::const_iterator ifr = vtx_.ref_facets.begin ();
             ifr != vtx_.ref_facets.end ();
             ifr++) {
            if (ifr != vtx_.ref_facets.begin ())
              out_ << ";";
            out_ << ifr->first;
          }
      }
    out_ << "}";
    return out_;
  }

  /* facet34 */

  uint32_t facet34::get_number_of_vertices () const
  {
    return _number_of_vertices_;
  }

  const facet_vertex & facet34::get_vertex (int i_) const
  {
    DT_THROW_IF((i_ < 0) || (i_ >= (int)_number_of_vertices_),
                std::logic_error,
                "Invalid vertex index " << i_ << " !");
    return *_vertices_[i_];
  }

  int32_t facet34::get_vertex_key (int i_) const
  {
    DT_THROW_IF((i_ < 0) || (i_ >= (int)_number_of_vertices_),
                std::logic_error,
                "Invalid vertex index " << i_ << " !");
    return _vertices_keys_[i_];
  }

  const geomtools::vector_3d & facet34::get_normal () const
  {
    return _normal_;
  }

  bool facet34::is_valid () const
  {
    if (_number_of_vertices_ < 3) return false;
    if (_number_of_vertices_ < 4) return false;
    if (! has_normal ()) return false;
    return true;
  }

  bool facet34::has_normal () const
  {
    return geomtools::is_valid (_normal_);
  }

  void facet34::compute_normal ()
  {
    if (_number_of_vertices_ < 3 || _number_of_vertices_ > 4)
      {
        return;
      }
    const geomtools::vector_3d & v0 = _vertices_[0]->get_position ();
    const geomtools::vector_3d & v1 = _vertices_[1]->get_position ();
    const geomtools::vector_3d & v2 = _vertices_[2]->get_position ();
    const geomtools::vector_3d u01 = v1 - v0;
    const geomtools::vector_3d u12 = v2 - v1;
    geomtools::vector_3d n = u01.cross (u12);
    const double m = n.mag ();
    n /= m;
    if (_number_of_vertices_ == 4) {
        const geomtools::vector_3d & v3 = _vertices_[3]->get_position ();
        const geomtools::vector_3d u23 = v3 - v2;
        geomtools::vector_3d n2 = u12.cross (u23);
        n2 /= m;
        if (! geomtools::are_near (n, n2, 1.e-5)) {
             invalidate (_normal_);
             return;
          }
      }
    _normal_ = n;
    return;
  }

  void facet34::_set_defaults ()
  {
    _number_of_vertices_ = 0;
    for (unsigned int i = 0; i < 4; i++) {
        _vertices_[i] = 0;
        _vertices_keys_[i] = -1;
        datatools::invalidate (_internal_angles_[i]);
      }
    geomtools::invalidate (_normal_);
    datatools::invalidate (_surface_tri_);
    _surface_tri_bis_ = 0.0;
    _category_ = INVALID_CATEGORY;
    return;
  }

  bool facet34::has_category () const
  {
    return _category_ > INVALID_CATEGORY;
  }

  void facet34::set_category (unsigned int c_)
  {
    DT_THROW_IF (c_ > MAX_CATEGORY, std::logic_error,"Invalid category '" << c_ << "' !");
    _category_ = c_;
  }

  void facet34::unset_category ()
  {
    _category_ = INVALID_CATEGORY;
    return;
  }

  facet34::facet34 ()
  {
    _set_defaults ();
    return;
  }

  void facet34::set_triangle (const facet_vertex & v0_,
                              const facet_vertex & v1_,
                              const facet_vertex & v2_,
                              int iv0_,
                              int iv1_,
                              int iv2_)
  {
    _number_of_vertices_ = 3;
    _vertices_[0] = &v0_;
    _vertices_[1] = &v1_;
    _vertices_[2] = &v2_;
    _vertices_[3] = 0;

    DT_THROW_IF ((! check_triangle (v0_.get_position (),
                                    v1_.get_position (),
                                    v2_.get_position ())),
                 std::logic_error,
                 "Invalid set of vertices for a triangle !");
    _vertices_keys_[0] = iv0_;
    _vertices_keys_[1] = iv1_;
    _vertices_keys_[2] = iv2_;
    _vertices_keys_[3] = -1;
    compute_internals ();
    return;
  }

  facet34::facet34 (const facet_vertex & v0_,
                    const facet_vertex & v1_,
                    const facet_vertex & v2_,
                    int iv0_,
                    int iv1_,
                    int iv2_)
  {
    _set_defaults ();
    set_triangle (v0_,
                  v1_,
                  v2_,
                  iv0_,
                  iv1_,
                  iv2_);
    return;
  }

  bool facet34::check_triangle (const geomtools::vector_3d & v0_,
                                const geomtools::vector_3d & v1_,
                                const geomtools::vector_3d & v2_,
                                double /*tolerance_*/)
  {
    // Check that all 3 vertices are different :
    const geomtools::vector_3d u01 = v1_ - v0_;
    if (u01.mag2 () == 0) {
        return false;
      }
    const geomtools::vector_3d u12 = v2_ - v1_;
    if (u12.mag2 () == 0) {
        return false;
      }
    const geomtools::vector_3d u20 = v0_ - v2_;
    if (u20.mag2 () == 0) {
        return false;
      }

    // Check non-alignment of vertices :

    // double tolerance = tolerance_;
    // if (tolerance_ == 0.0 || ! datatools::is_valid (tolerance_))
    //   {
    //     // use default (rather strict) tolerance :
    //     tolerance = 1.e-13;
    //   }

    // Vertices 0, 1, 2 :
    const geomtools::vector_3d n012 = u01.cross (u12);
    const double m012 = n012.mag ();
    if (m012 == 0.0) {
        DT_LOG_WARNING (datatools::logger::PRIO_WARNING, "Vertices 0, 1, 2 are aligned !");
        return false;
      }
    //n012 /= m012;
    return true;
  }

  bool facet34::check_quadrangle (const geomtools::vector_3d & v0_,
                                  const geomtools::vector_3d & v1_,
                                  const geomtools::vector_3d & v2_,
                                  const geomtools::vector_3d & v3_,
                                  double tolerance_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    // Check that all 4 vertices are different :
    const geomtools::vector_3d u01 = v1_ - v0_;
    if (u01.mag2 () == 0)
      {
        return false;
      }
    const geomtools::vector_3d u12 = v2_ - v1_;
    if (u12.mag2 () == 0)
      {
        return false;
      }
    const geomtools::vector_3d u23 = v3_ - v2_;
    if (u23.mag2 () == 0)
      {
        return false;
      }
    const geomtools::vector_3d u30 = v0_ - v3_;
    if (u30.mag2 () == 0)
      {
        return false;
      }
    const geomtools::vector_3d u02 = v2_ - v0_;
    if (u02.mag2 () == 0)
      {
        return false;
      }
    const geomtools::vector_3d u13 = v3_ - v1_;
    if (u13.mag2 () == 0)
      {
        return false;
      }

    // Vertices 0, 1, 2 :
    geomtools::vector_3d n012 = u01.cross (u12);
    {
      const double m012 = n012.mag ();
      if (m012 == 0.0)
        {
          DT_LOG_WARNING (local_priority, "Vertices 0, 1, 2 are aligned !");
          return false;
        }
      n012 /= m012;
    }

    // Vertices 1, 2, 3 :
    geomtools::vector_3d n123 = u12.cross (u23);
    {
      const double m123 = n123.mag ();
      if (m123 == 0.0)
        {
          DT_LOG_WARNING (local_priority, "Vertices 1, 2, 3 are aligned !");
          return false;
        }
      n123 /= m123;
    }

    // Vertices 2, 3, 0 :
    geomtools::vector_3d n230 = u23.cross (u30);
    {
      const double m230 = n230.mag ();
      if (m230 == 0.0)
        {
          DT_LOG_WARNING (local_priority, "Vertices 2, 3, 0 are aligned !");
          return false;
        }
      n230 /= m230;
    }

    // Vertices 3, 0, 1 :
    geomtools::vector_3d n301 = u30.cross (u01);
    {
      const double m301 = n301.mag ();
      if (m301 == 0.0)
        {
          DT_LOG_WARNING (local_priority, "Vertices 3, 0, 1 are aligned !");
          return false;
        }
      n301 /= m301;
    }

    // Check planarity of the quadrangle :
    DT_LOG_TRACE (local_priority, "n012=" << n012 << " vs n123=" << n123);

    // Check non-alignment of vertices :
    double tolerance = tolerance_;
    if (tolerance_ == 0.0 || ! datatools::is_valid (tolerance_))
      {
        // use default (rather strict) tolerance :
        tolerance = 1.e-6;
      }

    if (! n012.isNear (n123, tolerance))
      {
        DT_LOG_WARNING (local_priority, "Quadrangle is not convex !");
        return false;
      }

    return true;
  }

  void facet34::compute_internal_angles ()
  {
    // Not implemented yet :
    /*
    geomtools::vector_3d u01 = _vertices_[1]->get_position () - _vertices_[0]->get_position ();
    if (u01.mag2 () == 0)
      {
        return false;
      }
    geomtools::vector_3d u12 = _vertices_[2]->get_position () - _vertices_[1]->get_position ();
    if (u12.mag2 () == 0)
      {
        return false;
      }
    if (is_triangle ())
      {
      }
    else if (is_quadrangle ())
      {
    geomtools::vector_3d u23 = _vertices_[3]->get_position () - _vertices_[2]->get_position ();
    if (u23.mag2 () == 0)
      {
        return false;
      }
        geomtools::vector_3d u20 = _vertices_[2]->get_position () - _vertices_[0]->get_position ();
        if (u20.mag2 () == 0)
          {
            return false;
          }
    bool rev = false;
    double a012 = (-u01).angle (u12);
    if (a012 > M_PI)
      {
        a012 = 2 * M_PI - a012;
        if (devel)
          {
            std::cerr << "DEVEL: geomtools::facet34::compute_internal_angles: "
                      << "reverse flag !"
                      << std::endl;
          }
        rev = true;
      }
    if (devel)
      {
        std::cerr << "DEVEL: geomtools::facet34::compute_internal_angles: "
                  << "a012=" << a012 / CLHEP::degree << " °"
                  << std::endl;
      }
    double a123 = (-u12).angle (u23);
    if (rev)
      {
        a123 = 2 * M_PI - a123;
      }
    if (devel)
      {
        std::cerr << "DEVEL: geomtools::facet34::compute_internal_angles: "
                  << "a123=" << a123 / CLHEP::degree << " °"
                  << std::endl;
      }
    if (a123 > M_PI) return false;
    double a230 = (-u23).angle (u30);
    if (rev)
      {
        a230 = 2 * M_PI - a230;
      }
    if (devel)
      {
        std::cerr << "DEVEL: geomtools::facet34::compute_internal_angles: "
                  << "a230=" << a230 / CLHEP::degree << " °"
                  << std::endl;
      }
    if (a230 > M_PI) return false;
    double a301 = (-u30).angle (u01);
    if (rev)
      {
        a301 = 2 * M_PI - a301;
      }
    if (devel)
      {
        std::cerr << "DEVEL: geomtools::facet34::compute_internal_angles: "
                  << "a301=" << a301 / CLHEP::degree << " °"
                  << std::endl;
      }
    if (a301 > M_PI) return false;

    double asum = a012 + a123 + a230 + a301;
    if (devel)
      {
        std::cerr << "DEVEL: geomtools::facet34::compute_internal_angles: "
                  << "asum=" << asum / CLHEP::degree << " °"
                  << std::endl;
      }
    */
    return;
  }

  void facet34::set_quadrangle (const facet_vertex & v0_,
                                const facet_vertex & v1_,
                                const facet_vertex & v2_,
                                const facet_vertex & v3_,
                                int iv0_,
                                int iv1_,
                                int iv2_,
                                int iv3_)
  {
    _number_of_vertices_ = 4;
    _vertices_[0] = &v0_;
    _vertices_[1] = &v1_;
    _vertices_[2] = &v2_;
    _vertices_[3] = &v3_;

    DT_THROW_IF ((! check_quadrangle (v0_.get_position (), v1_.get_position (),
                                      v2_.get_position (), v3_.get_position ())),
                 std::logic_error,
                 "Invalid set of vertices for a quadrangle !");

    _vertices_keys_[0] = iv0_;
    _vertices_keys_[1] = iv1_;
    _vertices_keys_[2] = iv2_;
    _vertices_keys_[3] = iv3_;
    compute_internals ();
   return;
  }

  facet34::facet34 (const facet_vertex & v0_,
                    const facet_vertex & v1_,
                    const facet_vertex & v2_,
                    const facet_vertex & v3_,
                    int iv0_,
                    int iv1_,
                    int iv2_,
                    int iv3_)
  {
    _set_defaults ();
    set_quadrangle (v0_,
                    v1_,
                    v2_,
                    v3_,
                    iv0_,
                    iv1_,
                    iv2_,
                    iv3_);
    return;
  }

  bool facet34::is_triangle () const
  {
    return _number_of_vertices_ == 3;
  }

  bool facet34::is_quadrangle () const
  {
    return _number_of_vertices_ == 4;
  }

  void facet34::compute_internals ()
  {
    compute_normal ();
    compute_surface ();
    compute_internal_angles ();
    return;
  }

  double facet34::get_internal_angle (int i_) const
  {
    DT_THROW_IF ((i_ < 0) || (i_ >= (int)_number_of_vertices_),
                 std::logic_error ,
                 "Invalid vertex index " << i_ << " !");
    return _internal_angles_[i_];
  }

  bool facet34::has_surface() const
  {
    return datatools::is_valid (_surface_tri_);
  }

  double facet34::get_surface () const
  {
    return _surface_tri_ + _surface_tri_bis_;
  }

  double facet34::get_surface_tri () const
  {
    return _surface_tri_;
  }

  double facet34::get_surface_tri_bis () const
  {
    return _surface_tri_bis_;
  }

  void facet34::compute_surface ()
  {
    datatools::invalidate (_surface_tri_);
    if (is_valid ())
      {
        if (is_triangle ())
          {
            const geomtools::vector_3d & v0 = _vertices_[0]->get_position ();
            const geomtools::vector_3d & v1 = _vertices_[1]->get_position ();
            const geomtools::vector_3d & v2 = _vertices_[2]->get_position ();
            const geomtools::vector_3d u01 = v1 - v0;
            const geomtools::vector_3d u02 = v2 - v0;
            _surface_tri_ = 0.5 * (u01.cross (u02)).mag ();
            _surface_tri_bis_ = 0.0;
           }
        if (is_quadrangle ())
          {
            const geomtools::vector_3d & v0 = _vertices_[0]->get_position ();
            const geomtools::vector_3d & v1 = _vertices_[1]->get_position ();
            const geomtools::vector_3d & v2 = _vertices_[2]->get_position ();
            const geomtools::vector_3d & v3 = _vertices_[3]->get_position ();
            const geomtools::vector_3d u01 = v1 - v0;
            const geomtools::vector_3d u03 = v3 - v0;
            const geomtools::vector_3d u21 = v1 - v2;
            const geomtools::vector_3d u23 = v3 - v2;
            _surface_tri_ = 0.5 * (u01.cross (u03)).mag ();
            _surface_tri_bis_ = 0.5 * (u21.cross (u23)).mag ();
          }
      }
    return;
  }

  void facet34::print (std::ostream & out_) const
  {
    out_ << "#vertices=" << get_number_of_vertices ();
    out_ << ";vertices={" << get_vertex_key (0) << ", "
         << get_vertex_key(1) << ", "
         << get_vertex_key(2);
    if (get_number_of_vertices () == 4)
      {
        out_ << ", " <<  get_vertex_key(3);
      }
    out_ << "}";
    out_ << ",angles={";
    for (size_t i=0; i<get_number_of_vertices (); i++)
      {
        if (i!=0) out_ << ',';
        out_ << get_internal_angle (i) / CLHEP::degree << " °";
      }
    out_ << '}';
    out_ << ";surface=" << get_surface () / CLHEP::mm2 << " mm2";
    out_ << ";category=" <<_category_;
    out_ << std::endl;
    return;
  }

  /* facet3 */
  /*
  size_t facet3::get_number_of_vertices () const
  {
    return 3;
  }

  const facet_vertex & facet3::get_vertex (int i_) const
  {
    if ((i_ < 0) || (i_ >= get_number_of_vertices ()))
      {
        ostringstream message;
        message << "geomtools::facet3::get_vertex: "
                << "Invalid vertex index " << i_ << " !";
        th row logic_error (message.str());
      }
    return *_vertices_[i_];
  }

  int facet3::get_vertex_key (int i_) const
  {
    if ((i_ < 0) || (i_ >= get_number_of_vertices ()))
      {
        ostringstream message;
        message << "geomtools::facet3::get_vertex_key: "
                << "Invalid vertex index " << i_ << " !";
        th row logic_error (message.str());
      }
    return _vertices_keys_[i_];
  }

  geomtools::vector_3d facet3::get_normal () const
  {
    geomtools::vector_3d n;
    const geomtools::vector_3d & v0 = _vertices_[0]->get_position ();
    const geomtools::vector_3d & v1 = _vertices_[1]->get_position ();
    const geomtools::vector_3d & v2 = _vertices_[2]->get_position ();
    geomtools::vector_3d u01 = v1 - v0;
    geomtools::vector_3d u12 = v2 - v1;
    n = u01.cross (u12);
    double m = n.mag ();
    n /= m;
    return n;
  }

  facet3::facet3 ()
  {
    _vertices_[0] = 0;
    _vertices_[1] = 0;
    _vertices_[2] = 0;
    _vertices_keys_[0] = -1;
    _vertices_keys_[1] = -1;
    _vertices_keys_[2] = -1;
    return;
  }

  facet3::facet3 (const facet_vertex & v0_,
                  const facet_vertex & v1_,
                  const facet_vertex & v2_,
                  int iv0_, int iv1_, int iv2_)
  {
    _vertices_[0] = &v0_;
    _vertices_[1] = &v1_;
    _vertices_[2] = &v2_;
    _vertices_keys_[0] = iv0_;
    _vertices_keys_[1] = iv1_;
    _vertices_keys_[2] = iv2_;
    return;
  }
  */

  /* facet4 */

  /*
  bool facet4::_is_valid_ () const
  {
    // missing checks...
    return true;
  }

  size_t facet4::get_number_of_vertices () const
  {
    return 4;
  }

  const facet_vertex & facet4::get_vertex (int i_) const
  {
    if ((i_ < 0) || (i_ >= get_number_of_vertices ()))
      {
        ostringstream message;
        message << "geomtools::facet4::get_vertex: "
                << "Invalid vertex index " << i_ << " !";
        th row logic_error (message.str());
      }
    return *_vertices_[i_];
  }

  int facet4::get_vertex_key (int i_) const
  {
    if ((i_ < 0) || (i_ >= get_number_of_vertices ()))
      {
        ostringstream message;
        message << "geomtools::facet4::get_vertex_key: "
                << "Invalid vertex index " << i_ << " !";
        th row logic_error (message.str());
      }
    return _vertices_keys_[i_];
  }

  geomtools::vector_3d facet4::get_normal () const
  {
    geomtools::vector_3d n;
    const geomtools::vector_3d & v0 = _vertices_[0]->get_position ();
    const geomtools::vector_3d & v1 = _vertices_[1]->get_position ();
    const geomtools::vector_3d & v2 = _vertices_[2]->get_position ();
    geomtools::vector_3d u01 = v1 - v0;
    geomtools::vector_3d u12 = v2 - v1;
    n = u01.cross (u12);
    double m = n.mag ();
    n /= m;
    return n;
  }

  facet4::facet4 ()
  {
    _vertices_[0] = 0;
    _vertices_[1] = 0;
    _vertices_[2] = 0;
    _vertices_[3] = 0;
    _vertices_keys_[0] = -1;
    _vertices_keys_[1] = -1;
    _vertices_keys_[2] = -1;
    _vertices_keys_[3] = -1;
    return;
  }

  facet4::facet4 (const facet_vertex & v0_,
                  const facet_vertex & v1_,
                  const facet_vertex & v2_,
                  const facet_vertex & v3_,
                  int iv0_, int iv1_, int iv2_, int iv3_)
  {
    _vertices_[0] = &v0_;
    _vertices_[1] = &v1_;
    _vertices_[2] = &v2_;
    _vertices_[3] = &v3_;
    _vertices_keys_[0] = iv0_;
    _vertices_keys_[1] = iv1_;
    _vertices_keys_[2] = iv2_;
    _vertices_keys_[3] = iv3_;
    if (! _is_valid_ ())
      {
        th row logic_error ("geomtools::facet4::facet4: Invalid set of vertices !");
      }
    return;
  }
  */

  /* facet_segment */

  bool facet_segment::is_valid () const
  {
    return vertex0_key >= 0 && vertex1_key >= 0 && facet0_key >= 0 && facet1_key;
  }

  bool facet_segment::is_shown () const
  {
    return shown;
  }

  void facet_segment::set_shown (bool s_)
  {
    shown = s_;
    return;
  }

  facet_segment::facet_segment ()
  {
    vertex0_key = vertex1_key = facet0_key = facet1_key = -1;
    shown = false;
    return;
  }

  void facet_segment::set_vertex_keys (int vk1_, int vk2_)
  {
    DT_THROW_IF (vk1_ == vk2_,
                 std::logic_error,
                 "Invalid vertex indexes " << vk1_ << "/" << vk2_ << " !");
    DT_THROW_IF (vk1_ < 0,
                 std::logic_error,
                 "Invalid vertex indexes " << vk1_ << " !");
    DT_THROW_IF (vk2_ < 0,
                 std::logic_error,
                 "Invalid vertex indexes " << vk2_ << " !");
    if (vk1_ < vk2_)
      {
        vertex0_key = vk1_;
        vertex1_key = vk2_;
      }
    else
      {
        vertex0_key = vk2_;
        vertex1_key = vk1_;
      }

    return;
  }

  void facet_segment::set_facet_keys (int fk1_, int fk2_)
  {
    DT_THROW_IF (fk1_ == fk2_,
                 std::logic_error,
                 "Invalid facet indexes " << fk1_ << "/" << fk2_ << " !");
    DT_THROW_IF (fk1_ < 0,
                 std::logic_error,
                 "Invalid facet indexes " << fk1_ << " !");
    DT_THROW_IF (fk2_ < 0,
                 std::logic_error,
                 "Invalid facet indexes " << fk2_ << " !");
    if (fk1_ < fk2_)
      {
        facet0_key = fk1_;
        facet1_key = fk2_;
      }
    else
      {
        facet0_key = fk2_;
        facet1_key = fk1_;
      }

    return;
  }

  void facet_segment::set (int vk1_, int vk2_, int fk1_, int fk2_)
  {
    set_vertex_keys (vk1_, vk2_);
    set_facet_keys (fk1_, fk2_);
    return;
  }

  bool operator==(const facet_segment & fs1_, const facet_segment & fs2_)
  {
    return (fs1_.vertex0_key == fs2_.vertex0_key)
      && (fs1_.vertex1_key == fs2_.vertex1_key);
  }

  bool operator<(const facet_segment & fs1_, const facet_segment & /*fs2_*/)
  {
    if (fs1_.vertex0_key < fs1_.vertex1_key) return true;
    return false;
  }

  void facet_segment::dump (std::ostream & out_) const
  {
    out_ << "geomtools::facet_segment: " << std::endl;
    out_ << "|-- Vertex 0 key = " << vertex0_key << std::endl;
    out_ << "|-- Vertex 1 key = " << vertex1_key << std::endl;
    out_ << "|-- Facet 0 key  = " << facet0_key << std::endl;
    out_ << "|-- Facet 1 key  = " << facet1_key << std::endl;
    out_ << "`-- The end " << std::endl;

    return;
  }

  /* tessellated_solid */

  const std::string & tessellated_solid::tessellated_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "tessellated";
    }
    return label;
  }

  bool tessellated_solid::is_consistent () const
  {
    return _consistent_;
  }

  bool tessellated_solid::_check_ ()
  {
    _consistent_ = true;
    for (vertices_col_type::const_iterator i = _vertices_.begin ();
         i != _vertices_.end ();
         i++)
      {
        if (i->second.ref_facets.size () == 0)
          {
            DT_LOG_WARNING (datatools::logger::PRIO_WARNING,
                            "Vertex " << i->first << " is not used at all !");
            _consistent_ = false;
          }
        else if (i->second.ref_facets.size () < 3)
          {
            DT_LOG_WARNING (datatools::logger::PRIO_WARNING,
                            "Vertex " << i->first << " is not properly referenced (#ref="
                            << i->second.ref_facets.size () << ") !");
            _consistent_ = false;
            return _consistent_;
          }
      }
    // missing check:

    return _consistent_;
  }

  void tessellated_solid::compute_facet_segments ()
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE (local_priority, "Entering...");
    DT_LOG_TRACE (local_priority, "Number of facets = " << _facets_.size ());
    if (local_priority >= datatools::logger::PRIO_TRACE) dump (std::cerr);

    _facet_segments_.clear ();
    int facet_segment_counter = 0;
    for (facets_col_type::const_iterator facet_iter = _facets_.begin ();
         facet_iter != _facets_.end ();
         facet_iter++)
      {
        const int current_facet_key = facet_iter->first;
        const facet34 & the_facet = facet_iter->second;
        if (local_priority >= datatools::logger::PRIO_TRACE)
          {
            std::ostringstream message;
            message << "Facet #" << current_facet_key << " with "
                    << the_facet.get_number_of_vertices () << " vertices : ";
            for (unsigned int ivtx = 0; ivtx < the_facet.get_number_of_vertices (); ivtx++)
              {
                message << the_facet.get_vertex_key (ivtx) << " ";
              }
            DT_LOG_TRACE (local_priority, message.str ());
          }

        for (unsigned int ivtx = 0; ivtx < the_facet.get_number_of_vertices (); ivtx++)
          {
            const unsigned int iref = ivtx;
            const unsigned int irefnext = (ivtx + 1 ) % the_facet.get_number_of_vertices ();
            DT_LOG_TRACE (local_priority, "Indexes=[" << iref << '-' << irefnext << "]");
            const facet_vertex & v1 = the_facet.get_vertex (iref);
            const facet_vertex & v2 = the_facet.get_vertex (irefnext);
            const int vk1 = the_facet.get_vertex_key (ivtx);
            const int vk2 = the_facet.get_vertex_key ((ivtx + 1) % the_facet.get_number_of_vertices ());
            DT_LOG_TRACE (local_priority, "Segment from vtx " << vk1 << "=(" << v1 << ") to vtx " << vk2 << "=(" << v2 << ")");

            // Facet segment candidate :
            facet_segment fseg;
            fseg.set_vertex_keys (vk1, vk2);

            // Check if it is already registered :
            bool already_exists = false;
            for (facet_segments_col_type::const_iterator fsi = _facet_segments_.begin ();
                 fsi != _facet_segments_.end ();
                 fsi++)
              {
                if (local_priority >= datatools::logger::PRIO_TRACE)
                  {
                    fsi->second.dump (std::cerr);
                  }
                if (fseg == fsi->second)
                  {
                    already_exists = true;
                    DT_LOG_TRACE (local_priority, "Segment already exists as #" << fsi->first << " !");
                    break;
                  }
              }
            if (already_exists) {
              continue;
            }

            const int vf1 = current_facet_key;
            // Search the other facet that shares this [v1,v2] segment :
            int vf2 = -1;
            DT_LOG_TRACE (local_priority, "Search the other facet (!=" << vf1  << ") that shares segment [vk1=" << vk1 << ";vk2=" << vk2 << "]");
            if (local_priority >= datatools::logger::PRIO_TRACE)
              {
                std::ostringstream message;
                message << "Now loop on " << v1.ref_facets.size () << " facets"
                        << " at vertex #" << vk1 << ": ";
                for (std::map<int, int>::const_iterator ifv1 = v1.ref_facets.begin ();
                     ifv1 != v1.ref_facets.end ();
                     ifv1++) message << ifv1->first << " ";
                DT_LOG_TRACE (local_priority, message.str ());
              }
            for (std::map<int, int>::const_iterator ifv1 = v1.ref_facets.begin ();
                 ifv1 != v1.ref_facets.end ();
                 ifv1++)
              {
                const int chck_fk1 = ifv1->first;
                DT_LOG_TRACE (local_priority, "Loop on V1's facet #" << chck_fk1);
                if (chck_fk1 == vf1)
                  {
                    DT_LOG_TRACE (local_priority, "Skipping facet #" << chck_fk1);
                    continue;
                  }
                if (local_priority >= datatools::logger::PRIO_TRACE)
                  {
                    std::ostringstream message;
                    message << "Now loop on " << v2.ref_facets.size () << " facets"
                            << " at vertex #" << vk2 << ": ";
                    for (std::map<int, int>::const_iterator ifv2 = v2.ref_facets.begin ();
                         ifv2 != v2.ref_facets.end ();
                         ifv2++) message << ifv2->first << " ";
                    DT_LOG_TRACE (local_priority, message.str ());
                  }
                for (std::map<int, int>::const_iterator ifv2 = v2.ref_facets.begin ();
                     ifv2 != v2.ref_facets.end ();
                     ifv2++)
                  {
                    const int chck_fk2 = ifv2->first;
                    DT_LOG_TRACE (local_priority, "Loop on V2's facet #" << chck_fk2);
                    if (chck_fk2 == vf1)
                      {
                        DT_LOG_TRACE (local_priority, "Skipping  facet #" << chck_fk2);
                        continue;
                      }
                    if (chck_fk1 == chck_fk2)
                      {
                        vf2 = chck_fk2;
                        break;
                      }
                  }
                if (vf2 >= 0)
                  {
                    DT_LOG_TRACE (local_priority, "Break for found  facet #" << vf2);;
                    break;
                  }
              }
            DT_THROW_IF (vf2 < 0, std::logic_error, "Cannot compute facet segments !");
            DT_LOG_TRACE (local_priority, "Segment belongs to facets #" << vf1 << " and " << vf2 << " !");

            fseg.set_facet_keys (vf1,vf2);
            _facet_segments_[facet_segment_counter] = fseg;
            DT_LOG_TRACE (local_priority, "Segment is registered as #" << facet_segment_counter << " !");
            facet_segment_counter++;
          }

      }

    // Compute the 'show' flag :
    for (facet_segments_col_type::iterator fsi = _facet_segments_.begin ();
         fsi != _facet_segments_.end ();
         fsi++) {
        facet_segment & the_fsgmt = fsi->second;

        const facet34 & fct0 = facets ().find (the_fsgmt.facet0_key)->second;
        const facet34 & fct1 = facets ().find (the_fsgmt.facet1_key)->second;

        if (fct0.get_normal ().isNear (fct1.get_normal (), 1.0e-4)) {
            the_fsgmt.set_shown (false);
          } else {
            the_fsgmt.set_shown (true);
          }
      }

    DT_LOG_TRACE (local_priority, "Compute " << _facet_segments_.size () << " facet segments !");
    DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  void tessellated_solid::_at_lock()
  {
    this->i_shape_3d::_at_lock();
    compute_facet_segments();
    return;
  }

  void tessellated_solid::_at_unlock()
  {
    _facet_segments_.clear();
    this->i_shape_3d::_at_unlock();
    return;
  }

  void tessellated_solid::_build_bounding_data()
  {
    if (!_xrange_.is_valid() || !_yrange_.is_valid() || !_zrange_.is_valid()) {
      compute_bounding_box();
      _grab_bounding_data().make_box(_xrange_.get_min(), _xrange_.get_max(),
                                     _yrange_.get_min(), _yrange_.get_max(),
                                     _zrange_.get_min(), _zrange_.get_max());
    }
    return;
  }

  bool tessellated_solid::validate_index (int i_)
  {
    return (i_ > INVALID_VERTEX) && (i_ < MAX_VERTEX);
  }

  tessellated_solid::tessellated_solid ()
  {
    _consistent_ = false;
    return;
  }

  tessellated_solid::~tessellated_solid ()
  {
    _facet_segments_.clear ();
    // for (facets_col_type::iterator i = _facets_.begin ();
    //      i != _facets_.end ();
    //      i++)
    //   {
    //     delete i->second;
    //   }
    _facets_.clear ();
    _vertices_.clear ();
    return;
  }

  const tessellated_solid::vertices_col_type & tessellated_solid::vertices () const
  {
    return _vertices_;
  }

  const tessellated_solid::facets_col_type & tessellated_solid::facets () const
  {
    return _facets_;
  }

  tessellated_solid::vertices_col_type & tessellated_solid::grab_vertices ()
  {
    return _vertices_;
  }

  tessellated_solid::facets_col_type & tessellated_solid::grab_facets ()
  {
    return _facets_;
  }

  const tessellated_solid::facet_segments_col_type & tessellated_solid::facet_segments () const
  {
    return _facet_segments_;
  }

  tessellated_solid::facet_segments_col_type & tessellated_solid::grab_facet_segments ()
  {
    return _facet_segments_;
  }

  int tessellated_solid::add_vertex (unsigned int vtx_key_, const facet_vertex & vtx_)
  {
    DT_THROW_IF (is_locked (), std::logic_error, "Object is locked !");
    _vertices_[vtx_key_] = vtx_;
    _xrange_.add (vtx_.position.x ());
    _yrange_.add (vtx_.position.y ());
    _zrange_.add (vtx_.position.z ());
    return vtx_key_;
  }

  const mygsl::min_max & tessellated_solid::get_bounding_box_x () const
  {
    return _xrange_;
  }

  const mygsl::min_max & tessellated_solid::get_bounding_box_y () const
  {
    return _yrange_;
  }

  const mygsl::min_max & tessellated_solid::get_bounding_box_z () const
  {
    return _zrange_;
  }

  // // virtual
  // void tessellated_solid::compute_bounding_box(box & bb_)
  // {
  //   this->i_object_3d::compute_bounding_box(bb_);
  //   if (_xrange_.is_valid() && _yrange_.is_valid() && _zrange_.is_valid()) {
  //     double x = std::abs(_xrange_.get_min());
  //     if (std::abs(_xrange_.get_max()) > x) {
  //       x = std::abs(_xrange_.get_max());
  //     }
  //     double y = std::abs(_yrange_.get_min());
  //     if (std::abs(_yrange_.get_max()) > y) {
  //       y = std::abs(_yrange_.get_max());
  //     }
  //     double z = std::abs(_zrange_.get_min());
  //     if (std::abs(_zrange_.get_max()) > z) {
  //       z = std::abs(_zrange_.get_max());
  //     }
  //     bb_.set_x(2 * x);
  //     bb_.set_y(2 * y);
  //     bb_.set_z(2 * z);
  //   }
  //   return;
  // }

  void tessellated_solid::compute_bounding_box()
  {
    _xrange_.reset();
    _yrange_.reset();
    _zrange_.reset();
    for (vertices_col_type::const_iterator i = _vertices_.begin();
         i != _vertices_.end();
         i++) {
        const facet_vertex & vtx = i->second;
        _xrange_.add(vtx.position.x());
        _yrange_.add(vtx.position.y());
        _zrange_.add(vtx.position.z());
      }
    return;
  }

  int tessellated_solid::add_vertex (unsigned int vtx_key_,
                                     double x_,
                                     double y_,
                                     double z_)
  {
    facet_vertex vtx;
    vtx.position.set (x_, y_, z_);
    return add_vertex (vtx_key_, vtx);
  }

  void tessellated_solid::add_facet3 (unsigned int facet_key_,
                                      int ivtx0_, int ivtx1_, int ivtx2_)
  {
    add_facet (facet_key_,  ivtx0_, ivtx1_, ivtx2_, INVALID_VERTEX);
    return;
  }

  void tessellated_solid::add_facet4 (unsigned int facet_key_,
                                      int ivtx0_, int ivtx1_, int ivtx2_, int ivtx3_)
  {
    add_facet (facet_key_,  ivtx0_, ivtx1_, ivtx2_, ivtx3_);
    return;
  }

  void tessellated_solid::add_facet (unsigned int facet_key_,
                                     int ivtx0_,
                                     int ivtx1_,
                                     int ivtx2_,
                                     int ivtx3_)
  {
    DT_THROW_IF (is_locked (), std::logic_error, "Object is locked !");
    size_t dim = 3;
    if (validate_index (ivtx3_)) {
      dim = 4;
    }
    int ivtx[4];
    ivtx[0] = ivtx0_;
    ivtx[1] = ivtx1_;
    ivtx[2] = ivtx2_;
    ivtx[3] = ivtx3_;
    vertices_col_type::iterator vtx_it[4];
    for (size_t i = 0; i < dim; i++) {
      vtx_it[i] = _vertices_.find (ivtx[i]);
      DT_THROW_IF (vtx_it[i] == _vertices_.end (),
                   std::logic_error,
                   "Invalid vertex index " << ivtx[i] << " !");
    }
    //i_facet * fct = 0;
    facet34 fct;
    if (dim == 3) {
      vtx_it[0]->second.add_ref_facet (facet_key_, 0);
      vtx_it[1]->second.add_ref_facet (facet_key_, 1);
      vtx_it[2]->second.add_ref_facet (facet_key_, 2);
      fct = facet34 ((vtx_it[0]->second),
                     (vtx_it[1]->second),
                     (vtx_it[2]->second),
                     ivtx[0],ivtx[1],ivtx[2]);
    } else {
      vtx_it[0]->second.add_ref_facet (facet_key_, 0);
      vtx_it[1]->second.add_ref_facet (facet_key_, 1);
      vtx_it[2]->second.add_ref_facet (facet_key_, 2);
      vtx_it[3]->second.add_ref_facet (facet_key_, 3);
      fct = facet34 ((vtx_it[0]->second),
                     (vtx_it[1]->second),
                     (vtx_it[2]->second),
                     (vtx_it[3]->second),
                     ivtx[0],ivtx[1],ivtx[2],ivtx[3]);
    }
    _facets_[facet_key_] = fct;
    return;
  }

  void tessellated_solid::remove_facet (unsigned int facet_key_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE (local_priority, "Before: #facets=" << _facets_.size ());

    // Check the existence of the facet :
    facets_col_type::iterator facet_found = _facets_.find (facet_key_);
    DT_THROW_IF (facet_found ==_facets_.end (),
                 std::logic_error,
                 "Facet #" << facet_key_ << "  does not exist !");

    // Get a pointer to the facet to be removed :
    //i_facet * the_facet_ptr = facet_found->second;
    facet34 the_facet = facet_found->second;

    // Traverse the collection of vertices in the facet :
    for (size_t i = 0; i < the_facet.get_number_of_vertices (); i++) {
        //int vtxkey = the_facet_ptr->get_vertex_key (i);
        int vtxkey = the_facet.get_vertex_key (i);

        // Check if the vertex exists in the dictionary of vertices :
        vertices_col_type::iterator vtx_found = _vertices_.find (vtxkey);
        DT_THROW_IF (vtx_found == _vertices_.end (),
                     std::logic_error,
                     "Vertex #" << vtxkey << "  does not exist !");

        // Get a reference to the vertex :
        facet_vertex & the_vertex = vtx_found->second;

        // Search the reference to the facet in the current vertex :
        std::map<int, int>::iterator found2 = the_vertex.ref_facets.find (facet_key_);
        DT_THROW_IF (found2 == the_vertex.ref_facets.end (),
                     std::logic_error,
                     "Vertex #" << vtxkey << " has is not used by facet #" << facet_key_ << " !");
        // Remove the facet reference in this vertex:
        the_vertex.ref_facets.erase (found2);
      }

    DT_LOG_TRACE (local_priority, "After: #facets=" << _facets_.size ());

    // Remove the facet from the dictionnary of facets :
    //facet_found->second = 0;
    _facets_.erase (facet_found);

    // Delete the facet :
    //delete the_facet_ptr;
    return;
  }

  void tessellated_solid::dump (std::ostream & out_) const
  {
    std::string tag   = "|-- ";
    std::string stag  = "|   ";
    std::string ltag  = "`-- ";
    std::string sltag = "    ";
    out_ << "geomtools::tessellated_solid" << std::endl;
    out_ << tag << "Locked: " << is_locked () << std::endl;
    out_ << tag << "Vertices: " << _vertices_.size () << std::endl;
    for (vertices_col_type::const_iterator i = _vertices_.begin ();
         i != _vertices_.end ();
         i++) {
        std::string tag2 = tag;
        {
          vertices_col_type::const_iterator j = i;
          if (++j == _vertices_.end ()) tag2 = ltag;
        }
        out_ << stag << tag2 << "Vertex[" << i->first << "] : ";
        i->second.print (out_);
      }
    out_ << tag << "Bounding box: " << std::endl;
    out_ << stag << tag << "X: ["  << _xrange_.get_min () << ';' << _xrange_.get_max () << ']' << std::endl;
    out_ << stag << tag << "Y: ["  << _yrange_.get_min () << ';' << _yrange_.get_max () << ']' << std::endl;
    out_ << stag << ltag << "Z: [" << _zrange_.get_min () << ';' << _zrange_.get_max () << ']' << std::endl;

    out_ << tag << "Facet segments: " << _facet_segments_.size () << std::endl;

    out_ << ltag << "Facets: " << _facets_.size () << std::endl;
    for (facets_col_type::const_iterator i = _facets_.begin ();
         i != _facets_.end ();
         i++) {
        std::string tag2 = tag;
        {
          facets_col_type::const_iterator j = i;
          if (++j == _facets_.end ()) tag2 = ltag;
        }
        out_ << sltag << tag2 << "Facet[" << i->first << "] : ";
        i->second.print (out_);
      }
    return;
  }

  void tessellated_solid::print_xyz (std::ostream & out_) const
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE (local_priority, "# vertices = " << _vertices_.size());
    DT_LOG_TRACE (local_priority, "# facets   = " << _facets_.size());

    // facets:
    int count = 0;
    for (facets_col_type::const_iterator i = _facets_.begin ();
         i != _facets_.end ();
         i++) {
      //const i_facet * fct = i->second;
      const facet34 & fct = i->second;
      out_ << "# facet " << count
           << " with " <<  fct.get_number_of_vertices ()
           << " vertices" << std::endl;
      const size_t nvtx = fct.get_number_of_vertices ();
      DT_LOG_TRACE (local_priority, "# nvtx = " << nvtx);

      // vertices:
      const geomtools::vector_3d u1 = fct.get_vertex (1).position - fct.get_vertex (0).position;
      const geomtools::vector_3d u2 = fct.get_vertex (2).position - fct.get_vertex (1).position;
      const geomtools::vector_3d u3 = u1.cross (u2);
      const int color = 5 + (int) (5 * u3.cosTheta ());
      fct.get_vertex (0).print_xyz (out_, color);
      fct.get_vertex (1).print_xyz (out_, color);
      out_ << std::endl;
      if (nvtx == 3) {
        fct.get_vertex (2).print_xyz (out_, color);
        fct.get_vertex (2).print_xyz (out_, color);
        out_ << std::endl;
      } else {
        fct.get_vertex (3).print_xyz (out_, color);
        fct.get_vertex (2).print_xyz (out_, color);
        out_ << std::endl;
      }
      /*
        const double dx = fct.get_vertex (1).position.x () - fct.get_vertex (0).position.x ();
        const double dy = fct.get_vertex (1).position.y () - fct.get_vertex (0).position.y ();
        const double dz = fct.get_vertex (1).position.z () - fct.get_vertex (0).position.z ();
        if (nvtx == 3)
        {
        facet_vertex last;
        last.position.setX (fct.get_vertex (2).position.x () - 1e-6 * dx / m);
        last.position.setY (fct.get_vertex (2).position.y () - 1e-6 * dy / m);
        last.position.setY (fct.get_vertex (2).position.z () - 1e-6 * dz / m);
        last.print_xyz (out_, color);
        }
        else
        {
        fct.get_vertex (fct.get_number_of_vertices () - 1)
        .print_xyz (out_, color);
        }
        fct.get_vertex (2).print_xyz (out_, color);
        out_ << std::endl;
      */
      out_ << std::endl;
      count++;
    }
    return;
  }

  bool tessellated_solid::is_inside (const vector_3d &,
                                     double /*skin_*/) const
  {
    DT_THROW_IF (true,std::runtime_error, "Not implemented !");
    return false;
  }

  // if 'skin' < 0 no skin is taken into account:
  bool tessellated_solid::is_on_surface (const vector_3d & ,
                                         int /*mask_*/,
                                         double /*skin_*/) const
  {
    DT_THROW_IF (true, std::runtime_error, "Not implemented !");
    return false;
  }

  vector_3d tessellated_solid::get_normal_on_surface (const vector_3d & /*position_*/) const
  {
    DT_THROW_IF (true, std::runtime_error, "Not implemented !");
    vector_3d v;
    return v;
  }

  bool tessellated_solid::find_intercept (const vector_3d & /*from_*/,
                                          const vector_3d & /*direction_*/,
                                          intercept_t & /*intercept_*/,
                                          double /*skin_*/) const
  {
    DT_THROW_IF (true,std::runtime_error, "Not implemented !");
    return false;
  }

  std::string tessellated_solid::get_shape_name () const
  {
    return tessellated_solid::tessellated_label();
  }


  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(tessellated_solid,
                                             "geomtools::tessellated_solid");


  void tessellated_solid::initialize_from_stl(const std::string & filename_,
                                              double length_unit_)
  {
    std::ifstream fin(filename_.c_str(), std::ios::binary);
    std::string solid_token;
    {
      fin.seekg(0, std::ios::end);
      std::streampos length = fin.tellg();
      fin.seekg(0, std::ios::beg);
      // std::cerr << "length : " << length << '\n';
      solid_token.reserve(length);
      fin.seekg(0, std::ios::beg);
      solid_token.assign((std::istreambuf_iterator<char>(fin)),
                       (std::istreambuf_iterator<char>()));
    }

    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef geomtools::stl::solid_parser<iterator_type> solid_parser_type;

    iterator_type solid_iter = solid_token.begin();
    iterator_type solid_end  = solid_token.end();
    solid_parser_type     sld_grammar;
    geomtools::stl::solid sld;
    bool result = boost::spirit::qi::phrase_parse(solid_iter,
                                                  solid_end,
                                                  sld_grammar,
                                                  boost::spirit::ascii::space,
                                                  sld);
    if (result && solid_iter == solid_end) {
      geomtools::stl::stl_to_geomtools_converter stl2gt;
      stl2gt.set_length_unit(length_unit_);
      stl2gt.set_fix_attempt(true);
      stl2gt.set_check_normal(true);
      stl2gt.convert(sld, *this);
      this->lock();
    } else {
      DT_THROW(std::logic_error, "STL parsing from file '" << filename_ << "' failed!");
    }
    return;
  }

  void tessellated_solid::initialize(const datatools::properties & config_,
                                     const handle_dict_type * objects_)
  {
    reset();
    this->i_shape_3d::initialize(config_, objects_);

    double default_length_unit = 1.0 * CLHEP::mm;
    // std::cerr << "DEVEL: default_length_unit [0] = " << default_length_unit << std::endl;
    if (config_.has_key ("length_unit")) {
      const std::string length_unit_str = config_.fetch_string("length_unit");
      default_length_unit = datatools::units::get_length_unit_from(length_unit_str);
    }
    // std::cerr << "DEVEL: default_length_unit [1] = " << default_length_unit << std::endl;

    if (config_.has_key ("length_unit_value")) {
      double length_unit_value = config_.fetch_real("length_unit_value");
      if (! config_.has_explicit_unit("length_unit_value")) {
        length_unit_value *= default_length_unit;
      }
      default_length_unit = length_unit_value;
    }
    // std::cerr << "DEVEL: default_length_unit [2] = " << default_length_unit << std::endl;

    if (config_.has_key("stl_file")) {
      std::string stl_filename = config_.fetch_string("stl_file");
      datatools::fetch_path_with_env(stl_filename);
      initialize_from_stl(stl_filename, default_length_unit);
    }

    lock();
    return;
  }

  void tessellated_solid::reset ()
  {
    unlock();

    _vertices_.clear();
    _facets_.clear();
    _xrange_.reset();
    _yrange_.reset();
    _zrange_.reset();
    _facet_segments_.clear();
    _consistent_ = false;

    this->i_shape_3d::reset();
    return;
  }

} // end of namespace geomtools
