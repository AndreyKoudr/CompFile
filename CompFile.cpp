#include "CompFile.h"

CompFile::CompFile() : opened_(false)
{
}

CompFile::CompFile(const string &fileName, const string &header)
{
  opened_ = false;
                              // store
  fileName_ = fileName;
  header_ = header;

  open(fileName_,std::ifstream::in | std::ifstream::binary);

  if (is_open())
  {
    opened_ = readDirectory();
  } else
  {
    errorString_ = string("Unable to open file ") + fileName_;
  }
}

bool CompFile::isOpen() const
{
  return opened_;
}

bool CompFile::readDirectory()
{
                              // check header
  string buffer = header_;
  read(&buffer[0],header_.size());
  if (buffer == header_)
  {
                              // read # entries
    unsigned short int N = 0;
    read((char *) &N,2);
    if (!N) 
    {
                              // no entries in file
      errorString_ = "No entries in file";
      return false;
    } else
    {
      poslen_.clear();
      poslen_.resize(N);
                              // create pos+len array
      read((char *) &poslen_[0],N * sizeof(PosLen));
      return true;
    }
  } else
  {
                              // wrong header
    errorString_ = "Wrong file header";
    return false;
  }
}

const string &CompFile::errorString() const
{
  return errorString_;
}

void CompFile::closeFile()
{
  close();
  opened_ = false;
}

CompFile::~CompFile()
{
  if (opened_)  
    closeFile();
}

int CompFile::numEntries() const
{
  return static_cast<int>(poslen_.size());
}

bool CompFile::getEntryPosLen(int entryNo, int &pos, int &len)
{
  if (!opened_) return false;

  if (entryNo >= 0 && entryNo <= numEntries())
  {
    pos = poslen_[entryNo].pos;
    len = poslen_[entryNo].len;
    return true;
  }
  else
  {
    errorString_ = string("Composite file ") + fileName_ +
      string(", wrong entry number when loading entry ") + to_string(entryNo) +
      ": there are only " + to_string(numEntries()) + " entries in the file";
    return false;
  }
}

bool CompFile::setEntryPos(int entryNo, int& len)
{
  int pos;
  if (getEntryPosLen(entryNo,pos,len))
  {
    seekg(poslen_[entryNo].pos);
    return true;
  }
  else
  {
    return false;
  }
}

const string &CompFile::fileName() const
{
  return fileName_;
}

const string &CompFile::header() const
{
  return header_;
}



