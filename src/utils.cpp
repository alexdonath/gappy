/**
 *  last changed Time-stamp: <2017-08-16 19:03:18 adonath>
 *
 *  Author: Alexander Donath, a <dot> donath <at> zfmk <dot> de
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <math.h>
#include <stdio.h>

#include "utils.h"

using namespace std;
using namespace utils;

int gap=5;

void utils::errmsg (std::string msg)
{
  std::cerr << endl << "[ERROR] Details: " << msg << endl;
  exit(EXIT_FAILURE);
}


void utils::errmsg (std::string msg, int code)
{
  std::cerr << endl << "[ERROR] Details: " << msg << " [LINE " << 
    code << "]" << endl;
  exit(EXIT_FAILURE);
}


void utils::errmsg (std::string msg, const char *details)
{
  std::cerr << endl << "[ERROR] An exception occured: " << msg << endl;
  std::cerr << " [EXCEPTION]: " << details << endl ;
  exit(EXIT_FAILURE);
}


void utils::msg (std::string msg, bool log)
{
  if (log) 
    {
      msg = utils::replaceAllChars(msg, '\n', "\n[LOG] ");
      std::clog << endl << "[LOG] " << msg;
    }
}


void utils::debugmsg (std::string msg, bool debug)
{

  if (debug)
    {
      msg = utils::replaceAllChars(msg, '\n', "\n[DEBUG] ");
      std::clog << endl << "[DEBUG] " << msg;
    }
}


string& utils::replaceAllStrings(std::string& org, const std::string& from, 
				 const std::string& to)
{
  size_t start = 0;
  size_t pos;
  while((pos = org.find(from, start)) != string::npos)
    {
      org.replace(pos, from.size(), to);
      start = pos + to.size();
    }
  return org;
}


string& utils::replaceAllChars(std::string& org, const char from, 
			       const std::string& to)
{
  size_t start = 0; //maybe start at 1 and end at lenght-1?
  size_t pos;
  while((pos = org.find(from, start)) != string::npos)
    {
      org.replace(pos, 1, to);
      start = pos + to.size();
    }
  return org;
}


void utils::tokenize(const std::string& str,
		     std::vector<double>& tokens,
		     const std::string& delimiters)
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(atof (str.substr(lastPos, pos - lastPos).c_str()));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}


void utils::tokenize(const std::string& str, 
		     std::vector<std::string>& tokens, 
		     const std::string& delimiters) 
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos).c_str());
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}


void utils::uncomment(std::string* data, const std::string& sign) 
{
  size_t esc = 0;
  unsigned int dataSize = data->size();
  unsigned int signSize = sign.size();
  bool match;
  size_t i = 0;
  while (i < dataSize) 
    {
      if ((*data)[i] == '\\') 
	++esc;
      else 
	{
	  match = true;
	  for (size_t j = 0; j < signSize; ++j) 
	    {
	      if ((*data)[i + j] != sign[j]) 
		{
		  match = false;
		  break;
		}
	    }
	  if (match && esc % 2 == 0) 
	    {
	      // comment start
	      data->erase(i);
	      break;
	    }
	  esc = 0;
	}
      ++i;
    }
}


void utils::trim(std::string* data) 
{
  if (*data != "") 
    {
      size_t begin = data->find_first_not_of(" \t\n");
      size_t end = data->find_last_not_of(" \t\n");
      if (begin != std::string::npos) 
	*data = data->substr(begin, end - begin + 1);
      else 
	*data = "";
    }
}


char utils::decode(int ch) 
{
  char ret = 'A';
  switch(ch)  
    {
    case 0: ret = 'A';
      break;
    case 1: ret = 'C';
      break;
    case 2: ret = 'G';
      break;
    case 3: ret = 'T';
      break;
    case 4: ret = '-';
      break;
    default: ret = 'N';
      break;
    }
  return ret;
}


int utils::encode(char ch) 
{
  int ret = 0;
  switch(ch)  
    {
    case 'A':
    case 'a': ret = 0;
      break;
    case 'C':
    case 'c': ret = 1;
      break;
    case 'G':
    case 'g': ret = 2;
      break;
    case 'T':
    case 'U':
    case 't':
    case 'u': ret = 3;
      break;
    case 'S':
    case 'R':
    case 'Y':
    case 'M':
    case 'K':
    case 'W':
    case 'H':
    case 'B':
    case 'V':
    case 'D':
    case 'N': ret = 5;
      break;
    default: ret = 4;
      break;
    }

  return ret;
}


void utils::encrypt(string &target)
{
  unsigned int len = target.length();
  char a;
  unsigned int b;
  string strFinal(target);
  unsigned int i = 0;
  unsigned int tmp = len-1;

  while (i <= tmp)
    {
      a = target.at(i); 
      if (a != '\n') 
	{
	  b = (unsigned int)a;
	  b += 2;
	  if (b > 254) 
	    b = 254;
	  a = (char)b;
	}
      strFinal.insert(i , 1, a);
      ++i;
    }
  string strEncrypted(strFinal, 0, len);
  target = strEncrypted;
}


string utils::decrypt(string target)
{
  unsigned int len = target.length();
  char a;
  unsigned int b;
  string strFinal(target);
  unsigned int i = 0;
  unsigned int tmp = len-1;

  while (i <= tmp)
    {
      a = target.at(i);
      if(a != '\n') 
	{
	  b = (unsigned int)a;
	  b -= 2;
	  a = (char)b;
	}
      strFinal.insert(i, 1, a);
      ++i;
    }
  string strDecrypted(strFinal, 0, len);
  return strDecrypted;
}


int utils::round2int(double num)
{
  return (int) round(num);
}


void utils::steps (bool dt)
{
  std::ostringstream message;
  
  if(dt)
    message << "\nYkg\"lgfg\"Dnwgvg\"ygnmv\n"
	    << "wpf\"lgfg\"Lwigpf\"fgo\"Cnvgt\"ygkejv.\n"
	    << "dnwgjv\"lgfg\"Ngdgpuuvwhg.\n"
	    << "dnwgjv\"lgfg\"Ygkujgkv\"cwej\"wpf\"lgfg\"Vwigpf\n"
	    << "|w\"kjtgt\"\\gkv\"wpf\"fcth\"pkejv\"gyki\"fcwgtp0\n"
	    << "Gu\"owuu\"fcu\"Jgt|\"dgk\"lgfgo\"Ngdgputwhg\n"
	    << "dgtgkv\"|wo\"Cduejkgf\"ugkp\"wpf\"Pgwdgikppg.\n"
	    << "wo\"ukej\"kp\"Vcrhgtmgkv\"wpf\"qjpg\"Vtcwgtp\n"
	    << "kp\"cpf)tg.\"pgwg\"Dkpfwpigp\"|w\"igdgp0\n"
	    << "Wpf\"lgfgo\"Cphcpi\"yqjpv\"gkp\"\\cwdgt\"kppg.\n"
	    << "fgt\"wpu\"dguejwgv|v\"wpf\"fgt\"wpu\"jknhv\"|w\"ngdgp0\n\n"
	    << "Ykt\"uqnngp\"jgkvgt\"Tcwo\"wo\"Tcwo\"fwtejuejtgkvgp.\n"
	    << "cp\"mgkpgo\"ykg\"cp\"gkpgt\"Jgkocv\"jcgpigp.\n"
	    << "fgt\"Ygnvigkuv\"yknn\"pkejv\"hguugnp\"wpu\"wpf\"gpigp.\n"
	    << "gt\"yknn\"wpu\"Uvwh)\"wo\"Uvwhg\"jgdgp.\"ygkvgp#\n"
	    << "Mcwo\"ukpf\"ykt\"jgkokuej\"gkpgo\"Ngdgpumtgkug\n"
	    << "wpf\"vtcwnkej\"gkpigyqjpv.\"uq\"ftqjv\"Gtuejnchhgp#\n"
	    << "Pwt\"ygt\"dgtgkv\"|w\"Cwhdtwej\"kuv\"wpf\"Tgkug.\n"
	    << "oci\"ncgjogpfgt\"Igyqgjpwpi\"ukej\"gpvtchhgp0\n\n"
	    << "Gu\"yktf\"xkgnngkejv\"cwej\"pqej\"fkg\"Vqfguuvwpfg\n"
	    << "wpu\"pgwgp\"Tcgwogp\"lwpi\"gpvigigp\"ugpfgp<\n"
	    << "fgu\"Ngdgpu\"Twh\"cp\"wpu\"yktf\"pkgocnu\"gpfgp0\n"
	    << "Yqjncp\"fgpp.\"Jgt|.\"pkoo\"Cduejkgf\"wpf\"iguwpfg#\n"
	    << "*Jgtocpp\"Jguug.\"Uvwhgp+\n";
  else 
    message << "\nNkmg\"gx)t{\"hnqygt\"yknvu.\"nkmg\"{qwvj\"ku\"hcfkpi\n"
	    << "cpf\"vwtpu\"vq\"cig.\"uq\"cnuq\"qpg)u\"cejkgxkpi<\n"
	    << "Gcej\"xktvwg\"cpf\"gcej\"ykufqo\"pggfu\"rctcfkpi\n"
	    << "kp\"qpg)u\"qyp\"vkog.\"cpf\"owuv\"pqv\"ncuv\"hqtgxgt0\n"
	    << "Vjg\"jgctv\"owuv\"dg.\"cv\"gcej\"pgy\"ecnn\"hqt\"ngcxkpi.\n"
	    << "rtgrctgf\"vq\"rctv\"cpf\"uvctv\"ykvjqwv\"vjg\"vtcike.\n"
	    << "ykvjqwv\"vjg\"itkgh\"/\"ykvj\"eqwtcig\"vq\"gpfgcxqt\n"
	    << "c\"pqxgn\"dqpf.\"c\"fkurctcvg\"eqppgevkqp<\n"
	    << "hqt\"gcej\"dgikppkpi\"dgctu\"c\"urgekcn\"ocike\n"
	    << "vjcv\"pwtvwtgu\"nkxkpi\"cpf\"dguvqyu\"rtqvgevkqp0\n\n"
	    << "Yg)nn\"ycnm\"htqo\"urceg\"vq\"urceg\"kp\"incf\"rtqitguukqp\n"
	    << "cpf\"ujqwnf\"pqv\"enkpi\"vq\"qpg\"cu\"jqoguvgcf\"hqt\"wu0\n"
	    << "Vjg\"equoke\"urktkv\"yknn\"pqv\"dkpf\"pqt\"dqtg\"wu=\n"
	    << "kv\"nkhvu\"cpf\"ykfgpu\"wu\"kp\"gx)t{\"uguukqp<\n"
	    << "hqt\"jctfn{\"ugv\"kp\"qpg\"qh\"nkhg)u\"gzrcpugu\n"
	    << "yg\"ocmg\"kv\"jqog.\"cpf\"crcvj{\"eqoogpegu0\n"
	    << "Dwv\"qpn{\"jg.\"yjq\"vtcxgnu\"cpf\"vcmgu\"ejcpegu.\n"
	    << "ecp\"dtgcm\"vjg\"jcdkvu)\"rctcn{|kpi\"uvcpegu0\n\n"
	    << "Kv\"okijv\"dg.\"gxgp.\"vjcv\"vjg\"ncuv\"qh\"jqwtu\n"
	    << "yknn\"ocmg\"wu\"qpeg\"cickp\"c\"{qwvjhwn\"nqxgt<\n"
	    << "Vjg\"ecnn\"qh\"nkhg\"vq\"wu\"hqtgxgt\"hnqygtu000\n"
	    << "Cpqp.\"o{\"jgctv<\"Uc{\"hctgygnn\"cpf\"tgeqxgt#\n"
	    << "*Jgtocpp\"Jguug.\"Uvgru+\n" ;
  string string = message.str();
  message.str("");
  cout << decrypt(string);
}


void utils::findGapsInSeq(const unsigned int seqId, const std::string &seq, 
			  std::vector<gaps_t> *gapsV, const unsigned int minSize,
			  const unsigned int maxSize, const char unknownChar)
{
  bool gap = false;
  unsigned int i = 0;
  unsigned int start = 0;
  gaps_t tmp_v;

  while ( i < seq.size() )
    {
      gap_t tmp;
      tmp.start = -1;
      tmp.stop = -1;
      tmp.sim = -1;
      tmp.count = 0;
      tmp.unknown = false;
      tmp_v.push_back(tmp);

      if ((seq[i] == '-') && gap)
	{//cout << "mark: still gap at " << i  << endl;
	  tmp_v[i].start = start;
	}
      else if ((seq[i] == '-') && !gap)
	{//cout << "start gap " << i << endl;
	  tmp_v[i].start = i;
	  start = i;
	  gap = true;
	}
      else if ((seq[i] != '-') && gap)
	{
	  if (i == 0)
	    {//cout << "gap start " << i << endl;
	      tmp_v[i].start = i;
	    }
	  //cout << "not gap anymore " << start << " " <<  i-1 << endl;
	  for (unsigned j = start ; j <= i-1 ; ++j)
	    tmp_v[j].stop = i-1;
	  
	  gap = false;
	}
      else if ((seq[i] != '-') && !gap)
	{//cout << "still no gap" << endl;

//	  if (!seq.getChar(i).compare("?"))
//	    {cout << i << ": unknown" << endl;
//	      tmp_v[i].unknown = true;
//	    }
	}
      else
	utils::errmsg("This shouldn't happen.",__LINE__);

      if (seq[i] == unknownChar)
	{//cout << i << ": unknown" << endl;
	  tmp_v[i].unknown = true;
	}
      
      ++i;
    }

  if (gap)
    {//cout << "gap end " << i-1 << endl;
      for (unsigned j = start ; j <= i-1 ; ++j)
	tmp_v[j].stop = i-1;
    }

  (*gapsV).push_back(tmp_v);

}


void utils::findIdenticalGaps (std::vector<gaps_t> *gapsV, 
			       const unsigned int alnLength)
{
  for (unsigned int j = 0; j < alnLength ; ++j)
    {
      //utils::StatusBar(j, alnLength-1, 20);
      int start;
      int stop;
      //unsigned int seq;
      for (unsigned int i = 0; i < (*gapsV).size() ; ++i)
	{
	  if ((*gapsV)[i][j].start == -1)
	    continue;
	  else
	    {
	      start = (*gapsV)[i][j].start;
	      stop = (*gapsV)[i][j].stop;
	      //seq = i;
	      for (unsigned int k = i; k < (*gapsV).size(); ++k)
		{
		  if ( (*gapsV)[k][j].start == start 
		       && 
		       (*gapsV)[k][j].stop == stop 
		       &&
		       (*gapsV)[k][j].sim == -1)
		    {
		      (*gapsV)[k][j].sim = i;
		    }
		}
	    }
	}
    }
}

void utils::markUnknown (std::vector<gaps_t> *gapsV,
			 const unsigned int alnLength,
			 const unsigned int noOfSeqs)
{

  unsigned int i = 0;
  //unsigned int add = 0;
  while (i < alnLength) //column
    {
      unsigned int j = 0;
      int ustart = -1;
      //int ustop = -1;
      while (j < noOfSeqs) //line
	{
	  if ((*gapsV)[j][i].unknown)
	    {
	      //cout << j << " " << i << endl;
	      unsigned int k = 0;
	      while (k < noOfSeqs)
		{
//		  for (int l = (*gapsV)[j][i].start ; l <= (*gapsV)[j][i].stop ; ++l)
//		    {
//		      if ( ((*gapsV)[k][l].start < ustart && (*gapsV)[k][l].start != -1)
//		     	   ||
//		     	   (ustart == -1 && (*gapsV)[k][l].start > ustart) ) 
//		     	ustart = (*gapsV)[k][l].start;
//		    }

		  if ( ((*gapsV)[k][i].start < ustart && (*gapsV)[k][i].start != -1)
		       ||
		       (ustart == -1 && (*gapsV)[k][i].start > ustart) ) 
		    ustart = (*gapsV)[k][i].start;

		  ++k;
		}
	      //cout << j << " " << ustart << endl;

	      if (ustart != -1)// && (*gapsV)[j][ustart].start != -1)
		{
		  //cout << "mark " << j << " " << ustart << endl;
		  (*gapsV)[j][ustart].unknown = true;
		}
	      else 
		{
		  //cout << "else: " << ustart << " " << (*gapsV)[j][ustart].start << endl;
		}
	    }
	  ++j;
	}
      ++i;
    }
}

void utils::findPartition (std::vector<gaps_t> *gapsV,
			   const unsigned int alnLength,
			   const unsigned int noOfSeqs,
			   std::vector< std::vector<char> > *scores, 
			   const unsigned int minSize,
			   const unsigned int maxSize,
			   const std::string unknownchar, 
			   std::map<unsigned int, short> *statistic)
{
  unsigned int i = 0;
  int add = 0;

  while (i < alnLength)
    {
      //utils::StatusBar(i, alnLength-1, 20);
      unsigned int j = 0;
      std::map <int, int> site;
      std::set <int> site2;
      std::set <int> sizes;
      add = i;
      while (j < noOfSeqs)
	{
	  site2.insert((*gapsV)[j][i].sim);
	  sizes.insert((*gapsV)[j][i].stop - (*gapsV)[j][i].start + 1);
	  site[(*gapsV)[j][i].sim]++;
	  ++j;
	}

      int max = 0;
      bool in = false;
      std::set<int>::iterator it;
      for (it = sizes.begin() ; it != sizes.end() ; ++it)
	max = (*it > max) ? *it : max;

      if ((unsigned) max <= maxSize && (unsigned) max >= minSize)
	in = true;
      else 
	in = false;

      if (site2.size() >= 2 && in)
	{
	  std::map<int, int>::const_iterator it;
	  unsigned int count = 0;
	  bool parsimony = false;
	  for (it=site.begin(); it != site.end(); ++it) 
	    {
	      if (it->second >= 2)
		++count;
	      if (count >= 2)
		{
		  parsimony = true;
		  break;
		}
	    }

	  if (parsimony)
	    {
	      j = 0;
	      if ( (*statistic).count(max) > 0 )
		(*statistic)[max] += 1;
	      else
		(*statistic)[max] = 1;
	      while (j < noOfSeqs)
		{
		  if ((*gapsV)[j][i].unknown) 
		    {
		      (*scores)[j].push_back(unknownchar[0]);
		    }
		  else if ((*gapsV)[j][i].sim == -1)
		    {
		      (*scores)[j].push_back('0');
		    }
		  else
		    {
		      (*scores)[j].push_back('1');
		    }
		  add = ((*gapsV)[j][i].stop > add) ? (*gapsV)[j][i].stop : add;
		  ++j;
		}
	    }
	}
      i=add+1;
    }
}



void utils::findPartitionOld (std::vector<gaps_t> *gapsV, 
			      const unsigned int alnLength,
			      const unsigned int noOfSeqs,
			      //std::vector< std::multimap<int,int> > *partitions,
			      std::vector< std::vector< std::vector<int> > > *partitions,
			      std::vector< std::vector<char> > *scores, 
			      std::map<unsigned int, short> *statistic,
			      const unsigned int minSize,
			      const unsigned int maxSize, 
			      const std::string unknownchar)
{
  //columnwise check if column defines partition
  unsigned int i = 0;
  unsigned int k = 0;
  int add = 0;
  std::vector<int> interesting;
  while (i < alnLength)
    {
      //      cout << "begin i: " << i << endl;
      add = i;
      std::vector< std::vector<int> > tmpVV;
      (*partitions).push_back(tmpVV);
      interesting.push_back(i);

      for (unsigned int j = 0 ; j < noOfSeqs ; ++j)
	{
	  std::vector<int> tmpV;
	  tmpV.push_back((*gapsV)[j][i].sim); //0
	  tmpV.push_back(j);               //1
	  tmpV.push_back((*gapsV)[j][i].stop - (*gapsV)[j][i].start + 1); //2
	  tmpV.push_back((*gapsV)[j][i].unknown); //3
	  //	  tmpV.push_back((*gapsV)[j][i].start); //4
	  (*partitions)[k].push_back(tmpV);
	  //cout << "gapsV[j][i].stop > add => "  << gapsV[j][i].stop << " vs. " << add << endl;
	  add = ((*gapsV)[j][i].stop > add) ? (*gapsV)[j][i].stop : add;
	}
      i=add+1;
      ++k;
    }

  
  (*gapsV).clear();
  //  cerr << "cleared" << endl;
  //  cerr << "was: " << alnLength << " is: " << partitions->size() 
  //       << " should be: " << interesting.size() << endl; 
  //  cerr << flush;

  unsigned int parts = 0;
  //  i = 0;
  //  while (i < interesting.size())
  //    {
  //      
  //      ++i;
  //    }
  std::set<int> keys;
  std::set<int> sizes;
  for (unsigned int i = 0; i < partitions->size(); ++i)
    {
      keys = getDistinctKeysVV((*partitions)[i],0); //
      sizes = getDistinctKeysVV((*partitions)[i],2); //
      std::set<int>::iterator it;
      bool in = false;
      int max = 0;
      for (it = sizes.begin() ; it != sizes.end() ; ++it)
	max = (*it > max) ? *it : max;

      if ((unsigned) max <= maxSize && (unsigned) max >= minSize)
	{
	  in = true;
	}
      else 
	{
	  in = false;
	}

      parts = (int) keys.size();
      //cout << "noOfdistinctKeys: " << (int) keys.size() << "\t";
      //method to test size of partition
      if (parts == 2 && utils::parsimonyInformative((*partitions)[i], keys) && in) //
	{ //cout << "max " << max << endl; 
	  
	  if ( (*statistic).count(max) > 0 )
	    (*statistic)[max] += 1;
	  else
	    (*statistic)[max] = 1;

	  for (unsigned int j = 0 ; j < (*partitions)[i].size() ; ++j)
	    {
	      //cout << (*partitions)[i][j][2] << endl;
	      if ((*partitions)[i][j][3]) //
		{
		  (*scores)[(*partitions)[i][j][1]].push_back(unknownchar[0]); //
		  //(*scores)[(*partitions)[i][j][1]].push_back(utils::toString(unknownchar[0]));
		}
	      else if ((*partitions)[i][j][0] == -1)
		{
		  (*scores)[(*partitions)[i][j][1]].push_back('1'); //
		  //(*scores)[(*partitions)[i][j][1]].push_back("-");
		  //(*scores)[(*partitions)[i][j][1]].push_back("1");
		}
	      else
		{
		  (*scores)[(*partitions)[i][j][1]].push_back('0'); //
		  //(*scores)[(*partitions)[i][j][1]].push_back("_");
		  //(*scores)[(*partitions)[i][j][1]].push_back(utils::toString((*partitions)[i][j][4]));
		}
	    }
	}
    }
}



void utils::findGapNeighborhood (std::vector<gaps_t> *gapsV,
				 const int region, 
				 const unsigned int alnLength) 
{
  for (int i = 0; (unsigned) i < (*gapsV).size()-1 ; i++)
    {
      for (int j = 0; (unsigned) j < (*gapsV)[i].size() ; j++)
	{
	  if ((*gapsV)[i][j].stop != -1)
	    {
	      cout << (*gapsV)[i][j].stop;
	      int start = (j < region) ? 0 : j-region;
	      cout << " j:" << j << " region:" << region << " start " << start;
	      int stop =  ((unsigned) j+region > (*gapsV)[i].size()-1) ? (*gapsV)[i].size()-1 : j+region;
	      cout << " stop " << stop << endl;
	      int k = i+1;
	    loop:
	      while ((unsigned) k < (*gapsV).size())
		{
		  for (int l = start ; l <= stop ; ++l)
		    {
		      if ((*gapsV)[k][l].sim != -1)
			continue;
		      if (j == l)
			{
			  if ((*gapsV)[i][j].stop == (*gapsV)[k][l].stop)
			    {
			      cout << "identical" << endl;
			      (*gapsV)[k][l].sim = i;
			      k+=(region+1);
			      goto loop;
			    }
			  else if (abs((*gapsV)[i][j].stop - (*gapsV)[k][l].stop) <= region)
			    {
			      cout << "similar1" << endl;
			      (*gapsV)[k][l].sim = i;
			      k+=(region+1);
			      goto loop;
			    }
			  else
			    {
			      cout << "not similar" << endl;
			    }
			}
		      else if (abs(j-k) <= region)
			{
			  int diff = region - abs(j-k);
			  if ((*gapsV)[i][j].stop == (*gapsV)[k][l].stop)
			    {
			      cout << "similar2" << endl;
			      k+=(region+1);
			      goto loop;
			    }
			  else if (abs((*gapsV)[i][j].stop - (*gapsV)[k][l].stop) <= diff
				   && 
				   (*gapsV)[i][j].stop >= l
				   &&
				   (*gapsV)[k][l].stop >= j)
			    {
			      cout << "similar3" << endl;
			      k+=(region+1);
			      goto loop;
			    }
			}
		      else 
			{
			  cout << "no" << endl;
			}
		    }
		  ++k;
		}
	    }
	}
    }
}


void utils::printGaps(const std::vector<gaps_t> &gapsV)
{
  cout << endl;
  for (unsigned int i = 0; i < gapsV.size() ; i++)
    {
      for (unsigned int j = 0; j < gapsV[i].size() ; j++)
	{
	  if (gapsV[i][j].start == -1 && gapsV[i][j].stop == -1)
	    {
	      cout << gapsV[i][j].start << "(" << gapsV[i][j].sim << ") ";
	    } 
	  else if (gapsV[i][j].start != -1 && gapsV[i][j].stop != -1)
	    {
	      cout << gapsV[i][j].start << "-" << gapsV[i][j].stop 
		   << "(" << gapsV[i][j].sim << ") ";
	    }
	  else
	    {
	      cout << "OOOooops:" << gapsV[i][j].start << "-" << gapsV[i][j].stop << "<<<" << endl;
	    }

	  //	  if (gapsV[i][j].stop != -1)
	  //	    {
	  //	      cout << "left gap: " << utils::findLeftGap(gapsV[i], j);
	  //	      cout << " right gap: " << utils::findRightGap(gapsV[i], j) << " ";
	  //	    }
	}
      cout << endl;
    }
}


void utils::updateIdenticalGaps(std::vector<gaps_t> *gapsV, 
				const unsigned int alnLength)
{
  for (unsigned int i = 0; i < (*gapsV).size()-1 ; i++)
    {
      for (unsigned int j = 0; j < alnLength ; j++)
	{
	  (*gapsV)[i][j].sim = -1;
	}
    }

  utils::findIdenticalGaps(gapsV, alnLength);


}


int utils::findLeftGap(const std::vector<gap_t> &currSeq, 
		       const unsigned int currPos)
{
  int leftGap = -1;

  for (int i = currPos-1 ; i >= 0 ; --i)
    {
      if (currSeq[i].stop != -1)
	{
	  leftGap = i;
	  break;
	}
    }
  return leftGap;
}


int utils::findRightGap(const std::vector<gap_t> &currSeq, 
			const unsigned int currPos)
{
  int rightGap = -1;

  for (unsigned int i = currSeq[currPos].stop + 1 ; i < currSeq.size() ; ++i)
    {
      if (currSeq[i].stop != -1)
	{
	  rightGap = i;
	  break;
	}
    }
  return rightGap;
}

bool utils::parsimonyInformative(const std::vector< std::vector< int > > &VV, 
				 const std::set<int> &keys)
{
  if ((int) keys.size() < 2)
    return false;

  std::map<int, int> m;
  
  for (unsigned int i = 0 ; i < VV.size() ; ++i)
    {
      m[VV[i][0]]++;
    }

  std::map<int, int>::const_iterator it;
  unsigned int count = 0;
  for (it=m.begin(); it != m.end(); ++it) 
    {
      //cerr << it->second << " " << it->first << endl;
      if (it->second >= 2)
	++count;
      if (count >= 2)
	return true;
    }
  return false;
}

bool utils::isTrueBipartition (std::vector<gaps_t> *gapsV,
			       const int seqPos,
			       const int siteId)
{
  
  //cout << (*gapsV).size() << endl;
  //cout << (*gapsV)[siteId].size() << endl;
   
  for (int i = 0 ; i < seqPos ; ++i)
    {
      //cout << (*gapsV)[i][siteId].start << " vs. " << (*gapsV)[seqPos][siteId].start << endl;
      if ( ( (*gapsV)[seqPos][siteId].start == siteId
	     &&
	     (*gapsV)[i][siteId].start == (*gapsV)[seqPos][siteId].start
	     &&
	     (*gapsV)[i][siteId].stop == (*gapsV)[seqPos][siteId].stop )
	   || 
	   ( (*gapsV)[i][siteId].start == -1
	     &&
	     (*gapsV)[i][siteId].stop == -1 )
	   )
	{continue;}
      else
	return false;
    }

  for (unsigned int i = seqPos+1 ; i < (*gapsV).size() ; ++i)
    {
      if ( ( (*gapsV)[seqPos][siteId].start == siteId
	     &&
	     (*gapsV)[i][siteId].start == (*gapsV)[seqPos][siteId].start
	     &&
	     (*gapsV)[i][siteId].stop == (*gapsV)[seqPos][siteId].stop )
	   || 
	   ( (*gapsV)[i][siteId].start == -1
	     &&
	     (*gapsV)[i][siteId].stop == -1 )
	   )
	{continue;}
      else
	return false;
    }
      
  //cout << "true:" << siteId << endl;
  return true;
}


void utils::demarkColumn (std::vector<gaps_t> *gapsV,
			  const unsigned int siteId)
{
  for (unsigned int i = 0 ; i < (*gapsV).size() ; ++i)
    {
      (*gapsV)[i][siteId].start = -1;
      (*gapsV)[i][siteId].stop = -1;
      (*gapsV)[i][siteId].sim = -1;
    }
}

void utils::demarkPosition (std::vector<gaps_t> *gapsV,
			    const unsigned int seqId,
			    const unsigned int siteId)
{
  (*gapsV)[seqId][siteId].start = -1;
  (*gapsV)[seqId][siteId].stop = -1;
  (*gapsV)[seqId][siteId].sim = -1;
}

void utils::markPosition (std::vector<gaps_t> *gapsV, const unsigned int seqId,
			  const unsigned int siteId, const int start, 
			  const int stop, const int sim)
{
  (*gapsV)[seqId][siteId].start = start;
  (*gapsV)[seqId][siteId].stop = stop;
  (*gapsV)[seqId][siteId].sim = sim;
}


void utils::checkIndels (std::vector<gaps_t> *gapsV, const unsigned int alnLength,
			 const unsigned minSize, const unsigned maxSize, 
			 const bool fuzzy)
{
  int start;
  int stop;
  
  unsigned int j = 0;
  while (j < alnLength)
    {
      start = (*gapsV)[0][j].start;
      stop = (*gapsV)[0][j].stop;

      int currCol = j;
      unsigned int currRow = 0;

      //find indel region
      while (currRow < (*gapsV).size())
	{
	  if ((*gapsV)[currRow][currCol].start == -1)
	    {
	      ++currRow;
	      continue;
	    }
	  if ((*gapsV)[currRow][currCol].stop > stop)
	    {
	      stop = (*gapsV)[currRow][currCol].stop;
	    }
	  if ( (start == -1 && (*gapsV)[currRow][currCol].start > start) 
	       || 
	       ( (*gapsV)[currRow][currCol].start != -1 
		 && 
		 (*gapsV)[currRow][currCol].start < start ) )
	    {
	      start = (*gapsV)[currRow][currCol].start;
	      currCol = start;
	      currRow = -1;
	    }
	  ++currRow;
	}

      if (start != -1)
	{
	  for (int k = start ; k <= stop ; ++k)
	    {
	      for (unsigned int i = 0 ; i < (*gapsV).size() ; ++i)
		{
		  stop = ((*gapsV)[i][k].stop > stop ) ? (*gapsV)[i][k].stop : stop ;
		  if ((*gapsV)[i][k].start != -1)
		    start = ((*gapsV)[i][k].start < start || start == -1) ? (*gapsV)[i][k].start : start ;
		}
	    }
	}
      j = (stop == -1) ? j+1 : stop+1;
    }
}


void utils::checkIndels2 (std::vector<gaps_t> *gapsV, const unsigned int alnLength,
			  const unsigned minSize, const unsigned maxSize, 
			  const bool fuzzy)
{
  int start;
  int stop;
  
  unsigned int j = 0;
  while (j < alnLength)
    {//cout << ">>J: " << j << endl;
      //utils::StatusBar(j, alnLength-1, 20);
      start = (*gapsV)[0][j].start;
      stop = (*gapsV)[0][j].stop;

      int currCol = j;
      unsigned int currRow = 0;

      // find indel region with maximal expansion
      while (currRow < (*gapsV).size())
	{
	  if ((*gapsV)[currRow][currCol].start == -1)
	    {
	      ++currRow;
	      continue;
	    }
	  if ((*gapsV)[currRow][currCol].stop > stop)
	    {
	      stop = (*gapsV)[currRow][currCol].stop;
	    }
	  if ( (start == -1 && (*gapsV)[currRow][currCol].start > start) 
	       || 
	       ( (*gapsV)[currRow][currCol].start != -1 
		 && 
		 (*gapsV)[currRow][currCol].start < start ) ) //<- useless?
	    {
	      start = (*gapsV)[currRow][currCol].start;
	      currCol = start;
	      currRow = -1;
	    }
	  ++currRow;
	}

      //cout << "1(a) START " << start <<  " STOP " << stop << endl;


      // find most left and most right border of all indels in region
      if (start != -1 && stop != -1)
	{
	  for (int k = start ; k <= stop ; ++k)
	    {
	      for (unsigned int i = 0 ; i < (*gapsV).size() ; ++i)
		{
		  stop = ((*gapsV)[i][k].stop > stop ) ? (*gapsV)[i][k].stop : stop ;
		  if ((*gapsV)[i][k].start != -1)
		    start = ((*gapsV)[i][k].start < start || start == -1) ? (*gapsV)[i][k].start : start ;
		}
	    }
	}

      //cout << "1(b) START " << start <<  " STOP " << stop << endl;

      //if valid window
      if ( start != -1 && stop != -1 )
	{
	  gaps_t diffGaps;
	  gaps_t usableIndels;
	  //int sameIndel = 0;
	  //int gapsOverall = 0;
	  //int gstart = 0;
	  //int gstop = 0;
	  bool only = false;

	  //collect all indels in region
	  for (unsigned int i = 0 ; i < (*gapsV).size() ; ++i)
	    {
	      int k = start;
	      while (k <= stop)
		{ 
		  if ( (*gapsV)[i][k].start == -1 
		       &&
		       (*gapsV)[i][k].stop == -1 ) //no indel
		    {
		      ++k;
		      continue;
		    }

		  bool in = false;
		  gaps_t::iterator it;
		  // save indel set
		  for (it = diffGaps.begin(); it != diffGaps.end(); ++it)
		    {
		      if (it->start == (*gapsV)[i][k].start 
			  &&
			  it->stop == (*gapsV)[i][k].stop) //same indel
			{
			  it->count += 1;
			  in = true;
			  break;
			}
		    }
		  if (!in) // not same indel -> create
		    {
		      gap_t tmp;
		      tmp.start = (*gapsV)[i][k].start;
		      tmp.stop = (*gapsV)[i][k].stop;
		      tmp.count = 1;
		      tmp.sim = i; //important if only one indel
		      if ((*gapsV)[i][k].unknown)
			tmp.unknown = true;
		      else
			tmp.unknown = false;
		      
		      diffGaps.push_back(tmp);
		    }
		  k = (*gapsV)[i][k].stop + 1;
		}
	    }

	  if (diffGaps.size() == 1)
	    only = true;

	  gaps_t::iterator iter;
	  int count = 0;
	  int nStart;
	  int nStop;
	  bool nothingElse = true;

	  //	  for (iter = diffGaps.begin() ; iter < diffGaps.end(); iter++)
	  //	    {
	  //	      cout << "WAS >>> " << iter->start << " "
	  //		   << iter->stop << " "
	  //		   << iter->count << " <<<\n";
	  //	    }

	  //for all different indels
	  while (diffGaps.begin() != diffGaps.end())
	    { //find highest count
	      //and delete uninformative
	      for (iter = diffGaps.begin(); iter != diffGaps.end(); ++iter)
		{ //if only one indel [allow gaps here (start = stop)]
		  if (iter->count == 1)
		    {
		      for (int m = iter->start; m <= iter->stop; ++m)
			{ //cout << "demarking " << iter->sim << " " << m << endl;
			  demarkPosition(gapsV, iter->sim, m);
			}
		      
		      only = true;
		      //		      cout << "erase: " << iter->start << " "
		      //			   << iter->stop << " "
		      //			   << iter->count << endl;

		      iter = diffGaps.erase(iter);
		      --iter;
		    }
		  else if (iter->count > count && (iter->start != iter->stop)) //if more than one indel //
		    {
		      //		      cout << count << endl;
		      nStart = iter->start;
		      nStop = iter->stop;
		      count = iter->count;
		      nothingElse = false;
		      //		      cout << count << endl;
		    }
		  }
		  


	      //if nothing found, except singles
	      if (nStart == -1 && nStop == -1)
		{
		  for (iter = diffGaps.begin(); iter != diffGaps.end(); ++iter)
		    {
		      if (iter->count > count) //if more than one indel
			{
			  nStart = iter->start;
			  nStop = iter->stop;
			  count = iter->count;
			  nothingElse = true;
			}  
		    }
		}

	      //	      for (iter = diffGaps.begin() ; iter < diffGaps.end(); iter++)
	      //		{
	      //		  cout << "IS >>> " << iter->start << " "
	      //		       << iter->stop << " " << iter->count << " <<<\n";
	      //		}

	      //find left most indel with highest count
	      //any indels left
	      if (diffGaps.begin() != diffGaps.end())
		{
		  for (iter = diffGaps.begin() ; iter != diffGaps.end() ; ++iter)
		    {
		      if (iter->count == count)
			{
			  if (iter->start < nStart)
			    {
			      nStart = iter->start;
			      nStop = iter->stop;
			    }
			}
		    }
		  
		  bool conflict = false;
		  int mleft = -1;
		  int mright = -1;
		  //cout << "reset mleft and mright to -1" << endl;
		  //overlapping indels with more than one member?
		  for (iter = diffGaps.begin(); iter != diffGaps.end(); ++iter)
		    {
		      //		      cout << nStart << " " << nStop << " vs "
		      //			   << iter->start << " " << iter->stop << endl;
		      if ( (iter->stop <= nStop && iter->stop >= nStart)
			   || 
			   (iter->start >= nStart && iter->start <= nStop)
			   ||
			   (iter->start <= nStart && iter->stop >= nStop) )
			{
			  if ( iter->start == iter->stop && fuzzy)
			    { //cout << "no conflict";
			      for (unsigned int m = 0 ; m < (*gapsV).size() ; ++m)
				if ( (*gapsV)[m][iter->start].start == iter->start
				     &&
				     (*gapsV)[m][iter->stop].stop == iter->stop 
				     &&
				     !nothingElse)
				  {
				    //cout << "demarking " << m << " " << iter->start << endl;
				    demarkPosition(gapsV,m,iter->start);
				  }
			      //			      mleft = iter->start;
			      //			      mright = iter->stop;
			      //			      cout << " mleft:" << mleft;
			      //			      cout << " mright:" << mright << endl ;
				
			    }
			  else if ( iter->start == iter->stop && !fuzzy && !only)
			    { //cout << "conflict1" << endl;
			      conflict = true;
			      
			      //mleft = (iter->start < nStart) ? iter->start : nStart;
			      if (mleft == -1)
				mleft = (iter->start < nStart) ? iter->start : nStart;
			      else 
				{
				  mleft = (mleft < nStart) ? mleft : nStart;
				  mleft = (mleft < iter->start) ? mleft : iter->start;
				}

			      //mright = (iter->stop > nStop) ? iter->stop : nStop;
			      if (mright == -1)
				mright = (iter->stop > nStop) ? iter->stop : nStop;
			      else
				{
				  mright = (mright > nStop) ? mright : nStop;
				  mright = (mright > iter->stop) ? mright : iter->stop;
				}
			    }
			  else if ( iter->stop != nStop 
				    ||
				    iter->start != nStart 
				    || 
				    iter->count != count) //OR allow gaps here
			    { //cout << "conflict2";
			      conflict = true;
			      
			      //mleft = (iter->start < nStart) ? iter->start : nStart;
			      if (mleft == -1)
				mleft = (iter->start < nStart) ? iter->start : nStart;
			      else 
				{
				  mleft = (mleft < nStart) ? mleft : nStart;
				  mleft = (mleft < iter->start) ? mleft : iter->start;
				}
			      //cout << " mleft:" << mleft;
			      
			      //mright = (iter->stop > nStop) ? iter->stop : nStop;
			      if (mright == -1)
				mright = (iter->stop > nStop) ? iter->stop : nStop;
			      else
				{
				  mright = (mright > nStop) ? mright : nStop;
				  mright = (mright > iter->stop) ? mright : iter->stop;
				}
			      //cout << " mright:" << mright << endl;
			    }
			  else
			    {
			      //cout << "else" << endl;
			    }

			  //			  cout << "erase: " << iter->start << " "
			  //			       << iter->stop << " "
			  //			       << iter->count << endl;
			  
			  iter = diffGaps.erase(iter);
			  --iter;
			}
		    }
		  
		  if (conflict || nStart == -1)
		    { //cout << "cannot use this locus: " << mleft << " " << mright << endl;
		      for (int l = mleft ; l <= mright ; ++l)
			demarkColumn(gapsV,l);
		    }
		  else
		    { //cout << "CAN use this locus: " << nStart << " " << nStop << " " << count << endl;
		      gap_t tmp;
		      tmp.start = nStart;
		      tmp.stop = nStop;
		      tmp.count = count;
		      usableIndels.push_back(tmp);
		    }
		}
	      else if (count == 0 && nStart == -1 && nStop ==-1 )
		{
		  gap_t tmp;
		  tmp.start = nStart;
		  tmp.stop = nStop;
		  tmp.count = count;
		  usableIndels.push_back(tmp);
		}
	      else
		{}
	      
	      count = 0;
	      nStart = -1;
	      nStop = -1;
	    } //end while
	  
	  //	  gaps_t::iterator uI;
	  //	  for (uI = usableIndels.begin() ; uI < usableIndels.end(); uI++)
	  //	    {
	  //	      cout << "USABLE >>> " << uI->start << " "
	  //	      	   << uI->stop << " " << uI->count << " <<<\n";
	  //	    }
	} // end if
      j = (stop == -1) ? j+1 : stop+1;

    } // end while
  
}


void utils::StatusBar(unsigned int cur, unsigned int max,
		      unsigned int size) 
{
  
  double level = double(cur)/double(max);
  /*  unsigned int numdots = utils::round2int(level * size);
      unsigned int numblanks = size - numdots;
      cerr << " [";

      unsigned int i;
      for (i = 0; i < numdots; i++) {
      cerr << "=";
      }
  
      for(i = 0; i < numblanks; i++) {
      cerr << " ";
      }
  
      cerr << "]";

      cerr << " (" << cur << "/" << max << setprecision(5) << ", " << utils::round2int(level*10000)/100 << "%)";
      } */
  fprintf(stderr, "%3.0f%% ", level*100);
  cerr << "\r";
}


std::vector<std::string> utils::split (const std::string &seq, 
				       unsigned int length)
{
  std::vector<std::string> tmp;
  string tmpString = seq;
  while(tmpString.size() > length)
  {
    tmp.push_back(tmpString.substr(0, length));
    tmpString = tmpString.substr(length);
  }
  tmp.push_back(tmpString);
  return tmp;
}
