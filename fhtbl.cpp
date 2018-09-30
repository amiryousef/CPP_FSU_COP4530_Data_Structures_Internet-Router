/*
    fthtbl.cpp
    03/23/2012
    Chris Lacher
    Copyright 2011, R.C. Lacher

    functionality test for hash tables

    THashTable <K,D,H>

       K =      String
       D =      int
       H =      THash<K>
*/

#include <fstream>
#include <cctype>

#include <xstring.h>
#include <hashclasses.h>
#include <primes.h>
#include <bitvect.h>

#include <hashtbl.h>
#include <compare.h>

// in lieu of makefile
#include <xstring.cpp>
#include <primes.cpp>
#include <bitvect.cpp>
#include <hashfunctions.cpp>
// */

void DisplayMenu(std::ostream& os = std::cout);
void DisplayPrompt(const char* kt, const char* dt, const char* ht);

// the KISS hash function
typedef fsu::String                        KeyType;
typedef int                                DataType;
typedef hashclass::KISS < KeyType >        HashType;
typedef fsu::Entry < KeyType, DataType >   EntryType;
typedef fsu::LessThan < EntryType >        ComparisonType;
const char* kT = "fsu::String";
const char* dT = "int";
const char* hT = "hashclass::KISS <>";
const bool PRIME = 1;
// */

/* // the MM hash function
typedef fsu::String                        KeyType;
typedef int                                DataType;
typedef hashclass::MM < KeyType >          HashType;
typedef fsu::Entry < KeyType, DataType >   EntryType;
typedef fsu::LessThan < EntryType >        ComparisonType;
const char* kT = "fsu::String";
const char* dT = "int";
const char* hT = "hashclass::MM <>";
const bool PRIME = 1;
// */

/* // the Simple hash function
typedef fsu::String                        KeyType;
typedef int                                DataType;
typedef hashclass::Simple < KeyType >      HashType;
typedef fsu::Entry < KeyType, DataType >   EntryType;
typedef fsu::LessThan < EntryType >        ComparisonType;
const char* kT = "fsu::String";
const char* dT = "int";
const char* hT = "hashclass::Simple <>";
const bool PRIME = 0;
// */

