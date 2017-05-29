/**
 *  last changed Time-stamp: <2016-10-21 16:51:37 adonath>
 *
 *  Author: Wolfgang Otto
 *  Modified by: Alexander Donath, a <dot> donath <at> zfmk <dot> de
 */

#ifndef ___WOL_OPTIONS_HPP___
#define ___WOL_OPTIONS_HPP___

#include <string>
#include <map>
#include <vector>
#include <list>
#include <sstream>

//#include "error.hpp"

namespace wol {

  /************************************************************************
   ** Options
   ***********************************************************************/

  class Options {

    ///////////////////////////////////////////////////////////////////////
    // intern types / constants
    ///////////////////////////////////////////////////////////////////////

  private:

    static const size_t message_indent = 30;
    static const size_t message_columns = 75;
    static const size_t message_defaults = true;

    typedef std::map< std::string, std::string > options_t;
    typedef std::list< std::string > arguments_t;
    typedef std::vector< std::string > unparsed_arguments_t;
    typedef std::list< std::string > error_messages_t;

    enum type_t {STRING, POINTER, OTHER};

    ///////////////////////////////////////////////////////////////////////
    // definitions
    ///////////////////////////////////////////////////////////////////////

  public:

    struct definition_t {
      std::string long_name;
      std::string short_name;
      std::string default_value;
      std::string description;
    };

    ///////////////////////////////////////////////////////////////////////
    // variables
    ///////////////////////////////////////////////////////////////////////

  private:

    options_t                _options;
    unparsed_arguments_t     _unparsed_arguments;
    std::string              _version_number;
    std::string              _version_message;
    std::string              _help_message;
    mutable error_messages_t _error_messages;
    std::string              _program_call;

    ///////////////////////////////////////////////////////////////////////
    // constructors / destructors
    ///////////////////////////////////////////////////////////////////////

  public:

    Options(const std::string& version, const std::string& description, 
            const definition_t definition[], const int argc, 
            const char* const argv[], const std::string& remote_file = "");

    ///////////////////////////////////////////////////////////////////////
    // operators
    ///////////////////////////////////////////////////////////////////////

  public:

    inline const std::string operator [] (const std::string& key) const;
    inline const std::string operator [] (const size_t index) const;

    ///////////////////////////////////////////////////////////////////////
    // intern functions
    ///////////////////////////////////////////////////////////////////////

  private:

    inline static void uncomment(std::string* data,
                                 const std::string& sign = "#");
    inline static void trim(std::string* data);

    const bool is_end_definition(const definition_t& definition) const;
    const size_t find_index
    (const definition_t definition[], const std::string& name, 
     const size_t counter, const std::string& remote_file = "") const;
    
    void check_definition(const definition_t definition[]) const;
    void create_version_message(std::string program_name,
                                const std::string& version);
    void create_help_message(const definition_t definition[],
                             const std::string& description,
                             std::string program_name);
    void set_defaults(const definition_t definition[]);
    void parse_file(const definition_t definition[],
                    const std::string& remote);
    void parse_named(const definition_t definition[], 
                     arguments_t* arguments);
    void parse_unnamed(const definition_t definition[], 
                       arguments_t* arguments);
    void check_completeness(const definition_t definition[]) const;

    template <typename T> const type_t type_test(const T& value) const;
    template <typename T> const type_t type_test(const T* pointer) const;
    inline const type_t type_test(const std::string& string) const;

    ///////////////////////////////////////////////////////////////////////
    // methods
    ///////////////////////////////////////////////////////////////////////

  public:

    // string /////////////////////////////////////////////////////////////
    const std::string str(const size_t indent = 0) const;

    // infos //////////////////////////////////////////////////////////////
    inline const std::string& version_number() const;
    inline const std::string& version_message() const;
    inline const std::string& help_message() const;
    inline const bool error() const;
    inline const std::string error_message() const;
    inline const std::string& program_call() const;

    template <typename T> inline
    const T value(const std::string& key) const;
    inline const std::string value(const std::string& key) const;
    template <typename T> inline
    void set_value(T* variable, const std::string& key) const;

    inline const size_t arguments_size() const;
    template <typename T> inline
    const T argument(const size_t index) const;
    inline const std::string argument(const size_t index) const;
    template <typename T> inline
    void set_argument(T* variable, const size_t index) const;
  };

  /************************************************************************
   ** global
   ***********************************************************************/

  /////////////////////////////////////////////////////////////////////////
  // operators
  /////////////////////////////////////////////////////////////////////////

