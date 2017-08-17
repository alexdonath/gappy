/**
 *  last changed Time-stamp: <2017-08-16 19:06:03 adonath>
 *
 *  Author: Alexander Donath, a <dot> donath <at> zfmk <dot> de
 */

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "files.h"
#include "utils.h"

//OUT
///#include <Seq/containers>

using namespace std;
using namespace files;
using namespace utils;
//OUT
///using namespace bpp;

void files::scanMaf (std::vector<unsigned int>* positions, 
		     const std::string& mafFile)
{
  if (mafFile == "") 
    return;

  ifstream input;
  input.open(mafFile.c_str());
  if (!input) 
    return;

  input.unsetf(ios_base::skipws);
  
  //size_t line_counter = 0;
  size_t chars_read = 0;
  char currChar;
  char lastChar = '\n';
  positions->clear();
  bool paragraph = false;
  bool skip = false;
  //bool seq = false;
  
  // Check if file is still readable
  while(input.good()) {
    
    input >> currChar;    
    if(input)     
      ++chars_read;       

    //cout << "last " << lastChar << " curr " << currChar << endl;

    if (paragraph)
      {
	if (lastChar == '\n' && (currChar == 'i' || currChar == 'e' || currChar == 'q'))
	  {
	    skip = true;
	    lastChar = currChar;
	  }
	else if (lastChar == '\n' && currChar == 's')
	  {
	    //seq = true;
	    lastChar = currChar;
	    skip = false;
	  }
	else if (lastChar == '\n' && (currChar == '\n' || currChar == ' '))
	  {
	    paragraph = false;
	    skip = false;
	    lastChar = currChar;
	  }
	else if (skip)
	  {
	    lastChar = currChar;
	  }
	else
	  lastChar = currChar;
      }
    else
      {
	if (lastChar == '\n' && currChar == 'a')
	  {
	    paragraph = true;
	    skip = false;
	    lastChar = currChar;
	    positions->push_back(chars_read-1);
	  }
	else if (lastChar == '\n' && currChar == '#')
	  {
	    lastChar = currChar;
	    skip = true;
	  }
	else if (lastChar == '\n' && currChar == '\n') 
	  {
	    lastChar = currChar;
	    paragraph = false;
	    skip = false;
	  }
	else if (skip) 
	  {
	    lastChar = currChar;
	  }
	else 
	  lastChar = currChar;
      }
  }
 
  input.close();
}


void files::getMafAlignments (std::vector<maf_t>* maf, 
			      const unsigned int pos, 
			      const std::string& mafFile)
{

  if (mafFile == "") 
    return;

  ifstream input;
  input.open(mafFile.c_str());
  if (!input) 
    return;
  
  std::string line;
  std::string prefix = "s ";
  std::string newPrefix = "a ";

  std::vector<std::string> tokens;

  input.clear();
  input.seekg(pos, ios::beg);
  getline(input, line);

  while (getline(input, line))

    {
      if (!line.compare(0, prefix.size(), prefix))
	{
	  tokens.clear();
	  utils::tokenize(line, tokens);
	      
	  if (tokens.size() != 7)
	    utils::errmsg("Problem with format of MAF file.",__LINE__);

	  maf_t tmp;
	  tmp.src = tokens[1];
	  tmp.start = atoi(tokens[2].c_str());
	  tmp.length = atoi(tokens[3].c_str());
	  tmp.strand = tokens[4][0];
	  tmp.srcsize = atoi(tokens[5].c_str());
	  tmp.text = tokens[6];
	  maf->push_back(tmp);
	}
      else if (!line.compare(0, newPrefix.size(), newPrefix)) 
	{
	  break;
	}
      else
	{
	  continue;
	}
    }
 
  input.close();
}


void files::mafToFastaAln (const std::vector<maf_t>& maf, 
			    files::Fasta * fastaFile)
{
  //add sequence to sequence container
  unsigned int i = 0;
  while (i < maf.size())
    {
      fastaFile->append_entry(maf[i].src, maf[i].text);
      ++i;
    }
}



void files::writeStat (std::string prefix, 
		       const std::map<unsigned int, short>& statistic,
		       const unsigned int minSize, const unsigned maxSize)
{
  
  prefix.append(".gap_counts.txt");
  std::ofstream statfile (prefix.c_str(), ios::out);
  if (statfile.is_open())
    {
      statfile << "#indel_size\tcount" << endl;
      for (unsigned int i = minSize ; i <= maxSize ; ++i)
	{
	  if (statistic.count(i) > 0)
	    {
	      statfile  << i << "\t" << statistic.find(i)->second << endl;
	    }
	}
      statfile.close();
    }
  else 
    utils::errmsg("Unable to write statistics file.",__LINE__);

}


