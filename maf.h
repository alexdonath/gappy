/**
 *  last changed Time-stamp: <2016-10-21 15:07:44 adonath>
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

#include <vector>

#include <Seq/containers>

using namespace std;
using namespace bpp;

namespace maf {

  struct maf_t {
    
    std::string src;
    unsigned int start;
    unsigned int length;
    char strand;
    unsigned int srcsize;
    std::string text;
  };


  void scanMaf (std::vector<unsigned int>* positions, 
		const std::string& mafFile);

  void getMafAlignments (std::vector<maf::maf_t>* maf, 
			 const unsigned int pos, 
			 const std::string& mafFile);

  void mafToFastaAln (const std::vector<maf_t>& maf, 
		      bpp::AlignedSequenceContainer * aln);

  void writeStat (std::string prefix, 
		      const std::map<unsigned int, short>& statistic,
		      const unsigned int minSize, 
		      const unsigned maxSize);

  /**
   * Write splid coding in NEXUS format.
   *
   */
  void writeNexus (const std::vector< std::vector<char> > & scores,
		   const std::vector< std::string > & names,
		   const std::string unknownchar,
		   unsigned int alnNoOfSeqs, 
		   std::string prefix, std::string notes);

  /**
   * Write splid coding in FASTA format.
   *
   */
  void writeFasta (const std::vector< std::vector<char> > & scores,
		   const std::vector< std::string > & names,
		   unsigned int alnNoOfSeqs, std::string prefix);
  /**
   * Write splid coding in PHYLIP format.
   *
   */
  void writePhylip (const std::vector< std::vector<char> > & scores,
		    const std::vector< std::string > & names,
		    unsigned int alnNoOfSeqs, std::string prefix);
}


