/**
 *  last changed Time-stamp: <2016-10-21 16:51:29 adonath>
 *
 *  Author: Alexander Donath, a <dot> donath <at> zfmk <dot> de
 */

#include <cmath>
#include <limits>
#include <set>
#include <vector>
#include <sstream>

using namespace std;

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))

namespace utils {

  struct gap_t {
    int start;
    int stop;
    short int sim;
    short unsigned int count;
    bool unknown;
  };
  typedef std::vector< gap_t > gaps_t;

  /**
   * Prints error messages.
   *
   * @param msg The message string
   * @author Alexander Donath
   */
  void errmsg (std::string msg);

  /**
   * Prints error messages.
   *
   * @param msg The message string
   * @param code the message code 
   * @author Alexander Donath
  */
  void errmsg (std::string msg, int code=1);

  /**
   * Prints error messages.
   *
   * @param msg The message string
   * @param *details more details on the message 
   * @author Alexander Donath
   */
  void errmsg (std::string msg, const char* details);

  /**
   * Prints log messages.
   *
   * @param msg string that contains log message
   * @param log bool whether log message should be printed or not.
   * @author Alexander Donath
   */
  void msg (std::string msg, bool log = false);

  /**
   * Prints debug messages.
   *
   * @param msg string that contains debug message
   * @param debug bool whether debug message should be printed or not.
   * @author Alexander Donath
   */
  void debugmsg (std::string msg, bool debug = false);

  /**
   * Replace all occurences of string FROM in string ORG to string TO.
   *
   * @param &org string Original string
   * @param &from string String that should be changed
   * @param &to string Replacement for from string
   * @return the new string
   * @author Alexander Donath
   */
  string& replaceAllStrings (std::string& org, const std::string& from, const std::string& to);

  /**
   * Replace all occurences of char FROM in string ORG to string TO.
   *
   * @param &org string Original string
   * @param from char Char that should be changed
   * @param &to string Replacement for from char
   * @return the new string
   * @author Alexander Donath
   */
  string& replaceAllChars (std::string& org, const char from, const std::string& to);

  /**
   * Tokenizes a string into (double) parts and filles vector. 
   * Delimiter is whitespace by default.
   *
   * add some more details
   *
   * @param &str string that shall be split 
   * @param &tokens 
   * @param &delimiters
   * @author Alexander Donath
   */
  void tokenize (const std::string& str,
		 std::vector<double>& tokens,
		 const std::string& delimiters = " ");


  /**
   * Tokenizes a string into string parts and filles vector. 
   * Delimiter is whitespace by default.
   *
   * add some more details
   *
   * @param &str string that shall be split 
   * @param &tokens 
   * @param &delimiters
   * @author Alexander Donath
   */
  void tokenize(const std::string& str, 
		std::vector<std::string>& tokens, 
		const std::string& delimiters = " ");

  /**
   * Removes comments
   *
   * @param *data string to be uncommented
   * @param &sign char that identifies the beginning of a commment
   * @author Alexander Donath
   */
  void uncomment (std::string* data, 
		 const std::string& sign = "#");

  /**
   * Removes whitespaces. 
   *
   * No more details needed. Or?
   *
   * @param *data string which includes the whitespaces
   * @author Wolfgang Otto
   */
  void trim (std::string* data);

  /** 
   * Encode nucleotide character to integer.
   *
   * encode() can deal with gaps (-), unknown (N),
   * as well as upper and lower case nucleotides.
   *
   * @param ch the character to be encoded
   * @return encoded integer value
   * @author Steve Hoffmann
   */
  int encode (char ch);

  /** 
   * Decode integer value to nucleotide character.
   *
   * @param  ch integer value to be decoded
   * @return decoded character in upper case
   * @author Steve Hoffmann
   */
  char decode (int ch);

  void encrypt(string &target);

  string decrypt(string target);
  
  int round2int(double num);

  void steps (bool dt = true);

  /**
   * Template for the conversion of any type to a string.
   * 
   * @param t parameter which shall be returned as string
   * @return ss a string
   * @author Alexander Donath
   */
  template <class T>
    inline std::string toString(T t)
    {
      std::stringstream ss;
      ss << t;
      return ss.str();
    }

  /**
   * Template to detect -inf or inf
   *
   * @param x Value to check.
   * @return false if x is inf or -inf, true else.
   * @author Alexander Donath
   */
  template<typename T>
    inline bool isNotInf(T x)
    {
      return (x >= std::numeric_limits<T>::min() 
	      && x <= std::numeric_limits<T>::max() );
    }

  /** 
   * Template to check if x is a number.
   *
   * @param x Value to check.
   * @return true if x is a number, false else (if x is NaN).
   * @author Alexander Donath
   */ 
  template <typename T>
    inline bool isNumber(T x)
    {
      return (x == x);
    }
  
  /**
   * Find the number of different keys in a multimap.
   *
   *
   */
  template <typename K, typename V>
    inline size_t numberOfKeys(const multimap<K, V> &mm)
  {
    if (mm.empty())
      return 0;
    
    size_t noOfKeys = 1;
    multimap<int, int>::const_iterator i = mm.begin();
    K currKey = i->first;
    ++i;
    
    while (i != mm.end()) 
      {
	if (currKey < i->first) 
	  {
	    ++noOfKeys;
	    currKey = i->first;
	  }
	++i;
      }
    return noOfKeys;
  }