void files::writeNexus (const std::vector< std::vector<char> > & scores,
			const std::vector< std::string > & names,
			const std::string unknownchar,unsigned int alnNoOfSeqs, 
			std::string prefix, std::string notes)
{
  std::ostringstream message;

  message << "#NEXUS" << endl << endl
	  << "[!" << notes << "]" << endl
	  << "BEGIN TAXA;" << endl
	  << "\tDIMENSIONS ntax=" << alnNoOfSeqs << ";" << endl
	  << "\tTAXLABELS ";

  for (unsigned int i = 0 ; i < alnNoOfSeqs ; ++i)
    message << " " << names[i];

  message << ";" << endl
	  << "END;" << endl << endl
	  << "BEGIN CHARACTERS;" << endl
	  << "\tDIMENSIONS nchar=" << scores[0].size() << ";" << endl
	  << "\tFORMAT" << endl
	  << "\t\tDATATYPE=STANDARD" << endl
	  << "\t\tMISSING=" << unknownchar[0] << endl
	  << "\t\tSYMBOLS=\"01\"" << endl
	  << "\t\tLABELS=LEFT;" << endl
	  << "\tMATRIX" << endl;

  for (unsigned int i = 0 ; i < alnNoOfSeqs ; ++i)
    {
      message << "\t" << names[i] << "\t";
      for (unsigned int j = 0 ; j < scores[i].size() ; ++j)
	{
	  message << scores[i][j];
	}
      message << endl;
    }
  
  message << "\t;" << endl
	  << "END;" << endl;

  prefix.append(".NEXUS");
  std::ofstream nexusfile (prefix.c_str(), ios::out);
  if (nexusfile.is_open())
    {
      nexusfile << message.str();
      message.str("");
      nexusfile.close();
    }
  else 
    utils::errmsg("Unable to write NEXUS file.",__LINE__);
}


void files::writeFasta (const std::vector< std::vector<char> > & scores,
			const std::vector< std::string > & names,
			unsigned int alnNoOfSeqs, std::string prefix)
{
  prefix.append(".01.FAS");
  std::ofstream fastafile (prefix.c_str(), ios::out);
  if (fastafile.is_open())
    {
      for (unsigned int i = 0 ; i < alnNoOfSeqs ; ++i)
	{
	  fastafile << ">" << names[i] << endl;
	  for (unsigned int j = 0 ; j < scores[i].size() ; ++j)
	    {
	      fastafile << scores[i][j];
	    }
	  fastafile << endl;
	}
    }
  else
    utils::errmsg("Unable to write FASTA file.",__LINE__);
}


void files::writePhylip (const std::vector< std::vector<char> > &scores,
			 const std::vector< std::string > & names,
			 unsigned int alnNoOfSeqs, std::string prefix)
{
  prefix.append(".01.phy");
  std::ofstream phyfile (prefix.c_str(), ios::out);
  if (phyfile.is_open())
    {
      
      std::vector< std::vector<std::string> > seqs(alnNoOfSeqs);
      for(unsigned int i = 0; i < alnNoOfSeqs; ++i)
	{
	  std::string tmp(scores[i].begin(), scores[i].end());
	  seqs[i] = utils::split (tmp, 60);
	}

      phyfile << "     " << alnNoOfSeqs << "    " << scores[0].size() << endl;
     
      //Write first block:
      for(unsigned int i = 0; i < alnNoOfSeqs; ++i)
	{
	  phyfile << names[i].substr(0,10) << " " << seqs[i][0] << endl;
	}
      phyfile << endl;
      //Write other blocks:
      for(unsigned int i = 1; i < seqs[0].size(); ++i)
	{
	  for(unsigned int j = 0; j < alnNoOfSeqs; ++j)
	    {
	      phyfile << "           " << seqs[j][i] << endl;
	    }
	  phyfile << endl;
	  
	}
    }
  else
    utils::errmsg("Unable to write PHYLIP file.",__LINE__);
}


/************************************************************************
 ** Fasta
 ***********************************************************************/

// constructors / destructors
files::Fasta::Fasta() throw(): _length(0) {
}

Fasta::~Fasta() throw() {
}

// methods

// string
const std::string Fasta::str(const size_t indent) const {
  std::ostringstream stream;
  std::string ind(indent, ' ');

  stream << ind << "length:" << _length << std::endl;
  stream << ind << "entries:" << std::endl;
  for (size_t i = 0; i < _entries.size(); ++i) {
    stream << ind << "  entry " << i << ":" << std::endl;
    stream << ind << "    tag: " << tag(i) << std::endl;
    stream << ind << "    id:  >" << _entries[i].id << "<" << std::endl;
    stream << ind << "    seq: ";
    if (seq(i).size() > 20) {
      stream << seq(i, 0, 10) << "..."
	     << seq(i, seq(i).size() - 10);
    }
    else {
      stream << seq(i);
    }
    stream << " (" << seq(i).size() << ")" << std::endl;
  }
  return(stream.str());
}

