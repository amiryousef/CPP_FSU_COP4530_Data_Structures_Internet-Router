/*
    iptable.cpp
    Amir Yousef
    04/18/2012
    contains RouteTable implementations and function implementations
*/

#include <fstream>
#include <iomanip>
#include <cstring>
#include <iptable.h>

void RouteTable::Load (const char* loadfile)
{
  std::ifstream fin;
  ipNumber dN, rN;

  fin.open(loadfile);

  if (fin.fail())
  {
    std::cerr << "** RouteTable: unable to open file " << loadfile << '\n'
              << "   Load() aborted\n";
    return;
  }

  fin >> std::hex;
  fin >> dN >> rN;

  while (!fin.fail())
  {
    if (dN != 0 && rN != 0)
      tablePtr_->Insert(dN, rN);
    fin >> dN >> rN;
  }

  fin.close();
  std::cout << "  Load() completed\n";
} // end RouteTable::Load()

void RouteTable::Save (const char* savefile)
{
  std::ofstream fout;
  TableType::Iterator i;

  fout.open(savefile);

  if (fout.fail())
  {
    std::cerr << "** RouteTable: unable to open file " << savefile << '\n'
              << "   Save() aborted\n";
    return;
  }

  fout << std::hex << std::uppercase << std::setfill('0');
  i = tablePtr_->Begin();

  while (i != tablePtr_->End())
  {
    fout << std::setw(8) << (*i).key_ << ' ' << std::setw(8) << (*i).data_ << '\n';
    ++i;
  }

  fout.close();
  std::cout << "  Save() completed\n";
} // end RouteTable::Save()

void RouteTable::Insert(const ipString& dS, const ipString& rS)
{
  ipNumber dN, rN, netID, hostID;
  ipClass ipC;

  dN = ipS2ipN(dS);
  ipC = ipInterpret (dN, netID, hostID);

  if (ipC == badClass)
  {
    std::cerr << "** RouteTable: bad destination number\n"
              << "   Insert() aborted\n";
    return;
  }

  rN = ipS2ipN(rS);
  ipC = ipInterpret (rN, netID, hostID);

  if (ipC == badClass)
  {
    std::cerr << "** RouteTable: bad route number\n"
              << "   Insert() aborted\n";
    return;
  }

  tablePtr_->Insert(dN, rN);
} // end RouteTable::Insert()

void RouteTable::Remove (const ipString& dS)
{
  ipNumber dN = ipS2ipN(dS);

  tablePtr_->Remove(dN);
} // end RouteTable::Remove()

ipClass RouteTable::ipInterpret (const ipNumber& address, ipNumber& netID, ipNumber& hostID)
// returns ipClass and sets netID and hostID of address
//           (bits numberd left to right beginning with 0)
//           begin      netID bits   hostID bits   (left to right)
//           -----      ----------   -----------
// classA:   0...       1..7         8..31
// classB:   10...      2..15        16..31
// classC:   110...     3..23        24..31
// badClass: all other
//
{
  netID = 0;
  hostID = 0;

  if (address == 0)
    return badClass;

  if ((address >> 31) == 0)
  {
    netID = address & 0x7f000000;
    hostID = address & 0x00ffffff;
    return classA;
  }
  else if ((address >> 30) == 2)
  {
    netID = address & 0x3fff0000;
    hostID = address & 0x0000ffff;
    return classB;
  }
  else if ((address >> 29) == 6)
  {
    netID = address & 0x1fffff00;
    hostID = address & 0x000000ff;
    return classC;
  }
  else
  {
    return badClass;
  }
} // end ipInterpret()

