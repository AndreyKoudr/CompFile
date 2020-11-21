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

#include <iostream>
#include <string>
#include "../CompFile.h"

using namespace std;

void wrongExit(string message)
{
  cout << message << endl << endl;
  cout << ">CompFileTest file_header composite_file" << endl;
  cout << "extracts all files from composite file (you must know its file header)" << endl;

  cout << endl << endl << "  Press ENTER..." << endl;
  getchar();
}

/**
  Test console project CompFileTest
  ---------------------------------

console shows how to extract entries from a composite file

e.g.
  > CompFileTest RADAR_FILES RADAR.CMP

where RADAR_FILES is file header; RADAR.CMP is a composite file.

  This example opens a composite file, extracts all its entries and saves 
each in a separate file.

  <B>The essense</B> of the example is in how to read composite file 
entry data into memory.
*/

int main(int argc, char* argv[])
{
                              // test # command line arguments
  if (argc != 3)
  {
    wrongExit("Wrong number of command line parameters");
    return 1;
  }

  string header = argv[1];
  string filename = argv[2];
                              // open composite file
  CompFile file(filename,header);
                              // open?
  if (file.isOpen())
  {
  } else
  {
    wrongExit(file.errorString());
    return 1;
  }
                              // here, for testing, we extract ALL entries and save them in files
  for (int i = 0; i < file.numEntries(); i++)
  {
    int size = 0;
                              // set (seek) composite file position for entry i together
                              // with entry size
    if (file.setEntryPos(i,size))
    {
      string name = "entry" + to_string(i) + ".bin";
      cout << "Reading/saving entry " << i << " of size " << size << " into flle " << name << endl;

      ofstream f(name,std::ios::out | std::ios::binary);
                              // allocate buffer of size to read data from composite file
      char *ptr = new (nothrow) char[size];
      if (ptr == nullptr)
      {
        wrongExit("Unable to allocate memory when reading entry " + to_string(i));
        f.close();
        return 1;
      }
                              // read entry data from composite file
      file.read(ptr,size);
      f.write(ptr,size);
      f.close();

      delete[] ptr;
    } else
    {
      wrongExit(file.errorString());
      return 1;
    }
  }

  cout << endl << endl << "  Press ENTER..." << endl;
  getchar();

	return 0;
}
