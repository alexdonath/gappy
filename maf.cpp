/**
 *  last changed Time-stamp: <2016-10-21 15:02:27 adonath>
 *
 *  Author: Alexander Donath, a <dot> donath <at> zfmk <dot> de
 *
 */
///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Copyright (C) 2010-2016 by Alexander Donath                           //
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

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "maf.h"
#include "utils.h"

#include <Seq/containers>

using namespace std;
using namespace maf;
using namespace utils;
using namespace bpp;

void maf::scanMaf (std::vector<unsigned int>* positions, const std::string& mafFile)
{
  if (mafFile == "") 
    return;

  ifstream input;
  input.open(mafFile.c_str());
  if (!input) 
    return;

  input.unsetf(ios_base::skipws);
  
  size_t line_counter = 0;
  size_t chars_read = 0;
  char currChar;
  char lastChar = '\n';
  positions->clear();
  bool paragraph = false;
  bool skip = false;
  bool seq = false;
  
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
	    seq = true;
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


void maf::getMafAlignments (std::vector<maf_t>* maf, 
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

void maf::mafToFastaAln (const std::vector<maf_t>& maf, 
			 bpp::AlignedSequenceContainer * aln)
{

  //create sequence
  const Alphabet* alpha = aln->getAlphabet();

  //add sequence to sequence container
  unsigned int i = 0;
  while (i < maf.size())
    {
      
      bpp::Sequence * bla = new Sequence(maf[i].src, maf[i].text, alpha);
      aln->addSequence(*bla);
      ++i;
    }
}

void maf::writeStat (std::string prefix, const std::map<unsigned int, short>& statistic,
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

void maf::writeNexus (const std::vector< std::vector<char> > & scores,
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


void maf::writeFasta (const std::vector< std::vector<char> > & scores,
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


void maf::writePhylip (const std::vector< std::vector<char> > &scores,
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

	  seqs[i] = utils::split (tmp, 2);
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
