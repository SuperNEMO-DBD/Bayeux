/// \file datatools/ioutils.h
#ifndef DATATOOLS_IOUTILS_H
#define DATATOOLS_IOUTILS_H

// Standard Library:
#include <iostream>
#include <string>
#include <vector>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>

// This project:
#include <datatools/bit_mask.h>

namespace datatools {

  void print_multi_lines(std::ostream & out_,
                         const std::string & text_,
                         const std::string & indent_ = "");

  //----------------------------------------------------------------------
  /*! \brief ostream_manipulator class
   */
  template <class T>
  class ostream_manipulator
  {
  public:
    ostream_manipulator(std::ostream& (*function_)(std::ostream &, const T &),
                        const T & value_)
      : _function_(function_), _value_(value_)
    {
      return;
    }

    friend std::ostream& operator<<(std::ostream & os_,
                                    const ostream_manipulator & manip_)
    {
      return manip_._function_(os_, manip_._value_);
    }

  private:

    std::ostream & (*_function_)(std::ostream &, const T &);
    T _value_;

  };


  //----------------------------------------------------------------------
  /*! \brief ostream_manipulator_ref class
   */
  template <class T>
  class ostream_manipulator_ref
  {
  public:
    ostream_manipulator_ref(std::ostream & (*function_)(std::ostream &, T &),
                            T & value_)
      : _function_(function_), _value_(value_)
    {
      return;
    }

    friend std::ostream & operator<<(std::ostream & os_,
                                     const ostream_manipulator_ref & manip_)
    {
      return manip_._function_(os_, manip_._value_);
    }

  private:

    std::ostream & (*_function_)(std::ostream &, T &);
    T & _value_;

  };


  //----------------------------------------------------------------------
  /*! \brief I/O utilities
   */
  struct io {
  public:

    struct constants {
      /// A portable representation for a NaN
      static const std::string & nan_real_repr();
      /// A portable representation for +infinity
      static const std::string & plus_infinity_real_repr();
      /// A portable representation for -infinity
      static const std::string & minus_infinity_real_repr();
    };

    enum reader_flags {
      reader_no_flags                  = 0,
      reader_debug                     = bit_mask::bit00,
      reader_allow_trailing_characters = bit_mask::bit01,
      reader_case_sensitive            = bit_mask::bit02,
      reader_strict_quoting            = bit_mask::bit03
    };

    enum writer_flags {
      writer_no_flags       = 0,
      writer_debug          = bit_mask::bit00,
      writer_underscore_sep = bit_mask::bit01,
      writer_tilda_sep      = bit_mask::bit02,
      writer_strict_quoting = bit_mask::bit03
    };

    static const int REAL_PRECISION  = 16; ///< Default precision for double
    static const int REAL8_PRECISION = 16; ///< Default precision for double
    static const int REAL4_PRECISION = 8;  ///< Default precision for float

    /*! \brief I/O indenter class
     */
    class indenter {

    public:

      /// Default constructor
      indenter();

      /// Returns the with of each indentation level
      size_t get_width() const;

      /// Returns the current level of indentation
      size_t get_level() const;

      /// Increment indendation level by one unit
      indenter& operator++(int);

      /// Decrement indendation level by one unit
      indenter& operator--(int);

      std::ostream& operator()(std::ostream&) const;

      indenter& operator()(size_t);

      friend std::ostream& operator<<(std::ostream &, const indenter &);

    private:

      size_t _width_; //!< Width of each indentation level
      size_t _level_; //!< Current indentation level

    };

    /// \deprecated Convert traditional main function arguments
    ///             to a vector of strings
    static void convert_command_line_args(int argc_,
                                          char ** argv_,
                                          std::string & app_name_,
                                          std::vector<std::string> & args_);

    /// Parse a boolean
    static bool read_boolean(std::istream&, bool&, uint32_t flags = 0);

    /// Parse a boolean
    static bool read_boolean(const std::string&, bool&, uint32_t flags = 0);

    /// Write a boolean
    static void write_boolean(std::ostream& a_out, bool a_bool, bool as_alpha = false);

    /// Parse an integer
    static bool read_integer(std::istream&, int&, uint32_t flags = 0);

    /// Parse an integer
    static bool read_integer(const std::string&, int&, uint32_t flags = 0);

    /// Write an integer
    static void write_integer(std::ostream&, int);

    /// Parse a quoted string
    static bool read_quoted_string(std::istream&, std::string&, uint32_t flags = 0);

    /// Parse an integer
    static bool read_quoted_string(const std::string&, std::string&, uint32_t flags = 0);

    /// Write a quoted string
    static void write_quoted_string(std::ostream&, const std::string& a_str);

    /// Read a double value from an ASCII stream
    static bool read_real_number(std::istream & in_, double & val, bool & normal, uint32_t flags = 0);

    /// Read a double value from a string
    static bool read_real_number(const std::string &, double & val, bool & normal, uint32_t flags = 0);

    /// Write a double value in an ASCII stream with unit support
    static void write_real_number(std::ostream & out_,
                                  double val_,
                                  int precision_ = REAL_PRECISION,
                                  const std::string& unit_symbol_ = "",
                                  const std::string& unit_label_ = "",
                                  double unit_value_ = 1.0,
                                  uint32_t flags_ = 0);

    bool is_colored() const;

    void set_colored(bool);

    static std::ostream& normal(std::ostream&);

    static std::ostream& reverse(std::ostream&);

    static std::ostream& bold(std::ostream&);

    static std::ostream& underline(std::ostream&);

    static std::ostream& red(std::ostream&);

    static std::ostream& green(std::ostream&);

    static std::ostream& debug(std::ostream&);

    static std::ostream& devel(std::ostream&);

    static std::ostream& notice(std::ostream&);

    static std::ostream& warning(std::ostream&);

    static std::ostream& error(std::ostream&);

    static std::ostream& verbose(std::ostream&);

    static std::ostream& tab(std::ostream&);

    static std::ostream& vtab(std::ostream&);

    static std::ostream& page_break(std::ostream&);

    static std::ostream& left(std::ostream&);

    static std::ostream& right(std::ostream&);

    static std::ostream& internal(std::ostream&);

    static std::ostream& showbase(std::ostream&);

    static std::ostream& ostream_width(std::ostream& os, const int& n);

    static ostream_manipulator<int> width(const int& n);

    static std::ostream& ostream_precision(std::ostream& os, const int& n);

    static ostream_manipulator<int> precision(const int & n);

    static std::string to_binary(const uint32_t& val, bool show_all = false);

    static std::string to_binary_2(const uint32_t& val);

    // Return singleton indenter
    static indenter & indent();

    // Return singleton I/O object
    static io & instance();

  protected:

    /// \brief Default constructor
    io();

  public:

    /// \brief Destructor
    // boost::check_delete<> needs it to be public
    ~io();

  private:

    bool _colored_stream_; //!< Color flag

  };

} // namespace datatools

#endif // DATATOOLS_IOUTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
