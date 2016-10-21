/**
 *  last changed Time-stamp: <2016-10-21 15:03:27 adonath>
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
// THE AUTHOR AND PUBLISHER MAKE NO REPRESENTATIONS OR WARRANTIES        //
// ABOUT THE SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED,     //
// INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF                //
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR                 //
// NON-INFRINGEMENT. THE AUTHORS AND PUBLISHER SHALL NOT BE LIABLE FOR   //
// ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR   //
// DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.                        //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <time.h>

#include "files.h"
#include "utils.h"
#include "options.h"

#define THIS_PROG "gappy"

using namespace std;
using namespace files;
using namespace utils;
using namespace wol;

int main (int argc, char *argv[])
{

  try 
    {
      bool DEBUG = false;
      bool LOG = false;
      bool nexus = false;
      bool phylip = false;

      std::vector<double> tokens;
      std::string line;
      std::string unknownchar = "?";
      std::ostringstream message;
      std::ostringstream final;

      cout.precision(10);
      message.precision(10);
      final.precision(10);
 
      const std::string version = "1.0";
      message << endl << THIS_PROG << " extracts splids (split-inducing indels) "
	      << endl << "from multiple sequence alignments in FASTA format. "
	      << endl << "Minimum and maximum values for splid length can be "
	      << endl << "selected."
	      << endl << "By default, the output is a FASTA file, containing the "
	      << endl << "binary coded splid absence/presence information, and "
	      << endl << "some summary statistics. Output is also available in "
	      << endl << "PHYLIP and NEXUS format"
	      << ".";
      const std::string description = message.str();
      message.str("");

      wol::Options::definition_t definition[] = 
	{
	  // {long, short, default, description},
	  // definitions
	  {"definition", "", "value", ""},
      
	  // arguments
	  //	{"UNNAMED", "NN", "REQ",  ""},
	  //	{"", "NN", "",  "ADDITIONS"},

	  // generic options
	  {"", "", "", "\nGeneric options:"},
	  {"help", "h", "FLAG", "Display this help and exit."},
	  {"version", "", "FLAG", "Output version information and exit."},
	  {"verbose", "v", "FLAG", "Show what's being done."},
	  {"debug", "", "FLAG", "Show even more informations on what's being done."},

	  // configuration
	  {"", "", "", "\nConfiguration:"},
	  {"fasta", "f", "", "Name of alignment file in FASTA format."},
	  {"maf", "m", "", "Name of alignment file in MAF format."},
	  {"output-prefix", "o", "GAPPY", "Prefix for the output files."},
	  {"minsize", "l", "2", "Minimum indel size."},          
	  {"maxsize", "u", "", "Maximum indel size."},  
	  {"gapsize", "g", "", "Only search for indels of this size."},
	  {"fuzzy", "z", "FLAG", "Use fuzzy search."},
	  {"unknownchar", "c", "?", "Unknown sites."},
	  {"wstart", "" , "", "Start search at this column. Numbering starts with 1."},
	  {"wend", "" , "", "End search at this column. Numbering starts with 1."},
	  {"nexus", "n", "FLAG", "Write result in NEXUS format. Default: FASTA format only."},
	  {"phylip", "p", "FLAG", "Write result in PHYLIP format. Default: FASTA format only."},
	  {"steps", "", "FLAG", ""},
	  //	{"bool", "c", "0", "bool"},
	  //	{"float", "f", "1.2", "float"},
	  //	{"double", "d", "REQ", "double"},
	  //	{"string", "u", " pre suf ", "string"},
	  //	{"empty", "", "", "string"},
	  // end of definition
	  {"", "", "", ""}
	};

      std::string remote_file = ".options.rc";
      
      const wol::Options options(version, description, definition, argc, argv, remote_file);

      if (options.value<bool>("help")) 
	{
	  std::cerr << options.help_message();
	  return(EXIT_SUCCESS);
	}
      else if (options.value<bool>("steps"))
	{
	  utils::steps(true);
	  cout << endl;
	  return(EXIT_SUCCESS);
	}
      else if (options.value<bool>("version")) 
	{
	  std::cerr << options.version_message();
	  return(EXIT_SUCCESS);
	}
      else if (options.value<bool>("debug"))
	{
	  DEBUG = true;
	  LOG = true;
	}
      else if (options.value<bool>("verbose"))
	{
	  DEBUG = false;
	  LOG = true;
	}
      if (options.value<bool>("nexus"))
	{
	  nexus = true;
	}
      if (options.value<bool>("phylip"))
	{
	  phylip = true;
	}
      if (options.error()) 
	{
	  std::cerr << options.error_message();
	  return(EXIT_FAILURE);
	}

      /* starting time */
      time_t start, end;
      struct tm * timeinfo;
      time ( &start );
      timeinfo = localtime ( &start );
      message << "Starting: " << asctime (timeinfo);
      utils::msg(message.str(),LOG);
      message.str("");

      /* used options */
      message << "options:" << std::endl << options.str(2);
      utils::debugmsg(message.str(),DEBUG);
      message.str("");

      /* Fasta or MAF */
      message << "Reading alignment...";

      std::string prefix = ""; 
      prefix.append(options.value("output-prefix").c_str());
      prefix.append("_");

      files::Fasta * fastaFile =  new files::Fasta;

      if (options.value("maf") != "") //TODO
	{
	  message << "MAF";
	  utils::msg(message.str(), LOG);
	  message.str("");

	  std::string maf = options.value("maf").c_str();
	  prefix.append(maf);
	  std::vector<unsigned int> paragraph_pos;
	  files::scanMaf (&paragraph_pos, maf);
	  std::vector<maf_t> mafSeqs;
      
	  /* all maf alignments
	     for (unsigned int i = 0 ; i < paragraph_pos.size(); ++i)
	     {files::getMafAlignments(&mafSeqs, paragraph_pos[i],maf);}
	  */
	  //get only first
	  files::getMafAlignments(&mafSeqs, paragraph_pos[0], maf);
	  files::mafToFastaAln(mafSeqs, fastaFile); 
	} 
      else if (options.value("fasta") != "")
	{
	  message << "Fasta.";
	  utils::msg(message.str(), LOG);
	  message.str("");


	  if (fastaFile->is_fasta_file(options.value("fasta").c_str()))
	    {
	      fastaFile->append_file(options.value("fasta").c_str());
	    }
	  else
	    {
	      utils::errmsg ("File is no FASTA file.", __LINE__);
	    }

	  prefix.append(options.value("fasta").c_str());

	}
      else
	{
	  utils::msg(message.str(), LOG);
	  message.str("");
	  std::cerr << endl << endl
		    << "Please provide alignment file [-m MAF] or [-f FASTA]."
		    << endl << endl << options.help_message();
	  return(EXIT_SUCCESS);
	}


      if (options.value("unknownchar") != "")
	{
	  unknownchar = options.value("unknownchar").c_str();
	  if (unknownchar.size() > 1)
	    {
	      std::cerr << "Please choose single character [-c]." << endl;
	      return(EXIT_SUCCESS);
	    }
	}
      
      unsigned int alnNoOfSites = fastaFile->length();
      /* windows */
      unsigned int windowStart = 0;
      unsigned int wstart = atoi(options.value("wstart").c_str());

      if (wstart == 0)
	{}
      else
	{
	  if(wstart < 0 || wstart > alnNoOfSites)
	    {
	      std::cerr << "Invalid window start parameter." << endl;
	      return(EXIT_SUCCESS);
	    }
	  else 
	    {
	      windowStart = wstart;
	    }
	}

      unsigned int windowEnd = alnNoOfSites;
      unsigned int wend = atoi(options.value("wend").c_str());
      
      if (wend == 0)
	{}
      else 
	{
	  if(wend > windowEnd || wend < windowStart)
	    {
	      std::cerr << "Invalid window end parameter:" << endl;
	      //std::cerr << options.help_message();
	      return(EXIT_SUCCESS);
	    }
	  else
	    {
	      windowEnd = wend;
	    }
	}

      if(windowEnd <= windowStart)
	{
	  std::cerr << "Invalid window end parameter." << wend << " vs " <<  windowEnd
		    << endl;
	  std::cerr << options.help_message();
	  return(EXIT_SUCCESS);
	}

      /* gap size */
      unsigned int minSize;
      unsigned int maxSize;
      if( atoi(options.value("gapsize").c_str()) )
	{
	  minSize = atoi(options.value("gapsize").c_str());
	  maxSize = minSize;
	}
      else if ( atoi(options.value("minsize").c_str()) 
		&& 
		atoi(options.value("maxsize").c_str()) )
	{
	  minSize = atoi(options.value("minsize").c_str());
	  maxSize = atoi(options.value("maxsize").c_str());
	}
      else if ( atoi(options.value("minsize").c_str()) 
		&& 
		!atoi(options.value("maxsize").c_str()) )
	{
	  minSize = atoi(options.value("minsize").c_str());
	  maxSize = alnNoOfSites;
	}
      else
	{
	  std::cerr << endl << endl 
		    << "Please choose minimum indel size [-l] OR exact indel "
		    << "size [-g]." 
		    << endl << endl << options.help_message();
	  return(EXIT_SUCCESS);
	}

      if ( maxSize < minSize )
	{
	  std::cerr << endl << endl 
		    << "Maximum indel size [-u] is smaller than minimum indel size [-l]." 
		    << endl << endl << options.help_message();
	  return(EXIT_SUCCESS);
	}
      
      prefix.append("_");
      prefix.append(utils::toString(minSize));
      prefix.append("-");
      prefix.append(utils::toString(maxSize));

      /* fuzzy */
      bool fuzzy = false;
      if (options.value<bool>("fuzzy"))
	{
	  fuzzy = true;
	  prefix.append("_z");
	}

      /* print some details about the data set */
      ///      unsigned int alnNoOfSeqs = completeSites->getNumberOfSequences(); //
      unsigned int alnNoOfSeqs = fastaFile->size();
      message << "Number of sequences in alignment: " << alnNoOfSeqs << endl
	      << "Number of sites in alignment: " << alnNoOfSites << endl
	      << "Searching for indels of at least: " << minSize << endl
	      << "and at most: " << maxSize << endl;
      utils::msg(message.str(), LOG);
      message.str("");

      //DO STUFF HERE
      //find gaps in all sequences
      std::vector< gaps_t > gapsV; //size = #of seqs 
                                   //gapsV[i].size() = sequence length
      for (unsigned int i = 0; i < alnNoOfSeqs ; i++)
	{
	  std::string seq = fastaFile->seq(i);
	  utils::findGapsInSeq(i, seq, &gapsV, 1, alnNoOfSites, unknownchar[0]);
	}

      //get names
      std::vector<std::string> names;
      for (unsigned int i = 0 ; i < alnNoOfSeqs ; i++)
	{
	  names.push_back(fastaFile->tag(i));
	}
      if (fastaFile)
	{
	  delete fastaFile;
	}

      //utils::printGaps(gapsV);
      message << "Finding identical gaps.";
      utils::msg(message.str(),LOG);
      message.str("");

