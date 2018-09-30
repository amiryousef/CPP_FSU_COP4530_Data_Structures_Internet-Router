/*
    iprouter.cpp
    03/26/2012

    simulating an Internet router
*/

#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cctype>

#include <xstring.h>
#include <iptable.h>

/* // in lieu of makefile
#include <xstring.cpp>
#include <bitvect.cpp>
#include <hash.cpp>
#include <primes.cpp>
#include <iptable.cpp>
// */

typedef fsu::String ipString;
const size_t maxFilenameSize = 21;

void DisplayMenu();

int main(int argc, char* argv[])
{
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

  unsigned int numBuckets;
  std::cout << "\nRouter control program started\n";
  std::cout << "  Enter approximate number of entries for RouteTable: ";
  *inptr >> numBuckets;
  if (BATCH) std::cout << numBuckets << '\n';
  if (numBuckets == 0)
    return 0;

  RouteTable routeTable (numBuckets);
  char file1 [maxFilenameSize], file2 [maxFilenameSize];
  char selection;

  ipString dS,   // destination (dot notation)
           rS;   // route       (dot notation)


  if (!BATCH) DisplayMenu();
  do
  {
    std::cout << "Enter Router Command ('M' for menu, 'Q' to quit): ";
    *inptr >> selection;
    if (BATCH) std::cout << selection << '\n';
    switch (selection)
    {
      case 'x': case 'X':
	if (BATCH)
	{
	  std::cout << '\n';
	  ifs.close();
	  inptr = &std::cin;
	  std::cout << "  ** switched to keyboard input **\n";
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
	selection = 'q';
	break;

      case 'L': case 'l':
        std::cout << "  Enter table file name: ";
        *inptr >> std::setw(maxFilenameSize) >> file1;
	if (BATCH) std::cout << file1 << '\n';
        routeTable.Load(file1);
        break;

      case 'S': case 's':
        std::cout << "  Enter table file name: ";
        *inptr >> std::setw(maxFilenameSize) >> file1;
	if (BATCH) std::cout << file1 << '\n';
        routeTable.Save(file1);
        break;

      case 'I': case 'i':
        std::cout << "  Enter destination and route (dot notation): ";
        *inptr >> dS >> rS;
	if (BATCH) std::cout << dS << ' ' << rS << '\n';
        routeTable.Insert(dS, rS);
        break;

      case 'R': case 'r':
        std::cout << "  Enter destination (dot notation): ";
        *inptr >> dS;
	if (BATCH) std::cout << dS << '\n';
        routeTable.Remove(dS);
        break;

      case 'G': case 'g':
        std::cout << "                  Enter msg file name: ";
        *inptr >> std::setw(maxFilenameSize) >> file1;
	if (BATCH) std::cout << file1 << '\n';
        std::cout << "  Enter log file name (0 for default): ";
        *inptr >> std::setw(maxFilenameSize) >> file2;
	if (BATCH) std::cout << file2 << '\n';
        if (file2[0] =='0')
          routeTable.Go(file1, 0);
        else
          routeTable.Go(file1, file2);
        break;

      case 'C': case 'c':
        routeTable.Clear();
        break;

      case 'D': case 'd':
        std::cout << "  Enter Dump file name (0 for default): ";
        *inptr >> std::setw(maxFilenameSize) >> file1;
	if (BATCH) std::cout << file1 << '\n';
        if (file1[0] == '0')
          routeTable.Dump(0);
        else
          routeTable.Dump(file1);
        break;

	// case 'A': case 'a':
	// routeTable.Analysis();
	// break;

      case 'M': case 'm':
        DisplayMenu();
        break;

      default:
        std::cout << "  Try again\n";
    }
  }
  while (selection != 'q');
  std::cout << "\nRouter control program stopped\n";
  return 1;
} // end main()

void DisplayMenu()
{
   std::cout << '\n'
             << "method     (arguments)              command\n"
             << "------     -----------              -------\n"
             << "Load       (filename)  ................  L\n"
             << "Save       (filename)  ................  S\n"
             << "Insert     (ipS, ipS)  ................  I\n"
             << "Remove     (ipS)  .....................  R\n"
             << "Go         (filename, filename)  ......  G\n"
             << "Clear      ()  ........................  C\n"
     // << "Analysis   ()  ........................  A\n"
             << "Dump       ()  ........................  D\n"
             << "Display menu  .........................  M\n"
             << "Switch to interactive mode  ...........  X\n"
             << "Quit program  .........................  Q\n"
             << std::endl;
}
