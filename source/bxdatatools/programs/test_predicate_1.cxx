// -*- mode: c++; -*- 
// test_predicate_1.cxx
// Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <datatools/utils/i_predicate.h>
#include <datatools/utils/handle.h>

using namespace std;

/** Some test class 
 */
class foo 
{
private:
  string color_;

public:
  const string & get_color () const
  {
    return color_;
  }

  foo & set_color (const string & a_color)
  {
    color_ = a_color;
    return *this;
  }
  foo  () : color_ ("white")
  {
  }
  foo (const string & a_color) : color_ (a_color)
  {
  }
  ~foo () 
  {
  }
  void print () const
  {
    cout << "foo @ " << this << " : {color=\"" << color_ << "\"}"<< endl;
  }
};

// define a predicate for the 'foo' class :
struct foo_is_blue_predicate : public datatools::utils::i_predicate<foo>
{
  bool operator () (const foo & a_foo) const
  {
    return (a_foo.get_color () == "blue");
  }
};

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'i_predicate' interface with a STL collection of handle<>'s..." << endl; 
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_) 
	{
          string arg = argv_[iarg];
	  if ((arg == "-d") || (arg == "--debug")) debug = true;
	  iarg++;
	}

      using namespace datatools::utils;
      {
	clog << endl << "Test 1: " << endl;
	foo f0 ("red");
	foo f1 ("green");
	foo f2 ("blue");

	foo_is_blue_predicate is_blue;
	if (is_blue (f0))
	  {
	    clog << "f0 is blue !" << endl;
	  }
	if (is_blue (f1))
	  {
	    clog << "f1 is blue !" << endl;
	  }
	if (is_blue (f2))
	  {
	    clog << "f2 is blue !" << endl;
	  }
	
	typedef datatools::utils::handle<foo> handle_type;
	typedef vector<handle_type> collection_type;
	collection_type foos;
	foos.push_back (handle_type (new foo ("red")));
	foos.push_back (handle_type (new foo ("green")));
	foos.push_back (handle_type (new foo ("blue")));
	foos.push_back (handle_type (new foo ("white")));
	foos.push_back (handle_type (new foo ("pink")));
	foos.push_back (handle_type (new foo ("yellow")));
	  
	clog << "Foos : " << endl;
	for (collection_type::const_iterator i = foos.begin ();
	     i != foos.end ();
	     i++)
	  {
	    if (i->has_data ()) i->get ().print ();
	  }
	  
	// a predicate to check if a foo is `blue' colored :
	foo_is_blue_predicate BP;

	// the `handle_predicate' is a wrapper for the `BP' predicate 
	// that enable to apply the BP predicate through the collection iterators :
	datatools::utils::handle_predicate<foo> HP (BP);
	collection_type::const_iterator found = find_if (foos.begin (), foos.end (), HP);
	if (found != foos.end ())
	  {
	    clog << "Found handle with a blue foo !" << endl;
	    found->get().print ();
	  }
	  
      }
     
    }
  catch (exception & x)
    { 
      clog << "error: " << x.what () << endl; 
      error_code =  EXIT_FAILURE;
    }
  catch (...) 
    { 
      clog << "error: " << "unexpected error!" << endl;  
      error_code = EXIT_FAILURE; 
    } 
  return error_code;
} 

// end of test_predicate_1.cxx 

