// -*- mode: c++ ; -*- 
/* stacked_model.cc
 */

#include <geomtools/stacked_model.h>
//
#include <exception>
#include <limits> 

#include <mygsl/min_max.h>

#include <datatools/utils/units.h>

#include <geomtools/i_stackable.h>
//#include <geomtools/regular_linear_placement.h>
#include <geomtools/visibility.h>

namespace geomtools {

  using namespace std;

  const string stacked_model::STACKED_PROPERTY_PREFIX = "stacked.";
  const string stacked_model::STACKED_MODEL_PROPERTY_PREFIX = "stacked.model_";
  const string stacked_model::STACKED_LABEL_PROPERTY_PREFIX = "stacked.label_";
  const string stacked_model::DEFAULT_STACKED_LABEL_PREFIX  = "stacked";
  const double stacked_model::DEFAULT_MECHANICS_PLAY = 0.0 * CLHEP::mm;
  const double stacked_model::DEFAULT_NUMERICS_PLAY = 0.0 * CLHEP::mm;


  /****************************************************************/

  stacked_model::stacked_item::stacked_item ()
  {
    model = 0;
    datatools::utils::invalidate (limit_min);
    datatools::utils::invalidate (limit_max);
    return;
  }

  double stacked_model::stacked_item::get_limit_min () const
  {
    return limit_min;
  }
  
  double stacked_model::stacked_item::get_limit_max () const
  {
    return limit_max;
  }
  
  bool stacked_model::stacked_item::has_limit_min () const
  {
    return datatools::utils::is_valid (limit_min);
  }

  bool stacked_model::stacked_item::has_limit_max () const
  {
    return datatools::utils::is_valid (limit_max);
  }

  /****************************************************************/

  // registration ID:
  string stacked_model::get_model_id () const
  {
    return "geomtools::stacked_model";
  }

  const string & stacked_model::get_material_name () const
  {
    assert_constructed ("geomtools::stacked_model::get_material_name");
    return __get_material_name ();
  }

  const string & stacked_model::__get_material_name () const
  {
    return __material_name;
  }

  void stacked_model::set_material_name (const string & mn_)
  {
    assert_unconstructed("geomtools::stacked_model::set_material_name");

    __material_name = mn_;
  }

  void stacked_model::set_stacking_axis (int a_)
  {
    assert_unconstructed ("geomtools::stacked_model::set_stacking_axis");

    if ((a_ < STACKING_ALONG_X) || (a_ > STACKING_ALONG_Z))
      {
	throw logic_error ("geomtools::stacked_model::set_staking_axis: Invalid stacking axis !");
      }
    __stacking_axis = a_;
  }

  int stacked_model::get_stacking_axis () const
  {
    assert_constructed ("geomtools::stacked_model::get_stacking_axis");
    return __get_stacking_axis ();
  }

  int stacked_model::__get_stacking_axis () const
  {
    return __stacking_axis;
  }

  bool stacked_model::is_stacking_along_x () const
  {
    return __get_stacking_axis () == STACKING_ALONG_X;
  }
  
  bool stacked_model::is_stacking_along_y () const
  {
    return __get_stacking_axis () == STACKING_ALONG_Y;
  }
  
  bool stacked_model::is_stacking_along_z () const
  {
    return __get_stacking_axis () == STACKING_ALONG_Z;
  }
  
  const geomtools::box & stacked_model::get_box () const
  {
    assert_constructed ("geomtools::stacked_model::get_box");
    return __solid;
  }
  
  const geomtools::box & stacked_model::get_solid () const
  {
    assert_constructed ("geomtools::stacked_model::get_solid");
    return __solid;
  }