ipNumber RouteTable::ipS2ipN (const ipString& S)
// ipString (dot notation) to ipNumber
{
  uint32_t byte1(0), byte2(0), byte3(0), byte4(0);
  size_t i = 0, size = S.Size();

  // byte1
  if (i == size || S[i] < '0' || S[i] > '9')
  {
    std::cerr << "** ipS2ipN(): ipString syntax error -- digit expected at begin of field 1.\n";
    return 0;
  }

  while (i < size && S[i] >= '0' && S[i] <= '9')
  {
    byte1 = byte1*10 + (S[i] - '0');

    if (byte1 > 255)
    {
      std::cerr << "** ipS2ipN(): ipString error -- field 1 excedes max 255\n";
      return 0;
    }

    i++;
  }

  if (i == size || S[i] != '.')
  {
    std::cerr << "** ipS2ipN(): ipString syntax error -- '.' expected at end of field 1.\n";
    return 0;
  }

  i++;

  // byte2
  if (i == size || S[i] < '0' || S[i] > '9')
  {
    std::cerr << "** ipS2ipN(): ipString syntax error -- digit expected at begin of field 2.\n";
    return 0;
  }

  while (i < size && S[i] >= '0' && S[i] <= '9')
  {
    byte2 = byte2*10 + (S[i] - '0');

    if (byte2 > 255)
    {
      std::cerr << "** ipS2ipN(): ipString error -- field 2 excedes max 255\n";
      return 0;
    }

    i++;
  }

  if (i == size || S[i] != '.')
  {
    std::cerr << "** ipS2ipN(): ipString syntax error -- '.' expected at end of field 2.\n";
    return 0;
  }

  i++;

  // byte3
  if (i == size || S[i] < '0' || S[i] > '9')
  {
    std::cerr << "** ipS2ipN(): ipString syntax error -- digit expected at begin of field 3.\n";
    return 0;
  }

  while (i < size && S[i] >= '0' && S[i] <= '9')
  {
    byte3 = byte3*10 + (S[i] - '0');

    if (byte1 > 255)
    {
      std::cerr << "** ipS2ipN(): ipString error -- field 3 excedes max 255\n";
      return 0;
    }

    i++;
  }

  if (i == size || S[i] != '.')
  {
    std::cerr << "** ipS2ipN(): ipString syntax error -- '.' expected at end of field 3.\n";
    return 0;
  }

  i++;

  // byte4
  if (i == size || S[i] < '0' || S[i] > '9')
  {
    std::cerr << "** ipS2ipN(): ipString syntax error -- digit expected at begin of field 4.\n";
    return 0;
  }

  while (i < size && S[i] >= '0' && S[i] <= '9')
  {
    byte4 = byte4*10 + (S[i] - '0');

    if (byte4 > 255)
    {
      std::cerr << "** ipS2ipN(): ipString error -- field 4 excedes max 255\n";
      return 0;
    }

    i++;
  }

  // check that we used all of S
  if (i != size)
  {
    std::cerr << "** ipS2ipN(): ipString syntax error.\n";
    return 0;
  }

  // assemble and return ipNumber
  return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
} // end ipS2ipN()


// *****  below this line are complete *****

std::ostream& operator << (std::ostream& os, ipClass ipC)
{
  switch(ipC)
  {
    case classA:
      os.put('A'); break;
    case classB:
      os.put('B'); break;
    case classC:
      os.put('C'); break;
    case badClass:
      os.put('D'); break;
  }
  return os;
}

uint64_t ipHash::operator () (const ipNumber& ipn) const
{
  return hashfunction::KISS (ipn);
}

RouteTable::RouteTable  (uint32_t sizeEstimate) : tablePtr_(0)
{
  ipHash iph;
  tablePtr_ = new TableType  (sizeEstimate, iph);
}

RouteTable::~RouteTable ()
{
  delete tablePtr_;
}

void RouteTable::Clear()
{
  tablePtr_->Clear();
}

void RouteTable::Dump(const char* dumpfile)
{
  if (dumpfile == 0)
  {
    std::cout.setf(std::ios::uppercase);
    std::cout << "\nSize(): " << std::dec << tablePtr_->Size() << std::hex
              << "\nDump():\n";
    std::cout.fill('0');
    tablePtr_->Dump(std::cout,8,8);
    std::cout.fill(' ');
  }

  else
  {
    std::ofstream out1;
    out1.open(dumpfile);
    if (out1.fail())
    {
      std::cerr << "** RouteTable: failure to open dump file " << dumpfile << '\n'
                << "   Dump() aborted\n";
      return;
    }
    out1.setf(std::ios::uppercase);
    out1 << "\nSize(): " << std::dec << tablePtr_->Size() << std::hex
         << "\nDump():\n";

    out1.fill('0');
    tablePtr_->Dump(out1,8,8);
    out1.close();
  }
} // end RouteTable::Dump()

