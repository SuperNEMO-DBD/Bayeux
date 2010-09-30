// -*- mode: c++; -*- 
/* genbb_help.h general header file 
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 
 *
 */

/*! \mainpage The genbb_help package documentation
 *
 * \section intro_section Introduction
 *
 * The 'genbb_help' library provides a version of the Fortran 
 * GENBB event generator and some interface C++ classes to handle 
 * data output from GENBB.
 *
 *
 * \section contents_section Contents 
 *
 * This section is empty.
 *
 *
 * \section build_section Installation instructions 
 *
 * Configuration, build, installation and setup are done thanks 
 * to the \c './pkgtools.d/pkgtool' script.
 *
 * Usage: 
 *  \verbatim
bash> ./pkgtools.d/pkgtool --help   
\endverbatim
 *
 * Using bash, a typical install sequence is:
 * 
 *  \verbatim
bash> ./pkgtools.d/pkgtool [options...] check [special options...]
bash> ./pkgtools.d/pkgtool [options...] configure  [special options...]
bash> ./pkgtools.d/pkgtool [options...] build 
bash> ./pkgtools.d/pkgtool [options...] build bin 
bash> ./pkgtools.d/pkgtool [options...] build bin_test 
bash> ./pkgtools.d/pkgtool [options...] doc 
bash> ./pkgtools.d/pkgtool [options...] install [special options...]
\endverbatim
 * 
 * Test: 
 *  \verbatim
bash> source genbb_help.sh
bash> genbb_help-config --help
\endverbatim
 * 
 * Setup from your \c '~/.bashrc': 
 *  \verbatim
export GENBB_HELP_ROOT=<installation base directory>
if [ -f ${GENBB_HELP_ROOT}/genbb_help.sh ]; then
   source ${GENBB_HELP_ROOT}/genbb_help.sh
else
   echo "ERROR: No setup for the genbb_help package !" >&2
fi
\endverbatim
 *
 * \section misc_section Miscellaneous informations
 *
 * This section is empty.
 *
 */

#ifndef __genbb_help__genbb_help_h
#define __genbb_help__genbb_help_h 1

#include <genbb_help/genbb_help_version.h>
#include <genbb_help/primary_particle.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/fermi.h>
#include <genbb_help/genbb_writer.h>
#include <genbb_help/rng.h>
#include <genbb_help/genbb_mgr.h>

#endif // __genbb_help__genbb_help_h

// end of genbb_help.h
