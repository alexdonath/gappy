/**
 *  last changed Time-stamp: <2016-10-21 15:01:46 adonath>
 *
 *  Author: Wolfgang Otto
 *  Modified by: Alexander Donath, a <dot> donath <at> zfmk <dot> de
 *
 */
///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Copyright (C) 2008-2016 by Wolfgang Otto                              //
// All Rights Reserved.                                                  //
//                                                                       //
// Permission to use, copy, modify, and distribute this                  //
// software and its documentation for NON-COMMERCIAL purposes            //
// and without fee is hereby granted provided that this                  //
// copyright notice appears in all copies.                               //
//                                                                       //
// THE AUTHOR AND PUBLISHER MAKE NO REPRESENTATIONS OR                   //
// WARRANTIES ABOUT THE SUITABILITY OF THE SOFTWARE, EITHER              //
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE                  //
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A                  //
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT. THE AUTHORS                  //
// AND PUBLISHER SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED            //
// BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING           //
// THIS SOFTWARE OR ITS DERIVATIVES.                                     //
//                                                                       //
///////////////////////////////////////////////////////////////////////////
                                                             

#include <iostream>
#include <fstream>

#include "options.h"

namespace wol {

  /************************************************************************
   ** Options
   ***********************************************************************/

  /////////////////////////////////////////////////////////////////////////
  // constructors / destructors
  /////////////////////////////////////////////////////////////////////////

  Options::Options (const std::string& version, const std::string& description, 
		    const definition_t definition[],const int argc, 
		    const char* const argv[], const std::string& remote_file): 
    _version_number(version), _version_message(""), _help_message(""), 
    _program_call("") 
  {
    std::string program_name = argv[0];
    _program_call += argv[0];
    arguments_t arguments;
    for (int i = 1; i < argc; ++i) 
      {
	arguments.push_back(argv[i]);
	_program_call += " ";
	_program_call += argv[i];
      }
    
#ifdef DEBUG
    check_definition(definition);
#endif
    create_version_message(program_name, version);
    create_help_message(definition, description, program_name);
    set_defaults(definition);
    parse_file(definition, remote_file);
    parse_named(definition, &arguments);
    parse_unnamed(definition, &arguments);
    check_completeness(definition);
  }
  
  /////////////////////////////////////////////////////////////////////////
  // intern functions
  /////////////////////////////////////////////////////////////////////////

  const size_t Options::find_index (const definition_t definition[], const std::string& name, 
				    const size_t counter, const std::string& remote_file) const 
  {
    std::list< std::string > names;
    size_t definition_index = 0;

    // search options that match name
    size_t index = 0;
    while (!is_end_definition(definition[index])) 
      {
	const std::string& long_name = definition[index].long_name;
	const std::string& short_name = definition[index].short_name;

	if (name == short_name || name == long_name) 
	  {
	    // exact match (there can be no other)
	    names.clear();
	    if (name == short_name) 
	      names.push_back(short_name);
	    else
	      names.push_back(long_name);

	    definition_index = index;
	    break;
	  }
	else if (name == long_name.substr(0, name.size())) 
	  {
	    // name matchs prefix of option long name
	    names.push_back(long_name);
	    definition_index = index;
	  }
	// (since short names consists of a single char they always 
	// match exact or they don't match so there is no need for check)

	++index;
      }

    // test if argument exists exactly one time
    if (names.size() == 0) 
      {
	// no definition for argument
	std::ostringstream stream;
	if (remote_file != "") 
	  {
	    stream << "Unkown option '" << name << "' in remote file '" 
		   << remote_file << "', line " << counter;
	  }
	else 
	  {
	    stream << "Unkown named command line argument '" << name 
		   << "' at position " << counter;
	  }
	const std::string message = stream.str();
	_error_messages.push_back(message);

	return(index);
      }
    else if (names.size() > 1) 
      {
	// ambiguous argument
	std::ostringstream stream;
	if (remote_file != "") 
	  stream << "Argument '" << name << "' in remote file '" << remote_file 
		 << "', line " << counter << ", is ambiguous ('" << *names.begin();
	else 
	  stream << "Argument '" << name << "' at position " << counter 
		 << " is ambiguous ('" << *names.begin();

	std::list< std::string >::const_iterator i = ++names.begin();

	while (i != --names.end()) 
	  {
	    stream << "', '" << *i;
	    ++i;
	  }
	stream << "' and '" << *(--names.end()) << "')";
	const std::string message = stream.str();
	_error_messages.push_back(message);

	return(index);
      }
    else 
      {
	return(definition_index);
      }
  }

