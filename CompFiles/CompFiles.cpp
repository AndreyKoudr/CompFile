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
//#include <filesystem>
#include <algorithm> 
#include <limits>
#include "../CompFile.h"

#define NOMINMAX
#include <windows.h>


using namespace std;

void wrongExit(string message)
{
  cout << message << endl << endl;
  cout << ">CompFiles file_header file_mask out_file" << endl;
  cout << "compiles all files found by file_mask to composite file according to the following format:" << endl;
  cout << "(1) header specified - header string (chars without length byte)" << endl;
  cout << "(2) total number of files compiled (2 bytes)" << endl;
  cout << "(3) header section, containing position + length pairs (each 4 + 4 bytes)" << endl;
  cout << "(4) files one by one (sorted by name)" << endl;

  cout << endl << endl << "  Press ENTER..." << endl;
  getchar();
}

// Parse file directory from string
static std::string getFileDirectory(const std::string &s)
{
  std::size_t pos = s.rfind('\\');
	if (pos != std::string::npos)
  {
		return s.substr(0,pos);
	} else
	{
    std::size_t pos = s.rfind('/');
	  if (pos != std::string::npos)
    {
		  return s.substr(0,pos);
    } else
    {
		  return std::string("");
    }
	}
}

// Add backslash to file name
static std::string addBackslash(const std::string &directory)
{
  if (directory.length() == 0)
  {
    return std::string("");
  } else 
  {
    if (directory.back() != '\\' && directory.back() != '/')
    {
      return directory + "\\";
    } else
    {
      return directory;
    }
  }
}

// Get file list by mask, each file with full or just file name
void getFileList(const char *mask, bool fullpath, bool onlyfiles, std::vector<std::string> &list)
{
#if 0

  // error C2039: 'directory_iterator': is not a member of 'std::experimental::filesystem'
  namespace fs = experimental::filesystem;
  std::string directory = getFileDirectory(std::string(mask));

  for (auto &p : fs::directory_iterator(directory))
  {
  }

#else

	HANDLE hFind;
	BOOL bContinue = true;
	WIN32_FIND_DATAA data;
	std::string s;
  std::string directory = getFileDirectory(std::string(mask));

	hFind = FindFirstFileA(mask,&data); 

  if ((hFind != NULL) && ((int) hFind != -1))
  {
	  while (hFind && bContinue) 
	  {
		  if (onlyfiles ? (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) : true)
		  {
				s = data.cFileName;
        if (fullpath) 
          s = addBackslash(directory) + s;
        list.push_back(s);
		  } 
		  bContinue = FindNextFileA(hFind,&data);
	  }
	  FindClose(hFind);
  }
#endif
}

bool comp_func(const string& s1, const string& s2)
{
  return s1 < s2;
}

/**
  Compiler
  --------
  VS 2019 is the compiler for the whole project.

  The base class CompFile does not have anything Windows-specific and can be
used in Linux with no or minimum problems. 

  However, this console compiles in Windows only because after hard 10 years work 
on C++ standartisation there is still no standard way to list files in a 
directory (see my efforts in getFileList()).

  Console project CompFiles to generate a composite file
  ------------------------------------------------------
with a header from all files specified by file mask like this :

>CompFiles RADAR_FILES TestFiles/*.* RADAR.CMP

  It takes all files from TestFiles folder, compiles them all into a
composite file RADAR.CMP and saves it with header RADAR_FILES.

  Test console project CompFileTest
  ---------------------------------
gives an example how to extract data from a composite file.
*/
int main(int argc, char* argv[])
{
                              // test # command line arguments
  if (argc != 4)
  {
    wrongExit("Wrong number of command line parameters");
    return 1;
  }
                              // get arguments
  string header = argv[1];
  string filemask = argv[2];
  string outfile = argv[3];
                              // get files
  vector<string> list;
  getFileList(filemask.c_str(),false,true,list);
                              // directory
  string directory = getFileDirectory(filemask);

  if (list.size() == 0)
  {
    wrongExit("No such files found");
    return 1;
  };
                              // sort file name list
  sort(list.begin(),list.end(),comp_func);
                              // open output
  ofstream file(outfile,std::ofstream::binary);
  if (!file.is_open())
  {
    wrongExit("Unable to write to file " + outfile);
    return 1;
  }
                                // write header
  file.write(header.c_str(),header.size());
                                // number of files
  unsigned short int n = static_cast<unsigned short int>(list.size());
  file.write((char *) &n,sizeof(n));
                                // directory
  long dirpos = file.tellp();
  size_t dirsize = list.size() * sizeof(PosLen);
  vector<PosLen> dir(list.size());
  file.write((char *) &list[0],dirsize);
                                // files
  for (int i = 0; i < list.size(); i++)
  {
    string name = list[i];

    ifstream f(addBackslash(directory) + name,std::ios::in | std::ios::binary);
                                // get file size
    f.ignore(numeric_limits<streamsize>::max());
    std::streamsize size = f.gcount();
    f.clear();                  //  since ignore will have set eof
                                // go to start
    f.seekg(0, std::ios_base::beg);
                                // report 
    cout << "Entry number " << i << ", file " << name << " of size " << size << endl;

    char *ptr = new (nothrow) char[size];
    if (ptr == nullptr)
    {
      wrongExit("Unable to allocate memory when reading file " + name);
      f.close();
      return 1;
    }

    f.read(ptr,size);
    dir[i].pos = file.tellp();
    file.write(ptr,size);
    dir[i].len = size;

    delete[] ptr;

    f.close();
  }
                                // rewrite directory
  file.seekp(dirpos);
  file.write((char *) &dir[0],dirsize);
                                // done
  file.close();

  cout << endl << list.size() << " files saved" << endl;

  cout << endl << endl << "  Press ENTER..." << endl;
  getchar();

	return 0;
}