  void stacked_model::add_stacked_model (int i_, const i_model & model_, const string & label_)
  {
    assert_unconstructed ("geomtools::stacked_model::add_stacked_model");

    stacked_dict_t::const_iterator found = __stacked_models.find (i_);
    if (found != __stacked_models.end ())
      {
	ostringstream message;
	message << "geomtools::stacked_model::add_stacked_model: "
		<< "Dictionary already has a model with "
		<< "index '" << i_ << "' !";
	throw logic_error (message.str ());
      }
    stacked_item bi;
    __stacked_models[i_] = bi;
    __stacked_models[i_].model = &model_;
    __stacked_models[i_].placmt.invalidate ();
    string label = label_;
    if (label.empty ())
      {
	ostringstream label_oss;
	label_oss << DEFAULT_STACKED_LABEL_PREFIX << "__" << i_ << "__";
	label = label_oss.str ();
      }
    else
      {
	if (__labels.find (label) != __labels.end ())
	  {
	    ostringstream message;
	    message << "geomtools::stacked_model::add_stacked_model: "
		    << "Label '" << label << "' is already used !";
	    throw logic_error (message.str ());
	  }
      }
    __stacked_models[i_].label = label;
    __labels[label] = i_;
    return;
  }

  const stacked_model::stacked_dict_t & stacked_model::get_models () const
  {
    return __stacked_models;
  }

  const stacked_model::labels_dict_t & stacked_model::get_labels () const
  {
    return __labels;
  }

  size_t stacked_model::get_number_of_stacked_models () const
  {
    return __stacked_models.size ();
  }

  bool stacked_model::has_stacked_model (const string & label_) const
  {
    labels_dict_t::const_iterator found = __labels.find (label_);
    return found != __labels.end ();
  }

  const stacked_model::stacked_item & 
  stacked_model::get_stacked_item (const string & label_) const
  {
    labels_dict_t::const_iterator found = __labels.find (label_);
    if (found == __labels.end ())
      {
	ostringstream message;
	message << "geomtools::stacked_model::get_stacked_item: "
		<< "Dictionary has no model with "
		<< "label '" << label_ << "' !";
	throw logic_error (message.str ());
      }
    return (this->get_stacked_item (found->second));
  }

  const stacked_model::stacked_item & 
  stacked_model::get_stacked_item (int i_) const
  {
    stacked_dict_t::const_iterator found = __stacked_models.find (i_);
    if (found == __stacked_models.end ())
      {
	ostringstream message;
	message << "geomtools::stacked_model::get_stacked_item: "
		<< "Dictionary has no model item with "
		<< "index '" << i_ << "' !";
	throw logic_error (message.str ());
      }
    return found->second;
  }

  const i_model & stacked_model::get_stacked_model (const string & label_) const
  {
    labels_dict_t::const_iterator found = __labels.find (label_);
    if (found == __labels.end ())
      {
	ostringstream message;
	message << "geomtools::stacked_model::get_stacked_model: "
		<< "Dictionary has no model with "
		<< "label '" << label_ << "' !";
	throw logic_error (message.str ());
      }
    return this->get_stacked_model (found->second);
  }

  const i_model & stacked_model::get_stacked_model (int i_) const
  {
    stacked_dict_t::const_iterator found = __stacked_models.find (i_);
    if (found == __stacked_models.end ())
      {
	ostringstream message;
	message << "geomtools::stacked_model::get_stacked_model: "
		<< "Dictionary has no model with "
		<< "index '" << i_ << "' !";
	throw logic_error (message.str ());
      }
    return *(found->second.model);
  }
  
  // ctor:
  stacked_model::stacked_model () : i_boxed_model ()
  {
    __material_name = "";
    __stacking_axis = STACKING_ALONG_INVALID;
    __mechanics_play = DEFAULT_MECHANICS_PLAY;
    __numerics_play = DEFAULT_NUMERICS_PLAY;
    return;
  }
  
  // dtor:
  stacked_model::~stacked_model ()
  {
    return;
  }
  