  void Options::check_definition(const definition_t definition[]) const 
  {
    std::map <std::string, size_t> used_names;

    size_t index = 0;
    bool unnamed_default = false;
    size_t unnamed_default_index = 0;

    while (!is_end_definition(definition[index])) 
      {
	const std::string& long_name = definition[index].long_name;
	const std::string& short_name = definition[index].short_name;
	const std::string& default_value = definition[index].default_value;
	const std::string& description = definition[index].description;
      
	if (long_name == "" && short_name == "" && description != "") 
	  {
	    // this definition is a header
	    if (default_value != "") 
	      {
		// headers can't have a default value
		std::ostringstream message;
		message << "[ERROR] in file " << __FILE__ << ":" << __LINE__ 
			<< " error: syntax error in option definition " << (index + 1) 
			<< ": headers can't have a default value";
		throw(message.str());
	      }
	  }
	else if (long_name != "" || short_name != "") 
	  {
	    // this definition is an option
	    if (short_name == "NN") 
	      {
		// this definition is an unnamed option
		if (long_name == "") 
		  {
		    if (default_value != "" || description == "") 
		      {
			// unnamed options need a long name as key
			std::ostringstream message;
			message << "[ERROR] in file " << __FILE__ << ":" << __LINE__ 
				<< " error: syntax error in option definition " 
				<< (index + 1) << ": unnamed options need" 
				<< " a long name as key";
			throw(message.str());
		      }
		  }
		else if (description != "") 
		  {
		    // unnamed options can't have a description
		    std::ostringstream message;
		    message << "[ERROR] in file " << __FILE__ << ":" << __LINE__ 
			    << " error: syntax error in option definition " << (index + 1) 
			    << ": unnamed options can't have a description" ;
		    throw(message.str());
		  }
		else if (unnamed_default && default_value == "REQ") 
		  {
		    // no default value for unnamed option after a
		    // former option has one
		    std::ostringstream message;
		    message << "[ERROR] in file " << __FILE__ << ":" << __LINE__ 
			    << " error: syntax error in option definition " 
			    << (unnamed_default_index + 1) 
			    << ": unnamed options can only have default values"
			    << " at the end" ;
		    throw(message.str());
		  }
		if (long_name != "" && default_value != "REQ") 
		  {
		    // found default value for unnamed option
		    unnamed_default = true;
		    unnamed_default_index = index;
		  }
	      }
	    else if (short_name.size() > 1) 
	      {
		// short names can only consists of a single char
		std::ostringstream message;
		message << "[ERROR] in file " << __FILE__ << ":" << __LINE__ 
			<< " error: syntax error in option definition " << (index + 1) 
			<< ": short names can only consists of a single char" ;
		throw(message.str());
	      }

	    // check for multiple names
	    if (long_name != "") 
	      {
		if (used_names.count(long_name) != 0) 
		  {
		    std::ostringstream message;
		    message << "[ERROR] in file " << __FILE__ << ":" << __LINE__ 
			    << " error: syntax error in option definition " << (index + 1) 
			    << ": options name '" << long_name 
			    << "' is already used in option definition " 
			    << (used_names[long_name] + 1);
		    throw(message.str());
		  }
		used_names[long_name] = index;
	      }
	    if (short_name != "" && short_name != "NN") 
	      {
		if (used_names.count(short_name) != 0) 
		  {
		    std::ostringstream message;
		    message << "[ERROR] in file " << __FILE__ << ":" << __LINE__ 
			    << " error: syntax error in option definition " << (index + 1) 
			    << ": options name '" << short_name 
			    << "' is already used in option definition " 
			    << (used_names[short_name] + 1);
		    throw(message.str());
		  }
		used_names[short_name] = index;
	      }
	  }
	else 
	  {
	    // only default value is set (because of no end definition)
	    std::ostringstream message;
	    message << "[ERROR] in file " << __FILE__ << ":" << __LINE__ 
		    << " error: syntax error in option definition " << (index + 1) 
		    << ": only default value is set";
	    throw(message.str());
	  }
	++index;
      }
  }