 /**
   * Find all different keys in a multimap.
   *
   *
   */
  template <typename K, typename V>
    inline set<K> getDistinctKeys(const multimap<K, V> &mm)
  {
    set<K> kSet;
    multimap<int, int>::const_iterator i = mm.begin();
    K currKey = i->first;
    ++i;
    kSet.insert(currKey);
    
    while (i != mm.end()) 
      {
	if (currKey < i->first) 
	  {
	    currKey = i->first;
	    kSet.insert(currKey);
	  }
	++i;
      }
    return kSet;
  }


  /**
   * Find all different keys in a vector of vectors. Key is jth
   * entry of the inner vector.
   *
   *
   */
  template <typename T>
    inline set<T> getDistinctKeysVV(const std::vector< std::vector< T > > &VV, const int j)
    {

      std::set<T> kSet;
      for (unsigned int i = 0 ; i < VV.size() ; ++i)
	{
	  kSet.insert(VV[i][j]);
	}
      return kSet;
    }


  /**
   * Compares to floating point numbers.
   *
   * The solution is not completely symmetric. That is, it is possible
   * that isEqual(x,y) != isEqual(y,x). Does not usually occur when
   * the magnitudes of x and y are significantly larger than epsilon
   * (= 1e-20).
   *
   * @param x the first floating point number
   * @param y the second floating point number
   * @return true if x==y, false otherwise
   * @author Alexander Donath
   */
  inline bool isEqual (double x, double y)
  {
    const double epsilon = 1e-20;
    return std::abs(x - y) <= epsilon * std::abs(x);

    // see Knuth section 4.2.2 pages 217-218
  } 
  
  /**
   * Finds all gaps of a given size (minSize <= gapsize <= maxSize) in
   * a sequence.
   *
   */
  void findGapsInSeq (const unsigned int seqId, const std::string &seq, 
		      std::vector<gaps_t> *gapsV, const unsigned int minSize,
		      const unsigned int maxSize, const char unknownChar);


  /**
   * Finds all identical gaps in an alignment.
   *
   */
  void findIdenticalGaps (std::vector<gaps_t> *gapsV, 
			  const unsigned int alnLength);
  
  /**
   * Identifies simple column-defined partitions.
   *
   */
  void findPartitionOld (std::vector<gaps_t> *gapsV, 
			 const unsigned int alnLength,
			 const unsigned int alnSize,
			 std::vector< std::vector< std::vector<int> > > *partitions,
			 //std::vector< std::multimap<int,int> > *partition,
			 std::vector< std::vector<char> > *scores,
			 std::map<unsigned int, short> *statistic,
			 const unsigned int minSize,
			 const unsigned int maxSize, 
			 const std::string unknownchar);

  void markUnknown (std::vector<gaps_t> *gapsV,
		    const unsigned int alnLength,
		    const unsigned int noOfSeqs);

  void findPartition (std::vector<gaps_t> *gapsV,
		      const unsigned int alnLength,
		      const unsigned int noOfSeqs,
		      std::vector< std::vector<char> > *scores, 
		      const unsigned int minSize,
		      const unsigned int maxSize,
		      const std::string unknownchar,
		      std::map<unsigned int, short> *statistic);
  /**
   * Do not use this function.
   *
   */
  void findGapNeighborhood (std::vector<gaps_t> *gapsV, 
			    const unsigned int region, 
			    const unsigned int alnLength);

  /**
   * Prints informations about all found gaps.
   *
   */
  void printGaps (const std::vector<gaps_t> &gapsV);

  /**
   * Updates information about identical gaps after, e.g. increase of
   * some.
   *
   */
  void updateIdenticalGaps(std::vector<gaps_t> *gapsV, 
			   const unsigned int alnLength);

  /**
   * Returns the starting position of the next gap to the right.
   *
   */
  int findRightGap(const std::vector< gap_t > &currSeq, 
		   const unsigned int currPos);

  /**
   * Returns the starting position of the next gap to the left.
   *
   */
  int findLeftGap(const std::vector< gap_t > &currSeq, 
		  const unsigned int currPos);

  /**
   * Checks if a given column contains at least two types of (gap)
   * characters, and at least two of them occur with a minimum
   * frequency of two.
   *
   */
  bool parsimonyInformative(const std::vector< std::vector< int > > &VV, 
			    const std::set<int> &keys);
  
  /**
   *
   *
   */
  bool isTrueBipartition (std::vector<gaps_t> *gapsV, const unsigned int seqPos,
			  const unsigned int siteId);

  void demarkColumn (std::vector<gaps_t> *gapsV, const unsigned int siteId);

  void demarkPosition (std::vector<gaps_t> *gapsV, const unsigned int seqId,
		       const unsigned int siteId);

  void markPosition (std::vector<gaps_t> *gapsV, const unsigned int seqId,
		     const unsigned int siteId, const int start, 
		     const int stop, const int sim);

  /**
   *
   *
   */
  void checkIndels (std::vector<gaps_t> *gapsV, const unsigned int alnLength,
		    const unsigned minSize, const unsigned maxSize,  
		    const bool fuzzy);

  /**
   *
   *
   */
  void checkIndels2 (std::vector<gaps_t> *gapsV, const unsigned int alnLength,
		     const unsigned minSize, const unsigned maxSize,  
		     const bool fuzzy);

  /**
   *
   *
   */
  void StatusBar (unsigned int cur, unsigned int max, unsigned int size);

  /**
   *
   *
   */
  std::vector<std::string> split (const std::string &seq, unsigned int length);


}