  void stacked_model::_at_construct (const string & name_,
				     const datatools::utils::properties & config_,
				     models_col_t * models_)
  {
    bool devel = i_model::g_devel;
    if (config_.has_flag ("devel"))
      {
	devel = true;
      }  
    if (devel)
      {
	clog << "DEVEL: stacked_model::_at_construct: Entering..." << endl;
      }
    set_name (name_);
    size_t number_of_items;
    string stacked_model_name;
    string label_name;
    string stacking_axis_label = "";
    int    stacking_axis;
    string material_name;
    double x, y, z;
    double lunit = CLHEP::mm;

    if (config_.has_flag ("debug"))
      {
	set_debug (true);
      }  

    /*** material ***/
    if (config_.has_key ("material.ref"))
      {
	material_name = config_.fetch_string ("material.ref");
      }  
    else
      {
	ostringstream message;
	message << "geomtools::stacked_model::_at_construct: "
		<< "Missing 'material.ref' property !"; 
	throw logic_error (message.str ());		
      }
    set_material_name (material_name);

    /*** Length unit ***/
    if (config_.has_key ("length_unit"))
      {
	string length_unit_str = config_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (length_unit_str);
      }  

    /*** Position play ***/
    if (config_.has_key ("mechanics_play"))
      {
	double mechanics_play = config_.fetch_real ("mechanics_play");
	mechanics_play *= lunit;
	if (mechanics_play < 0.0)
	  {
	    ostringstream message;
	    message << "geomtools::stacked_model::_at_construct: "
		    << "Mechanics play '" <<  mechanics_play / CLHEP::mm << "' mm cannnot be negative !"; 
	    throw logic_error (message.str ());    
	  }
        __mechanics_play = mechanics_play;
      }  

    if (config_.has_key ("numerics_play"))
      {
	double numerics_play = config_.fetch_real ("numerics_play");
	numerics_play *= lunit;
	if (numerics_play < 0.0)
	  {
	    ostringstream message;
	    message << "geomtools::stacked_model::_at_construct: "
		    << "Numerics play '" <<  numerics_play / CLHEP::mm << "' mm cannnot be negative !"; 
	    throw logic_error (message.str ());    
	  }
        __numerics_play = numerics_play;
      }  
 
    /*** Stacking axis ***/
    if (config_.has_key ("stacked.axis"))
      {
	stacking_axis_label = config_.fetch_string ("stacked.axis");
      }  
    else
      {
	ostringstream message;
	message << "geomtools::stacked_model::_at_construct: "
		<< "Missing 'stacked.axis' property !"; 
	throw logic_error (message.str ());		
      }
    if (stacking_axis_label == "x")
      {
	stacking_axis = STACKING_ALONG_X;
      }
    else if (stacking_axis_label == "y")
      {
	stacking_axis = STACKING_ALONG_Y;
      }
    else if (stacking_axis_label == "z")
      {
	stacking_axis = STACKING_ALONG_Z;
      }
    set_stacking_axis (stacking_axis);

    /*** number of stacked stacked models ***/
    if (config_.has_key ("stacked.number_of_items"))
      {
	number_of_items = config_.fetch_integer ("stacked.number_of_items");
      }  
    else
      {
	ostringstream message;
	message << "geomtools::stacked_model::_at_construct: "
		<< "Missing 'stacked.number_of_items' property !"; 
	throw logic_error (message.str ());		
      }

    if (devel) cerr << "DEVEL: stacked_model::_at_construct: " << "step 1" << endl;

    /*** check models ***/
    if (! models_)
      {
	ostringstream message;
	message << "geomtools::stacked_model::_at_construct: "
		<< "Missing logicals dictionary !"; 
	throw logic_error (message.str ());
      }

    if (devel) cerr << "DEVEL: stacked_model::_at_construct: " << "step 2" << endl;

    /*** loop over models to be stacked ***/
    for (int i = 0; i < number_of_items; i++)
      {
	string stacked_model_name;
	string label_name;
	ostringstream stacked_item_prop;
	stacked_item_prop << "stacked.model_" << i;
	if (config_.has_key (stacked_item_prop.str ()))
	  {
	    stacked_model_name = config_.fetch_string (stacked_item_prop.str ());
	  }  
	else
	  {
	    ostringstream message;
	    message << "geomtools::stacked_model::_at_construct: "
		    << "Missing '" << stacked_item_prop.str () << "' property !"; 
	    throw logic_error (message.str ());	
	  }
	// attempt to extract a user defined label:
	ostringstream label_item_prop;
	label_item_prop << STACKED_LABEL_PROPERTY_PREFIX  << i;
	if (config_.has_key (label_item_prop.str ()))
	  {
	    label_name = config_.fetch_string (label_item_prop.str ());
	  }  

	models_col_t::const_iterator found = 
	  models_->find (stacked_model_name);
	if (found != models_->end ())
	  {
	    // check if the model is stackable:
	    if (! i_shape_3d::is_stackable (found->second->get_logical ().get_shape ()))
	      {
		ostringstream message;
		message << "geomtools::stacked_model::_at_construct: "
			<< "The embedded model '" << found->second->get_name () 
			<< "' is not stackable !"; 
		throw logic_error (message.str ());
	      }
	    add_stacked_model (i, *(found->second), label_name);
	  }
	else
	  {
	    ostringstream message;
	    message << "geomtools::stacked_model::_at_construct: "
		    << "Cannot find model with name '" 
		    << stacked_model_name << "' !";
	    throw logic_error (message.str ());
	  }
      }

    if (devel) cerr << "DEVEL: stacked_model::_at_construct: " << "step 3" << endl;

    /*** compute main box dimensions ***/
    mygsl::min_max mmx;
    mygsl::min_max mmy;
    mygsl::min_max mmz;
    double stacked_x = 0.0;
    double stacked_y = 0.0;
    double stacked_z = 0.0;
    for (stacked_dict_t::const_iterator i = __stacked_models.begin ();
	 i != __stacked_models.end ();
	 i++)
      {
	int index = i->first;
	const stacked_item & bi = i->second;
	const i_model * stacked_model = bi.model;

	const i_shape_3d & the_shape = stacked_model->get_logical ().get_shape ();

	// try to get a stackable data from the shape:
	stackable_data the_SD;
	if (! i_shape_3d::pickup_stackable (the_shape, the_SD))
	  {
	    ostringstream message;
	    message << "geomtools::stacked_model::_at_construct: "
		    << "Cannot stack '" 
		    << the_shape.get_shape_name () << "' shape !";
	    throw logic_error (message.str ());
	  }

	if (devel) 
	  {
	    cerr << "DEVEL: stacked_model::_at_construct: " << "step 3d: dump stackable data for '" << stacked_model->get_name () << "' from '" << name_ << "'..." << endl;

	    the_SD.tree_dump (cerr, "geomtools::stacked_model::_at_construct: Stackable data:", "DEVEL: ");
	  }
	if (devel) cerr << "DEVEL: stacked_model::_at_construct: " << "step 3e: gets..." << endl;

	double gxmin = the_SD.get_xmin ();
	double gymin = the_SD.get_ymin ();
	double gzmin = the_SD.get_zmin ();
	double gxmax = the_SD.get_xmax ();
	double gymax = the_SD.get_ymax ();
	double gzmax = the_SD.get_zmax ();

	// Parse special stacking position directives:
	double gmin, gmax;
	datatools::utils::invalidate (gmin);
	datatools::utils::invalidate (gmax);
	{
	  string stacked_model_name;
	  string label_name;
	  ostringstream stacked_min_prop;
	  stacked_min_prop << "stacked.limit_min_" << index;
	  if (config_.has_key (stacked_min_prop.str ()))
	    {
	      gmin = config_.fetch_real (stacked_min_prop.str ());
	      gmin *= lunit;
	      stacked_item & mod_bi = const_cast <stacked_item &> (bi);
	      mod_bi.limit_min = gmin;
	    }  
	}
	{
	  string stacked_model_name;
	  string label_name;
	  ostringstream stacked_max_prop;
	  stacked_max_prop << "stacked.limit_max_" << index;
	  if (config_.has_key (stacked_max_prop.str ()))
	    {
	      gmax = config_.fetch_real (stacked_max_prop.str ());
	      gmax *= lunit;
	      stacked_item & mod_bi = const_cast <stacked_item &> (bi);
	      mod_bi.limit_max = gmax;
	    }  
	}

	if (bi.has_limit_min ())
	  {
	    if (is_stacking_along_x ())
	      {
		gxmin = bi.get_limit_min ();
	      }
	    else if (is_stacking_along_y ())
	      {
		gymin = bi.get_limit_min ();
	      }
	    else if (is_stacking_along_z ())
	      {
		gzmin = bi.get_limit_min ();
	      }	    
	  }
	if (bi.has_limit_max ())
	  {
	    if (is_stacking_along_x ())
	      {
		gxmax = bi.get_limit_max ();
	      }
	    else if (is_stacking_along_y ())
	      {
		gymax = bi.get_limit_max ();
	      }
	    else if (is_stacking_along_z ())
	      {
		gzmax = bi.get_limit_max ();
	      }	    
	  }

	// Compute the effective dimensions of the stacked item within the stack:
	double full_x = gxmax - gxmin;
	double full_y = gymax - gymin;
	double full_z = gzmax - gzmin;
	
	// Not sure of that:
	mmx.add (full_x);
	mmy.add (full_y);
	mmz.add (full_z);
	
	if (devel) cerr << "DEVEL: stacked_model::_at_construct: " << "step 3f: mmx... add" << endl;
	if (is_stacking_along_x ())
	  {
	    stacked_x += full_x;
	  }
	else if (is_stacking_along_y ())
	  {
	    stacked_y += full_y;
	  }
	else if (is_stacking_along_z ())
	  {
	    stacked_z += full_z;
	  }
      }

    if (devel) cerr << "DEVEL: stacked_model::_at_construct: " << "step 4" << endl;

    double max = -1.0;
    if (is_stacking_along_x ())
      {
	stacked_y = mmy.get_max ();
	stacked_z = mmz.get_max ();
      }
    else if (is_stacking_along_y ())
      {
	stacked_x = mmx.get_max ();
	stacked_z = mmz.get_max ();
      }
    else if (is_stacking_along_z ())
      {
	stacked_x = mmx.get_max ();
	stacked_y = mmy.get_max ();
      }

    double dim_x = stacked_x;
    double dim_y = stacked_y;
    double dim_z = stacked_z;

    if (config_.has_key ("x"))
      {
	x = config_.fetch_real ("x");
	x *= lunit;
	if (x < stacked_x)
	  {
	    ostringstream message;
	    message << "geomtools::stacked_model::_at_construct: "
		    << "Model '" << get_name () << "' : "
		    << "Enforced X dimension '" << x / CLHEP::mm << "' mm (<" << stacked_x / CLHEP::mm << ") is too small for stacked components to fit !"; 
	    throw logic_error (message.str ());    
	  }
        dim_x = x;
      }  

    if (config_.has_key ("y"))
      {
	y = config_.fetch_real ("y");
	y *= lunit;
	if (y < stacked_y)
	  {
	    ostringstream message;
	    message << "geomtools::stacked_model::_at_construct: "
		    << "Model '" << get_name () << "' : "
		    << "Enforced Y dimension '" << y / CLHEP::mm << "' mm (<" << stacked_y / CLHEP::mm << ") is too small for stacked components to fit !"; 
	    throw logic_error (message.str ());    
	  }
        dim_y = y;
      }  

    if (config_.has_key ("z"))
      {
	z = config_.fetch_real ("z");
	z *= lunit;
	if (z < stacked_z)
	  {
	    ostringstream message;
	    message << "geomtools::stacked_model::_at_construct: "
		    << "Model '" << get_name () << "' : "
		    << "Enforced Z dimension '" << z / CLHEP::mm << "' mm (<" << stacked_z / CLHEP::mm << ") is too small for stacked components to fit !"; 
	    throw logic_error (message.str ());    
	  }
        dim_z = z;
      }  

    if (devel) cerr << "DEVEL: stacked_model::_at_construct: " << "step 5" << endl;

    __solid.reset ();

    if (__mechanics_play > 0.0)
      {
	dim_x += __mechanics_play;
	dim_y += __mechanics_play;
	dim_z += __mechanics_play;
      }
    if (__numerics_play > 0.0)
      {
	dim_x += __numerics_play;
	dim_y += __numerics_play;
	dim_z += __numerics_play;
      }
    __solid.set_x (dim_x);
    __solid.set_y (dim_y);
    __solid.set_z (dim_z);
    if (! __solid.is_valid ())
      {
	throw logic_error ("geomtools::stacked_model::_at_construct: Invalid solid !");
      }

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (__solid);
    get_logical ().set_material_ref (__get_material_name ());

    // starting position:
    double pos;
    if (is_stacking_along_x ())
      {
	pos = -0.5 * stacked_x;
      }
    else if (is_stacking_along_y ())
      {
	pos = -0.5 * stacked_y;
      }
    else if (is_stacking_along_z ())
      {
	pos = -0.5 * stacked_z;
      }

    if (devel) cerr << "DEVEL: stacked_model::_at_construct: " << "step 6" << endl;

    int j = 0;
    for (stacked_dict_t::iterator i = __stacked_models.begin ();
	 i != __stacked_models.end ();
	 i++)
      {
	stacked_item & bi = i->second;
	const i_model * stacked_model = bi.model;
	double xi, yi, zi;
	xi = yi = zi = 0.0;

	const i_shape_3d & the_shape = stacked_model->get_logical ().get_shape ();
	stackable_data the_SD;
	if (! i_shape_3d::pickup_stackable (the_shape, the_SD))
	  {
	    ostringstream message;
	    message << "geomtools::stacked_model::_at_construct: "
		    << "Model '" << get_name () << "' : "
		    << "Cannot stack '" 
		    << the_shape.get_shape_name () << "' shape !";
	    throw logic_error (message.str ());
	  }
	double x0 = 0.0;
	double y0 = 0.0;
	double z0 = 0.0;

	double gxmin = the_SD.get_xmin ();
	double gymin = the_SD.get_ymin ();
	double gzmin = the_SD.get_zmin ();
	double gxmax = the_SD.get_xmax ();
	double gymax = the_SD.get_ymax ();
	double gzmax = the_SD.get_zmax ();

	// Eventually extract specific stacking limits:
	if (bi.has_limit_min ())
	  {
	    if (is_stacking_along_x ())
	      {
		gxmin = bi.get_limit_min ();
	      }
	    else if (is_stacking_along_y ())
	      {
		gymin = bi.get_limit_min ();
	      }
	    else if (is_stacking_along_z ())
	      {
		gzmin = bi.get_limit_min ();
	      }	    
	  }
	if (bi.has_limit_max ())
	  {
	    if (is_stacking_along_x ())
	      {
		gxmax = bi.get_limit_max ();
	      }
	    else if (is_stacking_along_y ())
	      {
		gymax = bi.get_limit_max ();
	      }
	    else if (is_stacking_along_z ())
	      {
		gzmax = bi.get_limit_max ();
	      }	    
	  }

	if (is_stacking_along_x ())
	  {
	    xi = pos - gxmin;
	    pos += gxmax - gxmin;
	  }
	else if (is_stacking_along_y ())
	  {
	    yi = pos - gymin;
	    pos += gymax - gymin;
	  }
	else if (is_stacking_along_z ())
	  {
	    zi = pos - gzmin;
	    pos += gzmax - gzmin;
	  }
	//double stacked_rotation_angle;
	bi.placmt.set (xi, yi, zi, 0.0, 0.0);
	bi.phys.set_name (i_model::make_physical_volume_name (bi.label));
	bi.phys.set_placement (bi.placmt);
	bi.phys.set_logical (bi.model->get_logical ());
	bi.phys.set_mother (_logical);
	j++;
      } // for

    if (devel) cerr << "DEVEL: stacked_model::_at_construct: " << "step 7" << endl;

    if (devel) clog << "DEVEL: stacked_model::_at_construct: Exiting." << endl;
    return;
  }

  void stacked_model::tree_dump (ostream & out_, 
				 const string & title_ , 
				 const string & indent_, 
				 bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    {
      out_ << indent << i_tree_dumpable::tag 
	   << "Material : " << get_material_name () << endl;
    }

    {
      out_ << indent << i_tree_dumpable::tag 
	   << "Stacking axis : " << get_stacking_axis () << endl;
    }
     
    {
      for (labels_dict_t::const_iterator i = __labels.begin ();
	   i != __labels.end ();
	   i++)
	{
	  out_ << indent << i_tree_dumpable::tag 
	       << "Stacked model : " << "'" << i->first << "'" << " [rank==" << i->second << "]" << endl;
	}
    }

    {
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	   << "Solid : " << endl;
      {
	ostringstream indent_oss;
	indent_oss << indent;
	indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	__solid.tree_dump (out_, "", indent_oss.str ());
      }   
    }

    return;
  }

  // register this creator:   
  geomtools::i_model::creator_registration<stacked_model> stacked_model::__CR;
      
} // end of namespace geomtools

// end of stacked_model.cc