  void Options::create_version_message(std::string program_name, const std::string& version) 
  {
    // remove path information
    size_t pos = program_name.find_last_of('/');
    if (pos != std::string::npos) 
      program_name = program_name.substr(pos + 1); 

    std::ostringstream message;
    message << std::endl << program_name << " version " << version
	    << std::endl << "written by Alexander Donath" << std::endl
	    << "(alex@bioinf.uni-leipzig.de)" << std::endl
	    << "Date and time of compilation: " << __DATE__ << " " << __TIME__
	    << std::endl << std::endl;

    _version_message = message.str();
  }

  void Options::create_help_message(const definition_t definition[],
                                    const std::string& program_description,
                                    std::string program_name) 
  {
    // remove path information
    size_t pos = program_name.find_last_of('/');
    if (pos != std::string::npos) 
      program_name = program_name.substr(pos + 1); 
    

    std::ostringstream message;
    message << "Usage: " << program_name << " [OPTIONS]";

    // unnamed options
    size_t index = 0;
    while (!is_end_definition(definition[index])) 
      {
	const std::string& long_name = definition[index].long_name;
	const std::string& short_name = definition[index].short_name;
	const std::string& default_value = definition[index].default_value;
	const std::string& description = definition[index].description;
	if (short_name == "NN") 
	  {
	    if (long_name == "") 
	      message << " " << description;
	    else if (default_value == "REQ") 
	      message << " " << long_name;
	    else 
	      message << " [" << long_name << "]";
	  }
	++index;
      }
    message << std::endl;
      
    // program_description
    size_t length = 0;
    for (size_t i = 0; i < program_description.size(); ++i) 
      {
	if (program_description[i] == '\n' ||
	    (length > message_columns && program_description[i] == ' ')) 
	  {
	    message << std::endl;
	    length = 0;
	  }
	else 
	  {
	    message << program_description[i];
	    ++length;
	  }
      }
    message << std::endl << std::endl;

    // header
    message << "OPTIONS:" << std::endl;
    
    // named options
    index = 0;
    while (!is_end_definition(definition[index])) 
      {
	const std::string& long_name = definition[index].long_name;
	const std::string& short_name = definition[index].short_name;
	const std::string& default_value = definition[index].default_value;
	const std::string& description = definition[index].description;

	if (short_name == "NN" || description == "") 
	  {
	    // unnamed or secret options 
	    ++index;
	    continue;
	  }
	else if (long_name == "" && short_name == "") 
	  {
	    // header
	    size_t length = 0;
	    for (size_t i = 0; i < description.size(); ++i) 
	      {
		if (description[i] == '\n' ||
		    (length > message_columns && description[i] == ' ')) 
		  {
		    message << std::endl;
		    length = 0;
		  }
		else 
		  {
		    message << description[i];
		    ++length;
		  }
	      }
	    message << std::endl;
	  }
	else 
	  {
	    // indent
	    size_t length = 0;
	    message << "  ";
	    length += 2;
	    // short name
	    if (short_name != "") 
	      {
		message << "-" << short_name;
		length += 1 + short_name.size();
		if (long_name != "") 
		  {
		    message << ", ";
		    length += 2;
		  }
	      }
	    // long name
	    if (long_name != "") 
	      {
		message << "--" << long_name;
		length += 2 + long_name.size();
	      }
	    // indent
	    message << "  ";
	    length += 2;
	    while (length < message_indent) 
	      {
		message << ' ';
		++length;
	      }
	    // description
	    for (size_t i = 0; i < description.size(); ++i) 
	      {
		if (description[i] == '\n' ||
		    (length > message_columns && description[i] == ' ')) 
		  {
		    message << std::endl;
		    length = 0;
		    for (size_t j = 0; j < message_indent + 2; ++j) 
		      {
			message << ' ';
			++length;
		      }
		  }
		else 
		  {
		    message << description[i];
		    ++length;
		  }
	      }
	    // default_value
	    if (message_defaults && default_value != "FLAG" && 
		default_value != "REQ" && default_value != "") 
	      {
		std::ostringstream default_stream;
		default_stream << "[" << default_value << "]";
		std::string default_string = default_stream.str();
          
		if (length + 1 + default_string.size() > message_columns) 
		  {
		    message << std::endl;
		    for (size_t j = 0; j < message_indent + 2; ++j) 
		      {
			message << ' ';
		      }
		  }
		else 
		  {
		    message << ' ';
		  }
		message << default_string;
	      }
	    message << std::endl;
	  }
	++index;
      }
    _help_message = message.str();
  }

