// -*- mode: c++; -*- 
/* utils.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2010-01-01
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

/*! \mainpage The geomtools package documentation
 *
 * \section intro_section Introduction
 *
 * \subsection pres_subsection Presentation
 *  The geomtools package provides some general purpose utility classes
 *  for geometry. It is based on the CLHEP 2D and 3D vectors and rotation 
 *  matrices well. Some basic 2D and 3D shapes are implemented with useful
 *  features. It is possible to build a geometry model in the way the GEANT4
 *  library does with GDLM exporting capability. 
 *
 *  This package depends on:
 *  - the datatools library
 *    https://nemo.lpc-caen.in2p3.fr/svn/datatools/  
 *  - the CLHEP C++ library (version 2.0 and above)
 *
 * \section contents_section Contents
 *  event model.
 *
 * \section build_section Installation instructions
 *
 *  1- Configure:
 *
 *     shell> ./pkgtools.d/pkgtool configure
 *
 *  2- Build:
 * 
 *     shell> ./pkgtools.d/pkgtool build
 *
 *  3- Build binaries:
 *
 *     shell> ./pkgtools.d/pkgtool build bin
 *
 *  4- Build test binaries:
 *
 *     shell> ./pkgtools.d/pkgtool build bin_test
 *
 *  5- Build documentation:
 *
 *     shell> ./pkgtools.d/pkgtool doc
 *
 *  6- Installation:
 *
 *     shell> ./pkgtools.d/pkgtool install
 *
 * \section geninfo_section General informations
 *
 *  Wiki repository:
 *   - https://nemo.lpc-caen.in2p3.fr/wiki/geomtools/
 *
 *  SVN repository:
 *   - Base repository: https://nemo.lpc-caen.in2p3.fr/svn/geomtools/
 *   - Development version: https://nemo.lpc-caen.in2p3.fr/svn/geomtools/trunk
 *   - Releases: https://nemo.lpc-caen.in2p3.fr/svn/geomtools/tags/geomtools-X.Y
 *
 *  Author:
 *   - Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 *  Mainteners:
 *   - Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef __geomtools__utils_h
#define __geomtools__utils_h 1

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <cmath>
#include <list>

#include <geomtools/clhep.h>
#include <datatools/serialization/serialization.h>
#include <boost/serialization/split_free.hpp>

namespace geomtools {

  using namespace std;

  typedef list<vector_2d> basic_polyline_2d;

  typedef list<vector_3d> basic_polyline_3d;

  void print_xy (ostream & out_, 
		 const vector_2d & p_,
		 bool endl_ = true);

  string to_xy (const vector_2d & p_);

  string vector_2d_to_xy (const vector_2d & p_);

  void print_xy_stdout (const vector_2d & p_);

  void print_xy_stderr (const vector_2d & p_);

  /******/

  void print_xyz (ostream & out_, 
		  const vector_3d & p_,
		  bool endl_ = true);

  void print (ostream & out_, const vector_3d & p_);

  string to_xyz (const vector_3d & p_);

  string vector_3d_to_xyz (const vector_3d & p_);

  void print_xyz_stdout (const vector_3d & p_);

  void print_xyz_stderr (const vector_3d & p_);

  /******/

  void print_xy (ostream & out_, 
		 const basic_polyline_2d & p_,
		 bool endl_ = true);

  string to_xy (const basic_polyline_2d & p_);

  string basic_polyline_2d_to_xy (const basic_polyline_2d & p_);

  void print_xy_stdout (const basic_polyline_2d & p_);

  void print_xy_stderr (const basic_polyline_2d & p_);

  /******/

  void print_xyz (ostream & out_, 
		  const basic_polyline_3d & p_,
		  bool endl_ = true);

  string to_xyz (const basic_polyline_3d & p_);

  string basic_polyline_3d_to_xyz (const basic_polyline_3d & p_);

  void print_xyz_stdout (const basic_polyline_3d & p_);

  void print_xyz_stderr (const basic_polyline_3d & p_);

  /******/

  //! Orientation constants
  enum orientation_type
    {
      VERTICAL   = 0,
      HORIZONTAL = 1
    };

  //! Direction constants
  enum direction_type
    {
      BACK   = 0, // -x
      FRONT  = 1, // +x
      LEFT   = 2, // -y
      RIGHT  = 3, // +y
      BOTTOM = 4, // -z
      TOP    = 5  // +z
    };

  enum face_3d
    {
      FACE_NONE_BIT = 0x0,
      FACE_ALL_BITS = 0xFFFFFFFF
    };
  
  
  //! Some constants
  struct constants
  {
    static const int NO_INTERCEPT = -1;
    static const double DEFAULT_TOLERANCE;
  };

  class filled_utils
  {
  public:

    enum filled_t
      {
	FILLED_NONE         = 0,
	FILLED_BY_ENVELOPE  = 1,
	FILLED_BY_EXTRUSION = 2
      };

    static const string FILLED_NONE_LABEL;
    static const string FILLED_BY_ENVELOPE_LABEL;
    static const string FILLED_BY_EXTRUSION_LABEL;

  };
  
  /* Initialize a rotation matrix for 
   * "World coordinates system->Local coordinates system":
   */

  //! Create a rotation using ZYZ Euler angles
  void create_zyz (rotation_3d & rot_,
		   double phi_,
		   double theta_,
		   double delta_);

  //! Create a rotation (default is using ZYZ Euler angles)
  void create (rotation_3d & rot_,
	       double phi_,
	       double theta_,
	       double delta_);

  //! Create a rotation using ZXZ Euler angles
  void create_zxz (rotation_3d & rot_,
		   double phi_,
		   double theta_,
		   double psi_);

  //! Create a rotation using XYZ Euler angles
  void create_xyz (rotation_3d & rot_,
		   double phi_,
		   double theta_,
		   double psi_);
   
  //! Create a rotation (default is using ZYZ Euler angles)
  void create_rotation_3d (rotation_3d & rot_,
			   double phi_,
			   double theta_,
			   double delta_);
  
  //! Create a rotation (default is using ZYZ Euler angles)
  void create_rotation_from_zyz_euler_angles (rotation_3d & rot_,
					      double phi_,
					      double theta_,
					      double delta_);

  //! Create a rotation (default is using ZYZ Euler angles)
  void create_rotation (rotation_3d & rot_,
			double phi_,
			double theta_,
			double delta_);

  //! Check if a rotation is the identity
  bool is_identity (const rotation_3d & rot_);

  //! Extract the XYZ Euler angles from a rotation
  void extract_xyz_euler_angle_from_rotation (const rotation_3d & rot_,
					      double & a_,
					      double & b_,
					      double & c_);


  //! Extract the ZYZ Euler angles from a rotation
  void extract_zyz_euler_angle_from_rotation (const rotation_3d & rot_,
					      double & a_,
					      double & b_,
					      double & c_);

  /********************/

  //! Special rotation angle values
  enum special_rotation_angle_t
    {
      ROTATION_ANGLE_INVALID = -1,
      ROTATION_ANGLE_0   = 0,
      ROTATION_ANGLE_90  = 1,
      ROTATION_ANGLE_180 = 2,
      ROTATION_ANGLE_270 = 3
    };

  bool check_special_rotation_angle (int);

  double get_special_rotation_angle (int);

  int get_special_rotation_angle_from_label (const string & );

  string get_special_rotation_angle_label (int);

  /********************/
  enum axis_t
    {
      AXIS_INVALID = -1,
      AXIS_X = 0,
      AXIS_Y = 1,
      AXIS_Z = 2
    };

  enum rotation_axis_t
    {
      ROTATION_AXIS_INVALID = -1,
      ROTATION_AXIS_X = AXIS_X,
      ROTATION_AXIS_Y = AXIS_Y,
      ROTATION_AXIS_Z = AXIS_Z
    };

  bool check_rotation_axis (int);

  int get_rotation_axis_from_label (const string & );

  string get_rotation_label (int);

  /********************/

  void create_rotation_from_axis (rotation_3d & rot_,
				  int axis_,
				  double angle_);

  void create_rotation (rotation_3d & rot_,
			int axis_,
			double angle_);

  void create_rotation (rotation_3d & rot_,
			int axis_,
			int special_angle_);
  
  void create_rotation_from (rotation_3d & rot_,
			     const string &);
  
  void reset (rotation_3d & rot_);
  
  void reset_rotation_3d (rotation_3d & rot_);
  
  void tree_dump (const rotation_3d & rot_,
		  ostream & out_, 
		  const string & title_ = "", 
		  const string & indent_ = "");

  void invalidate (rotation_3d & rot_);
  
  void invalidate_rotation_3d (rotation_3d & rot_);

  bool is_valid (const rotation_3d & rot_);
  
  bool is_valid_rotation_3d (const rotation_3d & rot_);

  /******/
  void create (vector_3d &,
	       double x_,
	       double y_,
	       double z_);

  void create_xyz (vector_3d &,
		   double x_,
		   double y_,
		   double z_);

  void create_polar (vector_3d &,
		     double r_,
		     double theta_,
		     double z_);

  void create_spherical (vector_3d &,
			 double r_,
			 double phi_,
			 double theta_);

  void invalidate (vector_3d & vec_);
  
  void invalidate_vector_3d (vector_3d & vec_);

  bool is_valid (const vector_3d & vec_);
  
  bool is_valid_vector_3d (const vector_3d & vec_);

  bool are_near (const vector_3d & vec1_, 
		 const vector_3d & vec2_,
		 double tolerance_ = constants::DEFAULT_TOLERANCE);

  bool are_near_vector_3d (const vector_3d & vec1_, 
			   const vector_3d & vec2_,
			   double tolerance_);

  /*****/

  void invalidate (vector_2d & vec_);
  
  void invalidate_vector_2d (vector_2d & vec_);

  bool is_valid (const vector_2d & vec_);

  bool is_valid_vector_2d (const vector_2d & vec_);


  void vector_2d_to_vector_3d (const vector_2d & v2d_, vector_3d & v3d_);

  void vector_3d_to_vector_2d (const vector_3d & v3d_, vector_2d & v2d_);

  void make_phi_theta (vector_3d & vec_, double phi_, double theta_);

  /*****/

  /* the 'ran_func' (functor) must have the following operator
   * defined:
   *   double operator() (double);
   * it returns a uniform deviated random number in the [0,1) range
   * like 'drand48' does.
   */
  template <class ran_func>
  void randomize_direction (ran_func & ran_, vector_3d & ran_dir_)
  {
    double phi = 2. * M_PI * ran_ ();
    double cos_theta = -1 + 2 * ran_ ();
    double sin_theta = sqrt (1. - cos_theta * cos_theta);
    double x = sin_theta * cos (phi);
    double y = sin_theta * sin (phi);
    double z = cos_theta;
    ran_dir_.set (x, y, z);
  }


  template <class ran_func>
  vector_3d randomize_direction (ran_func & ran_)
  {
    vector_3d dir;
    randomize_direction (ran_, dir);
    return dir;
  }


  /** The 'ran_func' (functor) must have the following operator
   * defined:
   *
   *   double operator() (double);
   *
   * This operator must returns a uniform deviated pseudo-random number 
   * in the [0,1) range like 'drand48' does.
   */
  template <class ran_func>
  void randomize_orthogonal_direction (ran_func & ran_, 
				       const vector_3d & dir_, 
				       vector_3d & ran_dir_)
  {
    double theta = 2. * M_PI * ran_ ();
    double dx = cos (theta);
    double dy = sin (theta);
    double dz = 0.0;
    double dir_theta = dir_.theta ();
    double dir_phi = dir_.phi ();
    clog << "theta (dir) = " 
	      << 180.* dir_theta / M_PI << "°" << endl;
    clog << "phi (dir) = " 
	      << 180.* dir_phi / M_PI << "°"  << endl;
    rotation_3d dir_rot;
    create_rotation_3d (dir_rot, dir_phi, dir_theta, 0.0);
    rotation_3d dir_inverse_rot;
    dir_inverse_rot = dir_rot.inverse ();
    vector_3d v (dx, dy, dz);
    ran_dir_ = v.transform (dir_inverse_rot); // XXX
    return;
  }
    

  template <class ran_func>
  vector_3d randomize_orthogonal_direction (ran_func & ran_, 
					    const vector_3d & ref_dir_)
  {
    vector_3d dir;
    randomize_direction (ran_, ref_dir_, dir);
    return dir;
  }

  /** Wrapper for rotation object
   */
  class rotation_wrapper_t : public rotation_3d
  {
  public:
    rotation_wrapper_t (double mxx_, double mxy_, double mxz_, 
			double myx_, double myy_, double myz_,
			double mzx_, double mzy_, double mzz_) 
      : rotation_3d (mxx_, mxy_, mxz_, 
		     myx_, myy_, myz_,
		     mzx_, mzy_, mzz_) {}
  };

  /** Class intercept_t hosts the paramater of the intercept 
   *  of a curve on a surfaceof a shape. 
   */
  class intercept_t
  {
  private:

    int       __shape_index; /** The index of the sub-shape
			      *  for composite shapes only.
			      *  Default is 0.
			      */
    int       __face;   //! The index of the impact face on the shape
    vector_3d __impact; //! The impact pointon the surface

  public:

    int get_shape_index () const
    {
      return __shape_index;
    }

    void set_shape_index (int si_)
    {
      __shape_index = si_;
    }

    int get_face () const
    {
      return __face;
    }

    void set_face (int face_)
    {
      __face = face_;
    }

    void set_impact (const vector_3d & point_)
    {
      __impact = point_;
    }

    void set (int shape_index_, int face_, const vector_3d & point_)
    {
      set_shape_index (shape_index_);
      set_face (face_);
      set_impact (point_);
    }

    const vector_3d & get_impact () const
    {
      return __impact;
    }

    void reset ()
    {
      __shape_index = -1;
      __face = FACE_NONE_BIT;
      invalidate (__impact);
    }

    intercept_t ()
    {
      __shape_index = -1;
      __face = FACE_NONE_BIT;
      invalidate (__impact);
    }

    bool is_valid () const
    {
      return __shape_index >= 0 && __face != FACE_NONE_BIT;
    }

    bool is_ok () const
    {
      return is_valid ();
    }

  };

  struct io
  {
    static const string VECTOR_2D_SERIAL_TAG;
    static const string VECTOR_3D_SERIAL_TAG;
    static const string ROTATION_3D_SERIAL_TAG;
    static const string POSITION_SUFFIX;
    static const string ROTATION_SUFFIX;
  };

} // end of namespace geomtools


