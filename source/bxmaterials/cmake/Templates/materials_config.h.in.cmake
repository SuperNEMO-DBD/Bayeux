// -*- mode: c++; -*- 
/* materials_config.h header file 
 */

#ifndef __mat_materials_config_h
#define __mat_materials_config_h 1

/*** brio versioning information ***/

//  MATERIALS_VERSION % 100        is the patch level
//  MATERIALS_VERSION / 100 % 1000 is the minor version
//  MATERIALS_VERSION / 100000     is the major version

#define MATERIALS_VERSION_MAJOR @materials_VERSION_MAJOR@
#define MATERIALS_VERSION_MINOR @materials_VERSION_MINOR@
#define MATERIALS_PATCH_VERSION @materials_PATCH_VERSION@

#define MATERIALS_VERSION @materials_VERSION_NUMERIC@

//  MATERIALS_LIB_VERSION must be defined to be the same as MATERIALS_VERSION
//  but as a *string* in the form "x.y[.z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define MATERIALS_LIB_VERSION "@materials_VERSION_STR@"

/***  materials embedded Boost/Serialization based I/O ***/
#if x0 == x1
#define MATERIALS_WITH_BIO @_PACKNAME___WITH_BIO@
#endif

#endif // __mat_materials_config_h

// end of materials_config.h
