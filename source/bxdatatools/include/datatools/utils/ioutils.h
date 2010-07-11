// -*- mode: c++; -*-
// ioutils.h

#ifndef __datatools__utils__ioutils_h
#define __datatools__utils__ioutils_h 1

#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

#ifdef USING_NCURSES
#include <curses.h>
#endif // USING_NCURSES

#include <boost/cstdint.hpp>

using namespace std;

namespace datatools {
  
  namespace utils {
    
    template <class Type>
    class ostream_manipulator 
    {
    private:
      ostream & (*__function) (ostream &, const Type &);
      Type        __value;

    public:
      ostream_manipulator (ostream & (*function_) (ostream & , const Type &), 
			   const Type & value_)				
	: __function (function_), __value (value_) 
      { 
      }

      friend ostream & operator<<(ostream & os_,	
				  const ostream_manipulator & os_manip_) 
      {
	return os_manip_.__function (os_, os_manip_.__value);
      } 
  
    };
    
    template <class Type> 
    class ostream_manipulator_ref 
    {
    private:
      ostream & (*__function) (ostream & , Type &);
      Type    &   __value;

    public:
      ostream_manipulator_ref (ostream & (*function_) (ostream &, Type &) , 
			       Type & value_) 
	: __function (function_) , __value (value_)
      { 
      }

      friend ostream & operator<< (ostream & os_, 
				   const ostream_manipulator_ref & os_manip_) 
      {
	return os_manip_.__function (os_, os_manip_.__value);
      } 
    };

    struct io
    {
    private:
      static bool g__colored_stream;
#ifdef USING_NCURSES
      static SCREEN * g__screen;
#endif // USING_NCURSES
      static io g__io;

    public:

      class indenter
      {
	size_t __width;
	size_t __level;

      public:

	size_t get_width () const;
	size_t get_level () const;
	// ctor:
	indenter ();
	
	indenter & operator++ (int);
	indenter & operator-- (int);
	ostream & operator() (ostream &) const;
	indenter & operator () (size_t);

	friend ostream & operator<< (ostream &, 
				     const indenter &); 
      };

      static indenter indent;

      static bool is_colored ();

      static void set_colored (bool);

#ifdef USING_NCURSES
      static void focus ();
#endif // USING_NCURSES

    private:
      // ctor:
      io ()
      {
      }

      // dtor:
      ~io ()
      {
	set_colored (false);
      }

    public:

      static ostream & normal (ostream &);

      static ostream & reverse (ostream &);

      static ostream & bold (ostream &);

      static ostream & underline (ostream &);

      static ostream & red (ostream &);

      static ostream & green (ostream &);

      static ostream & debug (ostream &);

      static ostream & devel (ostream &);
    
      static ostream & notice (ostream &);

      static ostream & warning (ostream &);

      static ostream & error (ostream &);

      static ostream & tab (ostream &);

      static ostream & vtab (ostream &);

      static ostream & page_break (ostream &);

      static ostream & left (ostream &);

      static ostream & right (ostream &);

      static ostream & internal (ostream &);

      static ostream & showbase (ostream &);

      static ostream & ostream_width (ostream & os_, const int & n_); 
    
      static ostream_manipulator<int> width ( const int & n_); 
    
      static ostream & ostream_precision (ostream & os_, const int & n_); 
    
      static ostream_manipulator<int> precision (const int & n_); 

      static string to_binary (const uint32_t & val_);

    };

    /*
      string create_temporary (const string & template_, 
      const string & path_ = "")
      { 
      char * sdn = ".";
      ostringstream oss;
      if (! path_.empty ())
      {
      oss << path_;
      }
      char sfn[] = "/home/usr1/temp-XXXXXX";
      FILE * sfp;
      
      if (! secure_dir(sdn)) {
      // Handle error 
      }
      
      int fd = mkstemp(sfn);
      if (fd == -1) {
      // Handle error 
      }
      
      if (unlink(sfn) == -1) {
      // Handle error 
      }
      
      }
    */

  } // namespace utils

} // namespace datatools 

#endif // __datatools__utils__ioutils_h

// end of ioutils.h