//! Serialization stuff for CLHEP 'vector_3d'
namespace boost {

  namespace serialization {
    
    template<class Archive>
    void save (Archive & ar_ , 
	       const geomtools::vector_3d & v_,
	       const unsigned int version_)
    {
      double x = v_.getX ();
      double y = v_.getY ();
      double z = v_.getZ ();
      ar_ & boost::serialization::make_nvp ("x", x);
      ar_ & boost::serialization::make_nvp ("y", y);
      ar_ & boost::serialization::make_nvp ("z", z);
    }
    
    template<class Archive>
    void load (Archive & ar_ , 
	       geomtools::vector_3d & v_,
	       const unsigned int version_)
    {
      double x;
      double y;
      double z;
      ar_ & boost::serialization::make_nvp ("x", x);
      ar_ & boost::serialization::make_nvp ("y", y);
      ar_ & boost::serialization::make_nvp ("z", z);
      v_.set (x, y, z);
    }
    
    template<class Archive>
    void serialize (Archive & ar_,
		    geomtools::vector_3d  & v_,
		    const unsigned int version_)
    {
      boost::serialization::split_free (ar_, v_, version_);
    } 
    
  } // namespace serialization

} // namespace boost


//! Serialization stuff for CLHEP 'vector_2d'
namespace boost {

