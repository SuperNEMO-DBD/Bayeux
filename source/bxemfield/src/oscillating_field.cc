// oscillating_field.cc

// Ourselves:
#include <emfield/oscillating_field.h>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux:
#include <datatools/properties.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>
#include <geomtools/utils.h>

namespace emfield {

  // Registration instantiation macro :
  EMFIELD_REGISTRATION_IMPLEMENT(oscillating_field, "emfield::oscillating_field")

  void oscillating_field::_set_defaults()
  {
    _set_electric_field(false);
    _set_electric_field_can_be_combined(true);
    _set_magnetic_field(false);
    _set_magnetic_field_can_be_combined(false);
    _set_electric_field_is_time_dependent(true);
    _set_magnetic_field_is_time_dependent(true);
    datatools::invalidate(_frequency_);
    datatools::invalidate(_phase_);
    datatools::invalidate(_pedestal_);
    datatools::invalidate(_scale_);
    _sin_cos_mode_ = SC_MODE_INVALID;
    return;
  }

  // Constructor :
  oscillating_field::oscillating_field(uint32_t flags_)
    : base_electromagnetic_field(flags_)
  {
    _set_defaults();
    return;
  }

  // Destructor :
  oscillating_field::~oscillating_field()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void oscillating_field::set_sine_cosine_mode(mode_sin_cos_type sc_mode_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Cannot change the sine/cosine mode value !");
    _sin_cos_mode_ = sc_mode_;
    return;
  }

  bool oscillating_field::is_cosine_mode() const
  {
    return _sin_cos_mode_ == SC_MODE_COSINE;
  }

  bool oscillating_field::is_sine_mode() const
  {
    return _sin_cos_mode_ == SC_MODE_SINE;
  }

  double oscillating_field::get_frequency() const
  {
    return _frequency_;
  }

  double oscillating_field::get_period() const
  {
    return 1. / _frequency_;
  }

  double oscillating_field::get_phase() const
  {
    return _phase_;
  }

  double oscillating_field::get_pedestal() const
  {
    return _pedestal_;
  }

  double oscillating_field::get_scale() const
  {
    return _scale_;
  }

  void oscillating_field::set_frequency(double f_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the frequency value !");
    _frequency_ = f_;
    return;
  }

  void oscillating_field::set_phase(double p_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the phase value !");
    _phase_ = p_;
    return;
  }

  void oscillating_field::set_pedestal(double p_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the pedestal value !");
    _pedestal_ = p_;
    return;
  }

  void oscillating_field::set_scale(double s_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the scale value !");
    _scale_ = s_;
    return;
  }

  void oscillating_field::set_field(base_electromagnetic_field::handle_type & f_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the EM field !");
    _field_ = f_;
    return;
  }

  double oscillating_field::_get_coefficient(double time_) const
  {
    double ph = 2.* M_PI * _frequency_ * time_ + _phase_;
    double term;
    if (is_cosine_mode()) {
      term = std::cos(ph);
    } else {
      term = std::sin(ph);
    }
    return _pedestal_ + _scale_ * term;
  }

  int oscillating_field::compute_electric_field(const ::geomtools::vector_3d & position_,
                                                double time_,
                                                ::geomtools::vector_3d & electric_field_) const
  {
    geomtools::invalidate(electric_field_);
    if (! _field_.get().is_electric_field()) {
      return STATUS_ERROR;
    }
    int status = _field_.get().compute_electric_field(position_,
                                                      time_,
                                                      electric_field_);
    if (status != STATUS_SUCCESS) {
      return status;
    }
    electric_field_ *= _get_coefficient(time_);
    return STATUS_SUCCESS;
  }