//      for (unsigned int i = 0 ; i < alnNoOfSeqs ; i++)
//      	{
//      	  cout << ">" << names[i] << "\n";
//      	  for (unsigned int j = 0 ; j < alnNoOfSites ; j++)
//      	    {
//	      cout << i << " " << j << ":";
//	      if (gapsV[i][j].unknown)
//		cout << "?" << endl;
//	      else
//		cout << "G" << endl;
//      	    }
//      	  cout << endl;
//      	}
//      cout << endl << endl;


      utils::findIdenticalGaps(&gapsV, alnNoOfSites);
      //utils::printGaps(gapsV);
      
      message << "Process Indels.";
      utils::msg(message.str(),LOG);
      message.str("");

      utils::checkIndels2(&gapsV, alnNoOfSites, minSize, maxSize, fuzzy);
      //utils::printGaps(gapsV);

      /* before */
      //std::vector< std::vector< std::vector<int> > > partitions;
      //std::vector< std::string > blablubb;
      //std::vector< std::vector<string> > scores(alnNoOfSeqs, blablubb);
      std::vector< std::vector<char> > scores(alnNoOfSeqs, vector<char>(0,0));
      std::map<unsigned int, short> statistic;

      utils::markUnknown (&gapsV, alnNoOfSites, alnNoOfSeqs);

      message << "Analyze partitions." << endl;
      utils::msg(message.str(),LOG);
      message.str("");
      utils::findPartition(&gapsV, alnNoOfSites, alnNoOfSeqs, &scores, minSize, 
			   maxSize,unknownchar, &statistic);

      //utils::findPartitionOld(&gapsV, alnNoOfSites, alnNoOfSeqs, &partitions,  
      //                        &scores, &statistic, minSize, maxSize, unknownchar);
      //write 01? sequences

      gapsV.clear();

      message << "Writing result files.";
      utils::msg(message.str(),LOG);
      message.str("");
      //write indel statistic
      files::writeStat (prefix, statistic, minSize, maxSize);
      //write fasta file
      files::writeFasta (scores, names, alnNoOfSeqs, prefix);
      //write nexus file
      if (nexus)
	{
	  std::string notes = "created by ";
	  notes.append(THIS_PROG);
	  files::writeNexus (scores, names, unknownchar, alnNoOfSeqs, prefix, notes);
	}
      //write nexus file
      if (phylip)
	{
	  files::writePhylip (scores, names, alnNoOfSeqs, prefix);
	}

      time ( &end );
      timeinfo = localtime ( &end );
      double dif = difftime (end,start);
      message << "Done: " << asctime (timeinfo);
      unsigned int seconds = (unsigned int)dif % 60;
      unsigned int total_minutes =  (unsigned int)dif / 60;
      unsigned int minutes = total_minutes % 60;
      unsigned int total_hours = total_minutes / 60;
      unsigned int hours = total_hours % 24;
      unsigned int days = total_hours / 24;
      message << "Time needed: ";
      if(days >= 1)
	message << days << "d ";
      message << total_hours << "h " << minutes << "min " << seconds << "sec." << endl;
      utils::msg(message.str(),LOG);
      message.str("");

    } 
  catch (exception & e) 
    {
      utils::errmsg("Something went wrong. Check your input data!", e.what());
    }

  cout << endl << endl;

  return 0;
}