  namespace serialization {
    
    template<class Archive>
    void save (Archive & ar_ , 
	       const geomtools::vector_2d & v_,
	       const unsigned int version_)
    {
      double x = v_.x ();
      double y = v_.y ();
      ar_ & boost::serialization::make_nvp ("x", x);
      ar_ & boost::serialization::make_nvp ("y", y);
    }
    
    template<class Archive>
    void load (Archive & ar_ , 
	       geomtools::vector_2d & v_,
	       const unsigned int version_)
    {
      double x;
      double y;
      ar_ & boost::serialization::make_nvp ("x", x);
      ar_ & boost::serialization::make_nvp ("y", y);
      v_.set (x, y);
    }
    
    template<class Archive>
    void serialize (Archive & ar_,
		    geomtools::vector_2d  & v_,
		    const unsigned int version_)
    {
      boost::serialization::split_free (ar_, v_, version_);
    } 
    
  } // namespace serialization

} // namespace boost


//! Serialization stuff for CLHEP 'rotation'
namespace boost {

  namespace serialization {
    
    template<class Archive>
    void save (Archive & ar_ , 
	       const geomtools::rotation_3d & r_,
	       const unsigned int version_)
    {
      double rxx = r_.xx ();
      ar_ & boost::serialization::make_nvp ("xx", rxx);
      if (geomtools::is_valid (r_))
	{
	  double rxy, rxz, ryx, ryy, ryz, rzx, rzy, rzz;
	  rxy = r_.xy ();
	  rxz = r_.xz ();
	  ryx = r_.yx ();
	  ryy = r_.yy ();
	  ryz = r_.yz ();
	  rzx = r_.zx ();
	  rzy = r_.zy ();
	  rzz = r_.zz ();
	  ar_ & boost::serialization::make_nvp ("xy", rxy);
	  ar_ & boost::serialization::make_nvp ("xz", rxz);
	  ar_ & boost::serialization::make_nvp ("yx", ryx);
	  ar_ & boost::serialization::make_nvp ("yy", ryy);
	  ar_ & boost::serialization::make_nvp ("yz", ryz);
	  ar_ & boost::serialization::make_nvp ("zx", rzx);
	  ar_ & boost::serialization::make_nvp ("zy", rzy);
	  ar_ & boost::serialization::make_nvp ("zz", rzz);
	}
    }
    
