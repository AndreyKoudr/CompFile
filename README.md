# CompFile

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

  Composite file structure
  ------------------------
  
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

console shows how to extract entries from a composite file

e.g.
  > CompFileTest RADAR_FILES RADAR.CMP

where RADAR_FILES is file header; RADAR.CMP is a composite file.

  This example opens a composite file, extracts all its entries and saves 
each in a separate file.

  <B>The essense</B> of the example is in how to read composite file 
entry data into memory.