  void Options::set_defaults(const definition_t definition[]) 
  {
    size_t index = 0;
    while (!is_end_definition(definition[index])) 
      {
	const std::string& long_name = definition[index].long_name;
	const std::string& short_name = definition[index].short_name;
	const std::string& default_value = definition[index].default_value;
      
	if (long_name != "" || (short_name != "" && short_name != "NN")) 
	  {
	    // determine key string (primary: long_name, secondary: short_name)
	    std::string key = long_name;
	    if (long_name == "") 
	      key = short_name;
        
	    if (default_value == "FLAG")
	      _options[key] = '0';
	    else 
	      _options[key] = default_value;
        
	  }
	++index;
      }
  }

  void Options::parse_file(const definition_t definition[],
                           const std::string& remote_file) 
  {
    // check if remote file is used
    if (remote_file == "") 
      return;
    

    // open file
    std::ifstream file(remote_file.c_str());
    if (!file) 
      return;
    

    size_t line_counter = 0;
    std::string line = "";
    std::map< std::string, size_t > used_names;

    // check each line in remote file
    while (!file.eof()) 
      {
	getline(file, line, '\n');
	++line_counter;
	uncomment(&line);
	trim(&line);

	// skip empty lines
	if (line == "") 
	  continue;
      
	// divide line in name and value
	std::string name = line;
	std::string value = "FLAG";
	size_t pos = line.find_first_of(" \t");
	if (pos != std::string::npos) 
	  {
	    name = line.substr(0, pos); 
	    value = line.substr(pos + 1);
	    trim(&name);
	    trim(&value);
	    // test for special values
	    if (value == "REQ" || value == "FLAG") 
	      value = "\\" + value;
	  }

	// test for string
	if ((value[0] == '"' && value[value.size() - 1] == '"') || 
	    (value[0] == '\'' && value[value.size() - 1] == '\'')) 
	  value = value.substr(1, value.size() - 2);
      
	// search options that match name
	size_t index = find_index(definition, name, line_counter, remote_file);

	if (!is_end_definition(definition[index])) 
	  {
	    const std::string& long_name = definition[index].long_name;
	    const std::string& short_name = definition[index].short_name;
	    const std::string& default_value = definition[index].default_value;

	    // determine key string (primary: long_name, secondary: short_name)
	    std::string key = long_name;
	    if (long_name == "") 
	      key = short_name;

	    // test if argument was already used
	    if (used_names.count(key) != 0) 
	      {
		std::ostringstream stream;
		stream << "Option '" << name << "' in remote file '" 
		       << remote_file << "', line " << line_counter 
		       << ", is already used in line " << used_names[key];
		const std::string message = stream.str();
		_error_messages.push_back(message);
		continue;
	      }
	    used_names[key] = line_counter;

	    // test syntax of default values and set options
	    if (default_value == "FLAG") 
	      {
		if (value == "FLAG") 
		  _options[key] = "1";
		else 
		  {
		    std::ostringstream stream;
		    stream << "Option '" << name << "' in remote file '" 
			   << remote_file << "', line " << line_counter 
			   << ", does not take an argument";
		    const std::string message = stream.str();
		    _error_messages.push_back(message);
		    _options[key] = "undef";
		  }
	      }
	    else 
	      {
		if (value == "FLAG") 
		  _options[key] = "";
		else 
		  _options[key] = value;
	      }
	  }
      }
    file.close();
  }