    template<class Archive>
    void load (Archive & ar_ , 
	       geomtools::rotation_3d & r_,
	       const unsigned int version_)
    {
      double rxx (0.0), rxy (0.0), rxz (0.0);
      double ryx (0.0), ryy (0.0), ryz (0.0);
      double rzx (0.0), rzy (0.0), rzz (0.0);
      ar_ & boost::serialization::make_nvp ("xx", rxx); 
      if (rxx == rxx)
	{
	  ar_ & boost::serialization::make_nvp ("xy", rxy);
	  ar_ & boost::serialization::make_nvp ("xz", rxz);
	  ar_ & boost::serialization::make_nvp ("yx", ryx);
	  ar_ & boost::serialization::make_nvp ("yy", ryy);
	  ar_ & boost::serialization::make_nvp ("yz", ryz);
	  ar_ & boost::serialization::make_nvp ("zx", rzx);
	  ar_ & boost::serialization::make_nvp ("zy", rzy);
	  ar_ & boost::serialization::make_nvp ("zz", rzz);
	  r_ = geomtools::rotation_wrapper_t (rxx, rxy, rxz, ryx, ryy, ryz, rzx, rzy, rzz);
	}
      else 
	{
	  r_ = geomtools::rotation_3d ();
	  geomtools::invalidate (r_);
	}
    }
    
    template<class Archive>
    void serialize (Archive & ar_,
		    geomtools::rotation_3d  & r_,
		    const unsigned int version_)
    {
      boost::serialization::split_free (ar_, r_, version_);
    } 
    
  } // namespace serialization

} // namespace boost

#endif // __geomtools__utils_h

// end of utils.h