int main(int argc, char* argv[])
{
  typedef fsu::HashTable < KeyType, DataType, HashType > HashTableType;
  HashTableType* tablePtr;
  size_t numbuckets;

  fsu::String filename;
  std::ifstream in1;
  std::ofstream out1;
  KeyType  key;
  DataType data;
  HashTableType::Iterator htableItr;
  char command;
  // int cw1 = 3, cw2 = 3;
  std::ifstream ifs;
  std::istream * inptr = &std::cin;
  bool BATCH = 0;
  if (argc > 1)
  {
    BATCH = 1;
    ifs.open(argv[1]);
    if (ifs.fail()) return 0;
    inptr = &ifs;
  }
  std::cout << "  Enter approximate number of buckets (0 to quit): ";
  *inptr >> numbuckets;
  if (BATCH) std::cout << numbuckets << '\n';
  if (numbuckets == 0)
    return 0;

  // tablePtr = new HashTableType(numbuckets, PRIME); // 1-parameter constructor
  HashType hfo;
  tablePtr = new HashTableType(numbuckets, hfo, PRIME); // 2-parameter constructor

  if (!BATCH) DisplayMenu();

  do
  {
    DisplayPrompt(kT, dT, hT);
    *inptr >> command;
    if (BATCH) std::cout << command;
    switch (command)
    {
    case 'm': case 'M':
      if (BATCH) std::cout << '\n';
      DisplayMenu();
      break;

    case 'x': case 'X':
      if (BATCH)
      {
	std::cout << '\n';
	inptr = &std::cin;
	ifs.close();
	std::cout << "   ** switched to keyboard input **\n";
	BATCH = 0;
	DisplayMenu();
      }
      else
      {
	std::cout << "  ** already using keyboard input **\n";
      }
      break;

    case 'q': case 'Q':
      if (BATCH)
      {
	std::cout << '\n';
	ifs.close();
      }
      command = 'q';
      break;

    case 'P': case 'p':
      *inptr >> key >> data;
      if (BATCH) std::cout << key << ' ' << data << '\n';
      std::cout << "Put(" << key << ',' << data << ")\n";
      tablePtr->Put(key,data);
      // cw1 = CorrectColumnWidth(key,cw1);
      // cw2 = CorrectColumnWidth(data,cw2);
      break;

    case 'G': case 'g':
      *inptr >> key;
      if (BATCH) std::cout << key << '\n';
      std::cout << "  Get(" << key << ") = " << tablePtr->Get(key) << '\n';
      break;

    case '+':  case '1': // Insert(key, data)
      *inptr >> key;     // operand1
      *inptr >> data;   // operand2
      if (BATCH) std::cout << ' ' << key << ' ' << data << '\n';;
      tablePtr->Insert(key, data);
      std::cout << key << ':' << data
		<< " inserted into table\n";
      // cw1 = CorrectColumnWidth(key,cw1);
      // cw2 = CorrectColumnWidth(data,cw2);
      break;
        
    case '-': case '2':
      *inptr >> key;
      if (BATCH) std::cout << ' ' << key << '\n';
      if (tablePtr->Remove(key))
      {
	std::cout << "  key " << key << " removed from table\n";
      }
      else
      {
	std::cout << "  key " << key << " not found in table\n";
      }
      break;

    case 'L': case 'l':
      std::cout << "  Enter data file name (0 to abort): ";
      *inptr >> filename;
      if (BATCH) std::cout << filename << '\n';;
      if (filename.Element(0) == '0')
	break;
      in1.clear();
      in1.open(filename.Cstr());
      if (in1.fail())
      {
	std::cout << "  Unable to open file " << filename << '\n'
		  << "  Load() aborted\n";
	break;
      }
      while (!in1.eof())
      {
	in1 >> key >> data;
	tablePtr->Insert(key,data);
      }
      in1.close();
      std::cout << "  load completed\n";
      break;

    case 'F': case 'f':
      std::cout << "  Enter data file name (0 for screen): ";
      *inptr >> filename;
      if (BATCH) std::cout << filename << '\n';;
      if (filename.Element(0) == '0')
      {
	htableItr = tablePtr->Begin();
	while (htableItr != tablePtr->End())
	{
	  std::cout << (*htableItr).key_ << '\t' << (*htableItr).data_ << '\n';
	  ++htableItr;
	}
	break;
      }
      out1.clear();
      out1.open(filename.Cstr());
      if (out1.fail())
      {
	std::cout << "  Unable to open file " << filename << '\n'
		  << "  save aborted\n";
      }
      else
      {
	htableItr = tablePtr->Begin();
	while (htableItr.Valid())
	{
	  out1 << (*htableItr).key_ << '\t' << (*htableItr).data_ << '\n';
	  ++htableItr;
	}
	out1.close();
	std::cout << "  data saved to file " << filename << '\n';
      }
      break;

    case 'd':  case 'D':
      if (BATCH) std::cout << '\n';
      std::cout << "  Enter data file name (0 for screen): ";
      *inptr >> filename;
      if (BATCH) std::cout << filename << '\n';;
      if (filename.Element(0) == '0')
      {
	tablePtr->Dump(std::cout);
	break;
      }
      out1.clear();
      out1.open(filename.Cstr());
      if (out1.fail())
      {
	std::cout << "  Unable to open file " << filename << '\n'
		  << "  Dump() aborted\n";
	break;
      }
      tablePtr->Dump(out1);
      out1.close();
      std::cout << "  Dump() completed\n";
      break;

    case 'o':  case 'O':
      if (BATCH) std::cout << '\n';
      std::cout << "  Enter data file name (0 for screen): ";
      *inptr >> filename;
      if (BATCH) std::cout << filename << '\n';;
      if (filename.Element(0) == '0')
      {
	for (htableItr = tablePtr->Begin(); htableItr != tablePtr->End(); ++htableItr)
	  std::cout << (*htableItr).key_ << '\t' << (*htableItr).data_ << '\n';
	break;
      }
      out1.clear();
      out1.open(filename.Cstr());
      if (out1.fail())
      {
	std::cout << "  Unable to open file " << filename << '\n'
		  << "  Display() aborted\n";
	break;
      }
      for (htableItr = tablePtr->Begin(); htableItr != tablePtr->End(); ++htableItr)
	out1 << (*htableItr).key_ << '\t' << (*htableItr).data_ << '\n';
      out1.close();
      std::cout << "  Display() completed\n";
      break;

    case 'i':  case 'I':
      *inptr >> key;
      if (BATCH) std::cout << ' ' << key << '\n';
      std::cout << " Includes(key): ";
      htableItr = tablePtr->Includes(key);
      if (htableItr != tablePtr->End())
	std::cout << "entry " << (*htableItr).key_ << ':'
		  << (*htableItr).data_ << " found\n";
      else
	std::cout << "key " << key << " not found\n";
      break;

    case 'r':  case 'R':
      *inptr >> key;
      if (BATCH) std::cout << ' ' << key << '\n';
      std::cout << " Retrieve(key,data): ";
      if (tablePtr->Retrieve(key,data))
	std::cout << "data " << data << " retrieved for key " << key << '\n';
      else
	std::cout << "key " << key << " not found\n";
      break;

    case 'e': case 'E':
      if (BATCH) std::cout << '\n';
      std::cout << "  table is ";
      if (!tablePtr->Empty()) std::cout << "not";
      std::cout << " empty\n";
      break;

    case 's': case 'S':
      if (BATCH) std::cout << '\n';
      std::cout << "  table size:      " << tablePtr->Size() << '\n';
      // std::cout << "  max bucket size: " << tablePtr->MaxBucketSize() << '\n';
      break;

    /*
    case 'a': case 'A':
      tablePtr->Analysis(std::cout);
      break;
    */

    case 'c': case 'C':
      if (BATCH) std::cout << '\n';
      tablePtr->Clear();
      break;

    case 'h': case 'H':
      *inptr >> numbuckets;
      if (BATCH) std::cout << numbuckets << '\n';
      tablePtr->Rehash(numbuckets);
      break;

    case 't': case 'T':
      std::cout << "Enter a key:  ";
      *inptr >> key  ;
      if (BATCH) std::cout << key << '\n';
      std::cout << '\n';
      std::cout << "Testing Begin() to End() ++I\n";
      for (htableItr = tablePtr->Begin(); htableItr != tablePtr->End(); ++htableItr)
	std::cout << *htableItr << ',';
      std::cout << "\n\n";
      
      std::cout << "Testing Begin() to End() I++\n";
      for (htableItr = tablePtr->Begin(); !(htableItr == tablePtr->End()); htableItr++)
	std::cout << *htableItr << ',';
      std::cout << "\n\n";

      std::cout << "Testing Includes(" << key << ") to End()\n";
      for (htableItr = tablePtr->Includes(key); htableItr != tablePtr->End(); ++htableItr)
	std::cout << *htableItr << ',';
      std::cout << "\n\n";
      break;

    default:
      if (BATCH) std::cout << '\n';
      std::cout << " ** undefined command (level 1)\n";

    } // end switch
  }
  while (command != 'q');
  delete tablePtr;
  return 1;
} // end main()