// infos 
const bool Fasta::is_fasta_stream(std::istream& in) {
  if (!in) {
    std::ostringstream message;
    message << "Could not find stream.";
    utils::errmsg (message.str(), __LINE__);
  }

  while (!in.eof()) {
    std::string line = "";
    getline(in, line, '\n');
    utils::uncomment(&line);
    utils::trim(&line);
    if (line == "") {
      continue;
    }
    if (line[0] == '>') {
      return(true);
    }
    else {
      return(false);
    }
  }
  return(false);
}

const bool Fasta::is_fasta_file(const std::string& file_name) {
  // open file
  std::ifstream file(file_name.c_str());
  if (!file) {
    std::ostringstream message;
    message << "Could not open fasta file '" << file_name << "'";
    utils::errmsg (message.str(), __LINE__);
  }

  bool result = is_fasta_stream(file);

  file.close();
  return(result);
}

// algorithms 

void Fasta::append_entry(const std::string& tag,
			 const std::string& seq) {
  // check tag
  if (tag == "") {
    utils::errmsg ("Empty tag is not allowed.", __LINE__);
  }
  size_t pos = tag.find_first_of(" \t");
  std::string id = tag.substr(0, pos);
  index_t::const_iterator iter = _index.find(id);
  if (iter != _index.end()) {
    std::ostringstream message;
    message << "Entry with first tag word '" << id
	    << "' does already exists (Tags till first space "
	    << "have to be unique )";
    utils::errmsg (message.str(), __LINE__);
  }
  // append
  entry_t entry = {tag, id, seq};

  if (size() == 0) {
    _length = seq.size();
  }
  else if (length() != seq.size()) {
    std::ostringstream message;
    message << "Sequences not aligned?";
    utils::errmsg (message.str(), 2*(_entries.size()+1));
  }

  _entries.push_back(entry);
}

void Fasta::append_stream(std::istream& in) {
  // check stream
  if (!in) {
    std::ostringstream message;
    message << "Could not find stream";
    utils::errmsg (message.str(), __LINE__);
  }

  size_t line_counter = 0;
  size_t seq_counter = 0;

  std::string tag = "";
  std::string seq = "";

  while (!in.eof()) {
    std::string line = "";
    getline(in, line, '\n');
    ++line_counter;
    utils::uncomment(&line);
    utils::trim(&line);
    if (line == "") {
      continue;
    }
    if (line[0] == '>') {
      // new entry
      if (tag != "") {
	// previous entry exists
	if (seq == "") {
	  // no sequence for last tag found
	  std::ostringstream message;
	  message << "Tag '" << tag << "' at line " << line_counter-1
		  << " has no sequence";
	  utils::errmsg (message.str(), line_counter-1);
	}
	// append old entry
	++seq_counter;
	append_entry(tag, seq);
	seq = "";
      }
      tag = line.substr(1);
    }
    else {
      if (tag == "") {
	// no tag for sequence
	std::ostringstream message;
	message << "Sequence " << seq_counter << " has no tag";
	utils::errmsg (message.str(), line_counter-1);
      }
      seq += line;
    }
  }
  // append last entry
  append_entry(tag, seq);
}

void Fasta::append_file(const std::string& file_name) {

  // open file
  std::ifstream file(file_name.c_str());
  if (!file) {
    std::ostringstream message;
    message << "Could not open fasta file '" << file_name << "'";
    utils::errmsg (message.str(), __LINE__);
  }
  append_stream(file);

  file.close();
}

void Fasta::clear() {
  _entries.clear();
  _index.clear();
}

void Fasta::write_file(const std::string& file_name) {
  // open file
  std::ofstream file(file_name.c_str());
  if (!file) {
    std::ostringstream message;
    message << "Could not create file '" << file_name << "'";
    utils::errmsg (message.str(), __LINE__);
  }

  for (size_t i = 0; i < _entries.size(); ++i) {
    //if (i > 0) {
    //  file << std::endl;
    //}
    file << ">" << tag(i) << std::endl;
    if (__FASTA_WIDTH > 0) {
      size_t pos = 0;
      while (pos < seq(i).size()) {
	file << seq(i, pos, __FASTA_WIDTH) << std::endl;
	pos += __FASTA_WIDTH;
      }
    }
    else {
      file << seq(i) << std::endl;
    }
  }
  file.close();
}
