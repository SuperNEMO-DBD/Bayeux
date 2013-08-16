/* writer.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-01
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *
 *   A Boost archive to ROOT file writer
 *
 * History:
 *
 */
#ifndef BRIO_WRITER_H_
#define BRIO_WRITER_H_ 1
// Standard Library
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

// Third Party
// - Boost
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

// - datatools
#include <datatools/eos/portable_oarchive.hpp>
#include <datatools/logger.h>
#include <datatools/exception.h>

// - ROOT
#include <TTree.h>

// This Project
#include <brio/detail/base_io.h>

namespace brio {
//! \brief The brio generic writer class
class writer : public detail::base_io {
 public:
  //! Default constructor
  writer();

  //! Constructor
  writer(const std::string& a_filename,
         datatools::logger::priority p_ = datatools::logger::PRIO_FATAL);

  //! Constructor
  writer(const std::string& a_filename,
         const std::string& a_format_str,
         datatools::logger::priority p_ = datatools::logger::PRIO_FATAL);

  //! Destructor
  virtual ~writer();

  //! Lock the writer
  void lock();

  //! Unlock the writer
  void unlock();

  //! Check if the writer is locked
  bool is_locked() const;

  //! Check if mixed types are allowed in any given store
  bool is_allow_mixed_types_in_stores() const;

  //! Check if automatic store is allowed
  bool is_allow_automatic_store() const;

  //! Check if protection against file overwriting exists
  bool is_existing_file_protected() const;

  /** Allow to serialize different types of objects within stores.
   *  Usually, each store is setup in order to accept only one
   *  kind of serializable object (potato) , in such a way one cannot mix
   *  several object's types (ex: potato and carot) within the same branch
   *  Default at construction is false.
   */
  void set_allow_mixed_types_in_stores(bool a_new_value = true);

  /** Enables the use of some automatic store (with
   *  standard label == store_info::AUTOMATIC_STORE_LABEL)
   *  within the writer.
   *  Default at construction is true.
   */
  void set_allow_automatic_store(bool a_new_value = true);

  //! Set the protection against file overwriting
  void set_existing_file_protected(bool a_new_value = true);

  /** Add a new store with label 'label_'
   *  to store objects with a dedicated serialization tag 'serial_tag_'
   */
  int add_store(const std::string& a_label,
                const std::string& a_serial_tag,
                size_t a_buffer_size = 256000);

  /** Add a new store with label 'label_'
   *  to store objects with a dedicated serialization tag 'serial_tag_'
   */
  int add_store(const std::string& a_label,
                size_t a_buffer_size = 256000);

  /** Add a new store with label 'label_'
   *  to store objects with arbitrary serialization tags
   */
  int add_mixed_store(const std::string& a_label,
                      size_t a_buffer_size = store_info::DEFAULT_STORE_BUFFER_SIZE);

  // Store template method
  template <typename T>
  int store(const T& a_data, const std::string& a_label = "");

  //! Smart print
  virtual void tree_dump(std::ostream& a_out = std::clog,
                         const std::string& a_title = "",
                         const std::string& a_indent = "",
                         bool a_inherit = false) const;

  //! Print
  void print_info(std::ostream& a_out = std::clog) const;

 protected:
  void _set_default();

  store_info* _add_store(const std::string& a_label,
                         const std::string& a_serial_tag,
                         size_t a_buffer_size);

  template <typename T>
  int _at_store(const T& a_data, store_info *a_store_info);
 
  virtual void _at_open(const std::string& a_filename);

 private:
  void __only_if_unlocked(const std::string& a_where) const;
  void __only_if_locked(const std::string& a_where) const;

 private:
  bool _locked_; /// Flag to lock the writer storage structure
  bool _allow_mixed_types_in_stores_; /// Flag to allow stores with mixed types
  bool _allow_automatic_store_;       /// Flag to allow an default automatic store
  bool _existing_file_protected_;     /// Flag to protect existing output data file
  store_info *_automatic_store_;     /// A handle to the automatic store (if any)
};
} // end of namespace brio

#include <brio/writer-inl.h>

#endif // BRIO_WRITER_H_