  void Options::parse_named(const definition_t definition[], 
                            arguments_t* arguments) 
  {
    size_t pos_counter = 0;
    std::map< std::string, size_t > used_names;

    arguments_t::iterator i = arguments->begin();
    while (i != arguments->end()) 
      {
	const std::string arg = *i;

	// check for unnamed argument
	if (arg[0] != '-') 
	  {
	    ++i;
	    continue;
	  }

	++pos_counter;

	// argument is named -> remove it from list
	i = arguments->erase(i);

	std::string name = "";
	std::string value = "FLAG";

	// determine argument name
	if (arg.size() > 1) 
	  {
	    if (arg[1] != '-') 
	      {
		// short name
		name = arg[1];
		if (arg.size() > 2) 
		  {
		    if (arg[2] == '=') 
		      {
			// value given per '='
			value = arg.substr(3);
			// test for special values
			if (value == "REQ" || value == "FLAG") 
			  value = "\\" + value;
		      }
		    else 
		      {
			// concatenation of short names (FLAGs) 
			// -> push remaining short names at begin
			std::string corrected_arg = '-' + arg.substr(2);
			i = arguments->insert(i, corrected_arg);
		      }
		  }
	      }
	    else 
	      {
		// long name
		size_t pos = arg.find('=');
		if (pos != std::string::npos) 
		  {
		    // value given per '='
		    name = arg.substr(2, pos - 2);
		    value = arg.substr(pos + 1);

		    // test for special values
		    if (value == "REQ" || value == "FLAG")
		      value = "\\" + value;
		  }
		else 
		  name = arg.substr(2);
	      }
	  }
	if (name == "") 
	  {
	    std::ostringstream stream;
	    stream << "Named command line argument at position " 
		   << pos_counter << " has no name";
	    const std::string message = stream.str();
	    _error_messages.push_back(message);
	    continue;
	  }

	// search options that match name
	size_t index = find_index(definition, name, pos_counter);

	if (!is_end_definition(definition[index])) 
	  {
	    const std::string& long_name = definition[index].long_name;
	    const std::string& short_name = definition[index].short_name;
	    const std::string& default_value = definition[index].default_value;

	    // determine key string (primary: long_name, secondary: short_name)
	    std::string key = long_name;
	    if (long_name == "") 
	      key = short_name;

	    // test if argument was already used
	    if (used_names.count(key) != 0) 
	      {
		std::ostringstream stream;
		stream << "Named command line argument '" << name 
		       << "' at position " << pos_counter 
		       << " is already used at position " << used_names[key];
		const std::string message = stream.str();
		_error_messages.push_back(message);
		continue;
	      }
	    used_names[key] = pos_counter;

	    // test syntax of default_values and set options
	    if (default_value == "FLAG") 
	      {
		if (value == "FLAG") 
		  _options[key] = "1";
		else 
		  {
		    std::ostringstream stream;
		    stream << "Named command line argument '" << name 
			   << "' at position " << pos_counter 
			   << " does not take an argument";
		    const std::string message = stream.str();
		    _error_messages.push_back(message);
		    _options[key] = "undef";
		  }
	      }
	    else 
	      {
		// check if default_value was given in argument by '='
		if (value == "FLAG") 
		  {
		    // take next parameter as default_value
		    if (i == arguments->end()) 
		      {
			std::ostringstream stream;
			stream << "Named command line argument '" << name 
			       << "' at position " << pos_counter 
			       << " takes an argument";
			const std::string message = stream.str();
			_error_messages.push_back(message);
			value = "undef";
		      }
		    else 
		      {
			value = *i;
			if (value[0] == '-') 
			  {
			    std::ostringstream stream;
			    stream << "Named command line argument '" << name 
				   << "' at position " << pos_counter 
				   << " takes an argument";
			    const std::string message = stream.str();
			    _error_messages.push_back(message);
			    value = "undef";
			  }
			else 
			  {
			    // remove value from list
			    i = arguments->erase(i);
			    // test for special values
			    if (value == "REQ" || value == "FLAG") 
			      value = "\\" + value;
			  }
		      }
		  }
		_options[key] = value;
	      }
	  }
      }
  }