  inline std::ostream& operator << (std::ostream& stream, 
                                    const Options& options);

  /*#######################################################################
    ## definitions
    #####################################################################*/

  
  /************************************************************************
   ** Options
   ***********************************************************************/

  /////////////////////////////////////////////////////////////////////////
  // operators
  /////////////////////////////////////////////////////////////////////////

  inline const std::string Options::operator [] 
  (const std::string& key) const {
    options_t::const_iterator pos = _options.find(key);
#ifdef DEBUG
    if (pos == _options.end()) {
      std::ostringstream message;
      message << "no option with key '" << key << "' exists";
      //      throw(Error(__FILE__, __LINE__, message.str()));
    }
#endif

    std::string value_string = pos->second;
    // test for escape
    if (value_string[0] == '\\') {
      value_string = value_string.substr(1);
    }

    return(value_string);
  }

  inline const std::string Options::operator [] 
  (const size_t index) const {
#ifdef DEBUG
    if (index >= _unparsed_arguments.size()) {
      std::ostringstream message;
      message << "no argument with index " << index << " exists";
      //      throw(Error(__FILE__, __LINE__, message.str()));
    }
#endif

    std::string value_string = _unparsed_arguments[index];
    // test for escape
    if (value_string[0] == '\\') {
      value_string = value_string.substr(1);
    }

    return(value_string);
  }

  /////////////////////////////////////////////////////////////////////////
  // intern functions
  /////////////////////////////////////////////////////////////////////////

  inline void Options::uncomment(std::string* data,
                                 const std::string& sign) {
    size_t esc = 0;
    for (size_t i = 0; i < data->size(); ++i) {
      if ((*data)[i] == '\\') {
        ++esc;
      }
      else {
        bool match = true;
        for (size_t j = 0; j < sign.size(); ++j) {
          if ((*data)[i + j] != sign[j]) {
            match = false;
            break;
          }
        }
        if (match && esc % 2 == 0) {
          // comment start
          data->erase(i);
          break;
        }
        esc = 0;
      }
    }
  }

  inline void Options::trim(std::string* data) {
    if (*data != "") {
      size_t begin = data->find_first_not_of(" \t\n");
      size_t end = data->find_last_not_of(" \t\n");
      if (begin != std::string::npos) {
        *data = data->substr(begin, end - begin + 1);
      }
      else {
        *data = "";
      }
    }
  }

  inline const bool Options::is_end_definition
  (const definition_t& definition) const {
    const std::string& long_name = definition.long_name;
    const std::string& short_name = definition.short_name;
    const std::string& default_value = definition.default_value;
    const std::string& description = definition.description;
    if (long_name == "" && short_name == "" 
        && default_value == "" && description == "") {
      return(true);
    }
    return(false);
  }

  template <typename T> inline
  const Options::type_t Options::type_test(const T& value) const {
    return(OTHER);
  }

  template <typename T> inline
  const Options::type_t Options::type_test(const T* pointer) const {
    return(POINTER);
  }

  inline const Options::type_t Options::type_test
  (const std::string& string) const {
    return(STRING);
  }

  /////////////////////////////////////////////////////////////////////////
  // methods
  /////////////////////////////////////////////////////////////////////////

  // infos ////////////////////////////////////////////////////////////////

  inline const std::string& Options::version_number() const {
    return(_version_number);
  }

  inline const std::string& Options::version_message() const {
    return(_version_message);
  }

  inline const std::string& Options::help_message() const {
    return(_help_message);
  }

  inline const bool Options::error() const {
    return(_error_messages.size() != 0);
  }

  inline const std::string Options::error_message() const {
    std::ostringstream stream;
    if (_error_messages.size() != 0) {
      error_messages_t::const_iterator iter = _error_messages.begin();
      while (iter != _error_messages.end()) {
        stream << "Error: " << *iter << "!" << std::endl;
        ++iter;
      }
      stream << std::endl;
    }
    stream << this->help_message();
    return(stream.str());
  }

  inline const std::string& Options::program_call() const {
    return(_program_call);
  }

