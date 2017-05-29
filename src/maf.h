/**
 *  last changed Time-stamp: <2016-10-21 16:53:21 adonath>
 *
 *  Author: Alexander Donath, a <dot> donath <at> zfmk <dot> de
 */

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