  void Options::parse_unnamed(const definition_t definition[], 
                              arguments_t* arguments) 
  {
    size_t index = 0;
    while (!is_end_definition(definition[index])) 
      {
	const std::string& long_name = definition[index].long_name;
	const std::string& short_name = definition[index].short_name;

	if (short_name == "NN" && long_name != "") 
	  {
	    // this definition is an unnamed option
	    if (arguments->size() == 0) 
	      break;
	    else 
	      {
		std::string value = *arguments->begin();
		arguments->pop_front();
		// test for special values
		if (value == "REQ" || value == "FLAG") 
		  value = "\\" + value;
          

		// long name is key string
		_options[long_name] = value;
	      }
	  }
	++index;
      }
      
    // test for unparsed arguments
    if (arguments->size() != 0) 
      {
	_unparsed_arguments.reserve(arguments->size());
	arguments_t::const_iterator iter = arguments->begin();
	while (iter != arguments->end()) 
	  {
	    _unparsed_arguments.push_back(*iter);
	    ++iter;
	  }
      
	// error message for unparsed arguments is turned off
	// std::ostringstream stream;
	// stream << "Unparsed ";
	// if (arguments->size() == 1) {
	//   stream << "argument ('" << *arguments->begin() << "')";
	// }
	// else {
	//   stream << "arguments ('" << *arguments->begin();
	//   arguments_t::const_iterator j = ++arguments->begin();
	//   while (j != --arguments->end()) {
	//     stream << "', '" << *j;
	//     ++j;
	//   }
	//   stream << "' and '" << *(--arguments->end()) << "')";
	// }
	// const std::string message = stream.str();
	// _error_messages.push_back(message);
      }
  }


  void Options::check_completeness(const definition_t definition[]) const 
  {
    std::list< std::string > names;

    size_t index = 0;
    while (!is_end_definition(definition[index])) 
      {
	const std::string& long_name = definition[index].long_name;
	const std::string& short_name = definition[index].short_name;
      
	if (long_name != "" || (short_name != "" && short_name != "NN")) 
	  {
	    // determine key string (primary: long_name, secondary: short_name)
	    std::string key = long_name;
	    if (long_name == "") 
	      key = short_name;
        
	    const std::string& value = _options.find(key)->second;

	    if (value == "REQ" || value == "NN") 
	      // value was not read yet
	      names.push_back(key);
	  }
	++index;
      }

    if (names.size() != 0) 
      {
	std::ostringstream stream;
	stream << "Missing ";
	if (names.size() == 1) 
	  stream << "argument for option ('" << *names.begin() << "')";
	else 
	  {
	    stream << "arguments for options ('" << *names.begin();
	    arguments_t::const_iterator j = ++names.begin();
	    while (j != --names.end()) 
	      {
		stream << "', '" << *j;
		++j;
	      }
	    stream << "' and '" << *(--names.end()) << "')";
	  }
	const std::string message = stream.str();
	_error_messages.push_back(message);
      }
  }

  /////////////////////////////////////////////////////////////////////////
  // methods
  /////////////////////////////////////////////////////////////////////////

  // string

  const std::string Options::str(const size_t indent) const 
  {
    std::ostringstream message;
    std::string ind(indent, ' ');

    message << ind << "version number:" << std::endl;
    message << '>' << this->version_number() << '<' << std::endl;
    message << ind << "version message:" << std::endl;
    message << '>' << this->version_message() << '<' << std::endl;
    message << ind << "help message:" << std::endl;
    message << '>' << this->help_message() << '<' << std::endl;
    message << ind << "program call:" << std::endl;
    message << '>' << this->program_call() << '<' << std::endl;
    message << ind << "values:" << std::endl;

    options_t::const_iterator i = _options.begin();
    while (i != _options.end()) 
      {
	const std::string& key = i->first;
	const std::string& value = this->value(key);
	message << ind << "  " << key << ": ";
	if (i->second == "REQ" || i->second == "NN") 
	  message << "undef";
	else 
	  message << ">" << value << "<";

	message << std::endl;
	++i;
      }
    message << ind << "arguments:" << std::endl;
    for (size_t i = 0; i < _unparsed_arguments.size(); ++i) 
      {
	const std::string& value = this->argument(i);
	message << ind << "  " << i << ": >" << value << "<" << std::endl;
      }
    return(message.str());
  }
}