  int oscillating_field::compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                                double time_,
                                                ::geomtools::vector_3d & magnetic_field_) const
  {
    geomtools::invalidate(magnetic_field_);
    if (! _field_.get().is_magnetic_field()) {
      return STATUS_ERROR;
    }
    int status = _field_.get().compute_magnetic_field(position_,
                                                      time_,
                                                      magnetic_field_);
    if (status != STATUS_SUCCESS) {
      return status;
    }
    magnetic_field_ *= _get_coefficient(time_);
    return STATUS_SUCCESS;
  }

  void oscillating_field::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Cannot reset the electromagnetic field !");

    _set_initialized(false);
    _field_.reset();
    _set_defaults();

    return;
  }

  void oscillating_field::initialize(const ::datatools::properties & setup_,
                                     ::datatools::service_manager & service_manager_,
                                     ::emfield::base_electromagnetic_field::field_dict_type & fields_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");

    base_electromagnetic_field::_parse_basic_parameters(setup_, service_manager_, fields_);

    double phase_unit = CLHEP::degree;
    double frequency_unit = CLHEP::hertz;

    double frequency = 0.0 * frequency_unit;
    double phase     = 0.0 * phase_unit;
    double pedestal  = 1.0; // dimensionless
    double scale     = 1.0; // dimensionless

    if (setup_.has_key("phase.unit")) {
      const std::string phase_unit_str = setup_.fetch_string("phase.unit");
      phase_unit = datatools::units::get_angle_unit_from(phase_unit_str);
    }

    if (setup_.has_key("frequency.unit")) {
      const std::string frequency_unit_str = setup_.fetch_string("frequency.unit");
      frequency_unit = datatools::units::get_frequency_unit_from(frequency_unit_str);
    }


    if (_sin_cos_mode_ == SC_MODE_INVALID) {
      if (setup_.has_key("sin_cos_mode")) {
        const std::string & scmode_label = setup_.fetch_string("sin_cos_mode");
        if (scmode_label == "cosine") {
          set_sine_cosine_mode(SC_MODE_COSINE);
        } else if (scmode_label == "sine") {
          set_sine_cosine_mode(SC_MODE_SINE);
        } else {
          DT_THROW(std::logic_error, "Invalid sine/cosine mode label '"
                   << scmode_label << "'!");
        }
      } else {
        set_sine_cosine_mode(SC_MODE_DEFAULT);
      }
    }

    if (!datatools::is_valid(_frequency_)) {
      if (setup_.has_key("frequency")) {
        frequency = setup_.fetch_real("frequency");
        if (! setup_.has_explicit_unit("frequency")) {
          frequency *= frequency_unit;
        }
      }
      set_frequency(frequency);
    }

    if (!datatools::is_valid(_phase_)) {
      if (setup_.has_key("phase")) {
        phase = setup_.fetch_real("phase");
        if (! setup_.has_explicit_unit("phase")) {
          phase *= phase_unit;
        }
      }
      set_phase(phase);
    }

    if (!datatools::is_valid(_pedestal_)) {
      if (setup_.has_key("pedestal")) {
        pedestal = setup_.fetch_real("pedestal");
      }
      set_pedestal(pedestal);
    }

    if (!datatools::is_valid(_scale_)) {
      if (setup_.has_key("scale")) {
        scale = setup_.fetch_real("scale");
      }
      set_scale(scale);
    }

    if (! _field_.has_data()) {
      // Parameters of the oscillating field:
      std::string field_name;
      DT_THROW_IF(!setup_.has_key("field.name"), std::logic_error,
                  "Missing 'field.name' property!");
      field_name = setup_.fetch_string("field.name");
      {
        base_electromagnetic_field::field_dict_type::iterator field_found
          = fields_.find(field_name);
        DT_THROW_IF(field_found == fields_.end(), std::logic_error,
                    "Cannot find a EM field with name '" << field_name << "' !");
        set_field(field_found->second);
      }
    }
    _set_electric_field(_field_.get().is_electric_field ());
    _set_magnetic_field(_field_.get().is_magnetic_field ());

    _set_initialized(true);
    return;
  }

  // virtual
  void oscillating_field::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    this->base_electromagnetic_field::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Field : " << "'" << _field_.get().get_name() << "' "
         << "(class ID='" <<  _field_.get().get_class_id() << "' "
         << "[@" << &_field_.get() << "])" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Sine/cosine : " <<  (is_cosine_mode() ? "Cosine" : "Sine") << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Frequency : " << _frequency_ / CLHEP::megahertz << " MHz"
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Phase : " << _phase_ / CLHEP::degree << " degree"
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Pedestal : " << _pedestal_
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Scale factor : " << _scale_
         << std::endl;

    return;
  }

} // end of namespace emfield