  template <typename T> inline
  const T Options::value(const std::string& key) const {
    options_t::const_iterator pos = _options.find(key);
#ifdef DEBUG
    if (pos == _options.end()) {
      std::ostringstream message;
      message << "no option with key '" << key << "' exists";
      //      throw(Error(__FILE__, __LINE__, message.str()));
    }
#endif

    std::string value_string = pos->second;
    // test for escape
    if (value_string.size() > 0 && value_string[0] == '\\') {
      value_string = value_string.substr(1);
    }

    T value;
    type_t value_type = type_test(value);

    if (value_type == STRING) {
      // string have to be treated extra because lexical casts have
      // problems with spaces inside a string
      std::string* value_string_pointer = &value_string;
      T* value_pointer = reinterpret_cast<T*>(value_string_pointer);
      value = *value_pointer;
    }
#ifdef DEBUG
    else if (value_type == POINTER) {
      std::ostringstream message;
      message << "lexical cast for pointers is not defined";
      //      throw(Error(__FILE__, __LINE__, message.str()));
    }
#endif
    else {
#ifdef DEBUG
      // test for empty string
      if (value_string == "") {
        std::ostringstream message;
        message << "lexical cast for option '" << key 
               << "' with empty value is not defined";
	//        throw(Error(__FILE__, __LINE__, message.str()));
      }
#endif

      std::istringstream stream(value_string);
      stream >> std::noskipws >> value;

#ifdef DEBUG
      if (!stream.eof()) {
        std::ostringstream message;
        message << "bad lexical cast for option '" << key 
               << "' with value '" << value_string << "' occurred";
	//        throw(Error(__FILE__, __LINE__, message.str()));
      }
#endif
    }

    return(value);
  }

  inline const std::string Options::value(const std::string& key) const {
    options_t::const_iterator pos = _options.find(key);
#ifdef DEBUG
    if (pos == _options.end()) {
      std::ostringstream message;
      message << "no option with key '" << key << "' exists";
      //      throw(Error(__FILE__, __LINE__, message.str()));
    }
#endif

    std::string value_string = pos->second;
    // test for escape
    if (value_string[0] == '\\') {
      value_string = value_string.substr(1);
    }

    return(value_string);
  }


  template <typename T> inline
  void Options::set_value(T* variable, const std::string& key) const {
    *variable = value< T >(key);
  }


  inline const size_t Options::arguments_size() const {
    return(_unparsed_arguments.size());
  }


  template <typename T> inline
  const T Options::argument(const size_t index) const {
#ifdef DEBUG
    if (index >= _unparsed_arguments.size()) {
      std::ostringstream message;
      message << "no argument with index " << index << " exists";
      //      throw(Error(__FILE__, __LINE__, message.str()));
    }
#endif

    std::string value_string = _unparsed_arguments[index];
    // test for escape
    if (value_string.size() > 0 && value_string[0] == '\\') {
      value_string = value_string.substr(1);
    }

    T value;
    type_t value_type = type_test(value);

    if (value_type == STRING) {
      // string have to be treated extra because lexical casts have
      // problems with spaces inside a string
      std::string* value_string_pointer = &value_string;
      T* value_pointer = reinterpret_cast<T*>(value_string_pointer);
      value = *value_pointer;
    }
#ifdef DEBUG
    else if (value_type == POINTER) {
      std::ostringstream message;
      message << "lexical cast for pointers is not defined";
      //      throw(Error(__FILE__, __LINE__, message.str()));
    }
#endif
    else {
#ifdef DEBUG
      // test for empty string
      if (value_string == "") {
        std::ostringstream message;
        message << "lexical cast for argument '" << index 
               << "' with empty value is not defined";
	//        throw(Error(__FILE__, __LINE__, message.str()));
      }
#endif

      std::istringstream stream(value_string);
      stream >> std::noskipws >> value;

#ifdef DEBUG
      if (!stream.eof()) {
        std::ostringstream message;
        message << "bad lexical cast for argument " << index 
               << "' with value '" << value_string << "' occurred";
	//        throw(Error(__FILE__, __LINE__, message.str()));
      }
#endif
    }

    return(value);
  }

  inline const std::string Options::argument(const size_t index) const {
#ifdef DEBUG
    if (index >= _unparsed_arguments.size()) {
      std::ostringstream message;
      message << "no argument with index " << index << " exists";
      //      throw(Error(__FILE__, __LINE__, message.str()));
    }
#endif

    std::string value_string = _unparsed_arguments[index];
    // test for escape
    if (value_string[0] == '\\') {
      value_string = value_string.substr(1);
    }

    return(value_string);
  }

  template <typename T> inline
  void Options::set_argument(T* variable, const size_t index) const {
    *variable = argument< T >(index);
  }


  /************************************************************************
   ** global
   ***********************************************************************/

  /////////////////////////////////////////////////////////////////////////
  // operators
  /////////////////////////////////////////////////////////////////////////

  inline std::ostream& operator << (std::ostream& stream, 
                                    const Options& options) {
    stream << options.str();
    return(stream);
  }

}

#endif