void RouteTable::Go (const char* msgfile, const char* logfile)
{
  std::ifstream fin;
  std::ofstream fout;
  fin.open(msgfile);
  if (fin.fail())
  {
    std::cerr << "** RouteTable: unable to open msg file " << msgfile << '\n'
              << "   Go() aborted\n";
    return;
  }

  if (logfile == 0) // log to standard output
  {
    fin >> std::hex;
    std::cout << std::hex << std::uppercase;
    ipClass ipC;
    ipString dS;
    ipNumber dN, rN, netID, hostID;
    fsu::String msgID;

    std::cout << "  Router simulation started\n";
    fin >> dS >> msgID;
    while (!fin.fail())
    {
      dN = ipS2ipN(dS);
      ipC = ipInterpret (dN, netID, hostID);
      if (ipC == badClass)
      {
        std::cout << "msgID: " << std::setw(5) << msgID
                  << std::setfill('0')
                  << " dest: " << std::setw(8)<< dN
                  << " NOT ROUTED -- BAD IP CLASS\n"
                  << std::setfill(' ');
      }
      else if (tablePtr_->Retrieve(dN,rN))
      {
        ipC = ipInterpret (rN, netID, hostID);
        std::cout << "msgID: " << std::setw(5) << msgID
                  << std::setfill('0')
                  << " dest: " << std::setw(8)<< dN
                  << " route class: " << ipC
                  << " netID: " << std::setw(8) << netID
                  << " hostID: " << std::setw(8) << hostID << '\n'
                  << std::setfill(' ');
      }
      else
      {
        std::cout << "msgID: " << std::setw(5) << msgID
                  << std::setfill('0')
                  << " dest: " << std::setw(8)<< dN
                  << " NOT ROUTED -- NO TABLE ENTRY\n"
                  << std::setfill(' ');
      }
      fin >> dS >> msgID;
    } // end while()
    fin.close();
    std::cout << "  Router simulation stopped\n";
  }  // end if
  else // log to file
  {
    fout.open(logfile);
    if (fout.fail())
    {
      std::cerr << "** RouteTable: unable to open log file " << logfile << '\n'
                << "   Go() aborted\n";
      return;
    }

    fin >> std::hex;
    fout << std::hex << std::uppercase;
    ipClass ipC;
    ipString dS;
    ipNumber dN, rN, netID, hostID;
    fsu::String msgID;

    std::cout << "  Router simulation started\n";
    fin >> dS >> msgID;
    while (!fin.fail())
    {
      dN = ipS2ipN(dS);
      ipC = ipInterpret (dN, netID, hostID);
      if (ipC == badClass)
      {
        fout << "msgID: " << std::setw(5) << msgID
             << std::setfill('0')
             << " dest: " << std::setw(8)<< dN
             << " NOT ROUTED -- BAD IP CLASS\n"
             << std::setfill(' ');
      }
      else if (tablePtr_->Retrieve(dN, rN))
      {
        ipC = ipInterpret (rN, netID, hostID);
        fout << "msgID: " << std::setw(5) << msgID
             << std::setfill('0')
             << " dest: " << std::setw(8)<< dN
             << " route class: " << ipC
             << " netID: " << std::setw(8) << netID
             << " hostID: " << std::setw(8) << hostID << '\n'
             << std::setfill(' ');
      }
      else
      {
        fout << "msgID: " << std::setw(5) << msgID
             << std::setfill('0')
             << " dest: " << std::setw(8)<< dN
             << " NOT ROUTED -- NO TABLE ENTRY\n"
             << std::setfill(' ');
      }
      fin >> dS >> msgID;
    } // end while()

    fin.close();
    fout.close();
    std::cout << "  Router simulation stopped\n";

  } // end else
  return;
} // end RouteTable::Go()
