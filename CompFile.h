/**
Copyright 2020 Andrey Kudryavtsev (andrewkoudr@hotmail.com)

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appears in all copies and that both the
copyright notice and this permission notice appear in supporting
documentation, and that the same name not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission. 
We make no representations about the suitability of this software for any 
purpose. It is provided "as is" without any expressed or implied warranty.
*/

#include "defines.h"
#include "Types.h"

#include <vector>
#include <string>
#include <fstream>

using namespace std;

/**
  Operations on composite files
  -----------------------------

  Suppose you write a project which uses some data files, like bitmaps, 
geometries, tables etc. They are contained in separate files. All these files
are scattered over directories (like Qt's Resources).

  This makes the program look (actually be) unprofessional. Or, at least, it was 
considered to be so in the early days of programming. This is why :
  - every file can be lost (or stolen) during installation or illegal copying 
what makes the whole project not functional
  - every file (e.g. a bitmap) can be easily replaced by another one
  - opening multiple files is slow process

  Therefore Microsoft compiles resources into executables. This is right. But 
the resource data maybe huge and the executable would have a corresponding size.

  That is why it looks reasonable to place groups of files into a single file with
programmatic access to every entry in this composite file.

  Of course, you can install and use HDF5. It is fine, but you will have
to update it with every new version of it and new version of compiler you use.
Usual headache with third-party software.

  So, if you wish to have just a simple class to contain multiple files in it, 
you can use this class.

  Composite file is a set of files appended to each other and preceded
by header according to the following format :

  - header specified - header string (without length byte); this header is 
checked on opening
  - total number of files (entries) compiled in the files (2 bytes);
  - directory, containing position + length pairs (each 4 + 4 bytes) for every entry;
  - files one by one.

  A composite file can be generated with the CompFiles program.

  The unusual integer type for total number is used here to keep compatibility 
with my very old files generated long ago. You can easily change it knowing the
file structure.
*/

#pragma pack(push,1)

// directory contains position + length pairs for each entry in file
typedef struct PosLen
{ 
  int pos = 0;
  int len = 0;
} PosLen;

static_assert(sizeof(PosLen) == 8,"Composite file directory must contain 4 + 4 int pairs");

class CompFile : public ifstream
{
public:
                              // constructor
  CompFile();
                              // constructor
  CompFile(const string &fileName, const string &header);
                              // destructor
  ~CompFile();
                              // successfully opened?
  bool isOpen() const;
                              // close file
  void closeFile();
                              // number of entries
  int numEntries() const;
                              // seek entry position in file before reading len bytes
  bool setEntryPos(int entryNo, int &len);
                              // get file name
  const string &fileName() const;
                              // get header
  const string &header() const;
                              // report error string after a unsuccessful call
  const string &errorString() const;

private:
                              // must-be header; checked over opening file
  string header_;
                              // array with pos + len pairs
  vector<PosLen> poslen_;
                              // to keep file name
  string fileName_;
                              // open?
  bool opened_;
                              // error string
  string errorString_;
                              // read directory, call errorString() for reason in case of failure
  bool readDirectory();
                              // get position and length for an entry
  bool getEntryPosLen(int entryNo, int &pos, int &len);

};

#pragma pack(pop)