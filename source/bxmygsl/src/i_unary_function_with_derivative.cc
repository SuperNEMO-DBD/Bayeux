// i_unary_function_with_derivative.cc

// Ourselves:
#include <mygsl/i_unary_function_with_derivative.h>

// Standard library:
#include <sstream>
#include <stdexcept>
#include <functional>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <mygsl/numerical_differentiation.h>

namespace mygsl {

  i_unary_function_with_derivative::i_unary_function_with_derivative (double epsilon_)
    : i_unary_function (epsilon_)
  {
    return;
  }

  i_unary_function_with_derivative::~i_unary_function_with_derivative ()
  {
    return;
  }

  double i_unary_function_with_derivative::eval_f (double x_) const
  {
    return eval(x_);
  }

  double i_unary_function_with_derivative::eval_df (double x_) const
  {
    return _eval_df_numeric(x_, get_epsilon());
  }

  void i_unary_function_with_derivative::eval_fdf (double x_, double & f_, double & df_) const
  {
    f_ = this->eval_f(x_);
    df_ = this->eval_df(x_);
    return;
  }

  double i_unary_function_with_derivative::_eval_df_numeric (double x_,
                                                             double h_) const
  {
    double df = std::numeric_limits<double>::quiet_NaN ();
    double h = get_epsilon();
    if (h_ > 0.0) {
      h = h_;
    }
    bool goon = true;
    int count = 0;
    int max_count = 3;
    while (goon) {
      double df_abserr = 0.0;
      double xmin = x_ - h;
      double xmax = x_ + h;
      bool xmin_ok = true;
      bool xmax_ok = true;
      if (has_explicit_domain_of_definition()) {
        DT_THROW_IF (! is_in_domain_of_definition (x_),
                     std::domain_error,
                     "Value '" << x_ << "' is out of the domain of definition !");
        xmin_ok = is_in_domain_of_definition (xmin);
        xmax_ok = is_in_domain_of_definition (xmax);
      }
      count++;
      goon = false;
      if (xmin_ok && xmax_ok) {
        df = derivative_central (*this, x_, h, df_abserr);
      } else if (! xmin_ok && xmax_ok) {
        df = derivative_forward (*this, x_, h, df_abserr);
      } else if (xmin_ok && ! xmax_ok) {
        df = derivative_backward (*this, x_, h, df_abserr);
      } else {
        goon = true;
        h /= 10.0;
      }
      if (count == max_count) {
        break;
      }
    }
    return df;
  }

  // Class unary_function_promoted_with_numeric_derivative:

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(unary_function_promoted_with_numeric_derivative,
                                             "mygsl::unary_function_promoted_with_numeric_derivative")

  bool unary_function_promoted_with_numeric_derivative::has_functor() const
  {
    return !_functor_.is_null();
  }

  void unary_function_promoted_with_numeric_derivative::set_functor(const i_unary_function & functor_)
  {
    _functor_.reset(functor_);
    return;
  }

  unary_function_promoted_with_numeric_derivative::unary_function_promoted_with_numeric_derivative()
    : _functor_()
  {
    return;
  }

  unary_function_promoted_with_numeric_derivative::unary_function_promoted_with_numeric_derivative (const i_unary_function & functor_)
    : _functor_(functor_)
  {
    return;
  }

  bool unary_function_promoted_with_numeric_derivative::is_in_domain_of_definition(double x_) const
  {
    return _functor_.func().is_in_domain_of_definition(x_);
  }

  bool unary_function_promoted_with_numeric_derivative::has_explicit_domain_of_definition() const
  {
    return _functor_.func().has_explicit_domain_of_definition();
  }

  double unary_function_promoted_with_numeric_derivative::get_non_zero_domain_min() const
  {
    return _functor_.func().get_non_zero_domain_min();
  }

  double unary_function_promoted_with_numeric_derivative::get_non_zero_domain_max() const
  {
    return _functor_.func().get_non_zero_domain_max();
  }

  double unary_function_promoted_with_numeric_derivative::_eval (double x_) const
  {
    return _functor_.func().eval(x_);
  }

  bool unary_function_promoted_with_numeric_derivative::is_initialized() const
  {
    return !_functor_.is_null();
  }

  void unary_function_promoted_with_numeric_derivative::reset()
  {
    _functor_.reset();
    return;
  }

} // namespace mygsl