void DisplayMenu(std::ostream& os)
{
  os << " OPERATION                             ENTRY\n"
     << " ---------                             -----\n"
     << " Load data from file  ...............  L filename\n"
     << " save data to File  .................  F filename\n"
     << " x.Insert(key,data)  ................  + key data\n"
     << " x.Remove(key)  .....................  - key\n"
     << " x.Includes(key)  ...................  I key\n"
     << " x.Retrieve(key,&data)  .............  R key\n"
     << " x.Put(key,data)  ...................  P key data\n"
     << " x.Get(key)  ........................  G key\n"
     << " x.Clear()  .........................  C\n"
     << " x.Rehash(numBuckets)  ..............  H numBuckets\n"
     << " x.Size()  ..........................  S\n"
     // << " x.Analysis()  ......................  A\n"
     << " x.Empty()  .........................  E\n"
     << " x.Dump(filename)  ..................  D\n"
     << " os << x  ...........................  O\n"
     << " iTerator test  .....................  T\n"
     << " display Menu   .....................  M\n"
     << " switch from batch mode  ............  X\n"
     << " Quit program   .....................  Q\n"
     << std::endl;
}

void DisplayPrompt(const char* kt, const char* dt, const char* ht)
{
  std::cout << "THashTable < " << kt 
	    << " , " << dt
	    << " , " << ht
	    << " >\n"
	    << "Enter [command][arguments] ('M' for menu, 'Q' to quit): ";
}

