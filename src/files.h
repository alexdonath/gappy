/**
 *  last changed Time-stamp: <2017-08-16 19:20:24 adonath>
 *
 *  Author: Alexander Donath, a <dot> donath <at> zfmk <dot> de
 */

#include <vector>
#include <string>
#include <map>

using namespace std;

namespace files {

  /************************************************************************
   ** Fasta
   ***********************************************************************/

  class Fasta {

    // intern types
  private:

    struct entry_t {
      std::string tag;
      std::string id;
      std::string seq;
    };
    typedef std::vector< entry_t > entries_t;
    typedef std::map< std::string, size_t > index_t;

    // static
  private:

    static const size_t __FASTA_WIDTH = 60;

    // variables
  private:

    entries_t _entries;
    index_t   _index;
    size_t    _length;

    // constructors / destructors
  public:

    Fasta() throw();

    ~Fasta() throw();

    // methods
  public:

    // string 
    const std::string str(const size_t indent = 0) const;

    // infos 
    static const bool is_fasta_stream(std::istream& in);
    static const bool is_fasta_file(const std::string& file_name);

    inline const size_t size() const;
    inline const size_t length() const;
    inline const std::string& tag(size_t index) const;
    inline const std::string& id(size_t index) const;
    inline const std::string& seq(size_t index) const;
    inline const std::string seq(size_t index, size_t pos,
				 size_t length = std::string::npos) const;
    inline const bool exists(const std::string& tag) const;
    inline const std::string& seq(const std::string& tag) const;
    inline const std::string seq(const std::string& tag, size_t pos,
				 size_t length = std::string::npos) const;
    inline const size_t index(const std::string& tag) const;

    // algorithms 
    void append_entry(const std::string& tag, const std::string& seq);
    void append_stream(std::istream& in);
    void append_file(const std::string& file_name);
    void clear();
    void write_file(const std::string& file_name);
  };

  /*#######################################################################
    ## definitions
    #####################################################################*/

  // methods

  // infos
  inline const size_t Fasta::size() const {
    return(_entries.size());
  }

  inline const size_t Fasta::length() const {
    return(_length);
  }

  inline const std::string& Fasta::tag(size_t index) const {
#ifdef DEBUG
    if (index >= _entries.size()) {
      std::ostringstream message;
      message << "Index '" << index << "' is out of entries range ("
              << _entries.size() <<")";
      utils::errmsg (message.str(), __LINE__);
    }
#endif
    return(_entries[index].tag);
  }

  inline const std::string& Fasta::id(size_t index) const {
#ifdef DEBUG
    if (index >= _entries.size()) {
      std::ostringstream message;
      message << "Index '" << index << "' is out of entries range ("
              << _entries.size() <<")";
      utils::errmsg(message.str(), __LINE__);
    }
#endif
    return(_entries[index].id);
  }

  inline const std::string& Fasta::seq(size_t index) const {
#ifdef DEBUG
    if (index >= _entries.size()) {
      std::ostringstream message;
      message << "Index '" << index << "' is out of entries range '"
              << _entries.size() <<"'";
      utils::errmsg(message.str(), __LINE__);
    }
#endif
    return(_entries[index].seq);
  }

  inline const std::string Fasta::seq
    (size_t index, size_t pos, size_t length) const {
    const std::string ref = seq(index);
#ifdef DEBUG
    if (pos >= ref.size()) {
      std::ostringstream message;
      message << "Pos '" << pos << "' is out of seq range";
      utils::errmsg(message.str(), __LINE__);
    }
#endif
    return(ref.substr(pos, length));
  }


  inline const bool Fasta::exists(const std::string& tag) const {
    size_t pos = tag.find_first_of(" \t");
    std::string id = tag.substr(0, pos);
    index_t::const_iterator iter = _index.find(id);
    return(iter != _index.end());
  }

  inline const std::string& Fasta::seq(const std::string& tag) const {
    size_t pos = tag.find_first_of(" \t");
    std::string id = tag.substr(0, pos);
    index_t::const_iterator iter = _index.find(id);
#ifdef DEBUG
    if (iter == _index.end()) {
      std::ostringstream message;
      message << "Entry with tag '" << tag << "' does not exists";
      utils::errmsg(message.str(), __LINE__);
    }
#endif
    return(seq(iter->second));
  }

  inline const std::string Fasta::seq
    (const std::string& tag, size_t pos, size_t length) const {
    const std::string ref = seq(tag);
#ifdef DEBUG
    if (pos >= ref.size()) {
      std::ostringstream message;
      message << "Pos '" << pos << "' is out of seq range";
      utils::errmsg(message.str(), __LINE__);
    }
#endif
    return(ref.substr(pos, length));
  }

  inline const size_t Fasta::index(const std::string& tag) const {
    size_t pos = tag.find_first_of(" \t");
    std::string id = tag.substr(0, pos);
    index_t::const_iterator iter = _index.find(id);
#ifdef DEBUG
    if (iter == _index.end()) {
      std::ostringstream message;
      message << "Entry with tag '" << tag << "' does not exists";
      utils::errmsg(message.str(), __LINE__);
    }
#endif
    return(iter->second);
  }

  /************************************************************************
   ** END Fasta
   ***********************************************************************/

  struct maf_t {
    std::string src;
    unsigned int start;
    unsigned int length;
    char strand;
    unsigned int srcsize;
    std::string text;
  };

  /**
   *
   */
  void scanMaf (std::vector<unsigned int>* positions, 
		const std::string& mafFile);
  /**
   *
   */
  void getMafAlignments (std::vector<files::maf_t>* maf, 
			 const unsigned int pos, 
			 const std::string& mafFile);
  /**
   *
   */
  void mafToFastaAln (const std::vector<maf_t>& maf, 
		      files::Fasta * fastaFile);
  /**
   *
   */
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


