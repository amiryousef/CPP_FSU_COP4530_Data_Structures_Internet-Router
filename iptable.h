/*
    iptable.h
    Amir Yousef
    04/18/2012
    contains RouteTable class definition and function prototypes

    Defining the RouteTable class for fast lookup of ip route addresses.

    The table stores information as ipNumber (destination, route) pairs.
    These data are stored in files as one pair per line in hex notation.

    The data are entered manually in ipStrings, which are converted to
    ipNumbers before insertion in the table.

    ipString is the familier "dot" notation N1.N2.N3.N4 where Ni is a
    decimal in the range 0..255, interpreted as a byte.
    ipString is stored as a String object.

    ipNumber is a 32 bit unsigned int, logically divided into
    four bytes <byte1><byte2><byte3><byte4>. The four decimal fields
    in the ipString notation correspond to the four bytes in the   
    ipNumber.
    The individual bits of the 32 bit word are decoded by ipClass.

    The ipClass of an address is defined as follows numbering the bits
    from left to right, begining with 0:

      classA:    bit[0] = 0
      classB:    bit[0] = 1 && bit[1] = 0
      classC:    bit[0] = 1 && bit[1] = 1 && bit[2] = 0
      badClass:  not A, B, or C

    Depending on the ipClass of an address, the netID and hostID words
    are obtained by masking. The folowing table gives the bits defining 
    the
    ipClass, netID, and hostID, and the value output to log files
    indicating ipClass.

              begin      netID bits   hostID bits   ipWrite() output
              -----      ----------   -----------   ----------------
    classA:   0...       1..7         8..31         'A'
    classB:   10...      2..15        16..31        'B'
    classC:   110...     3..23        24..31        'C'
    badClass: all other                             'D'

*/

#ifndef _IPTABLE_H
#define _IPTABLE_H

#include <iostream>
#include <fstream>
#include <stdint.h>

#include <xstring.h>
#include <hashfunctions.h>
#include <hashtbl.h>
#include <list.h>

typedef uint32_t      ipNumber;  // 32-bit register
typedef fsu::String   ipString;  // "dot" notation

class ipHash;
class RouteTable;

enum ipClass
{
   classA, classB, classC, badClass
} ;

class ipHash
{
  public:
    uint64_t operator () (const ipNumber&) const;
} ;

std::ostream& operator << (std::ostream& os, ipClass ipc);
// sends 'A', 'B', 'C', or 'D' to os depending on ipClass value

class RouteTable
{
public:  // member functions:

  void Load          (const char* loadfile);
  void Save          (const char* savefile);
  void Insert        (const ipString& dS, const ipString& rS);
  void Remove        (const ipString& dS);
  void Go            (const char* msgfile, const char* logfile);
  void Clear         ();
  void Dump          (const char* dumpfile);
       RouteTable    (uint32_t sizeEstimate);
       ~RouteTable   ();

public:  // static member functions:

  static ipClass  ipInterpret
    (
     const ipNumber& address, // the only "input"
     ipNumber& netID,         // calculated netID of address
     ipNumber& hostID         // calculated hostID of address
    );
    // pre:     none
    // post:    netID and hostID for address are set
    //          (these are set to 0 in case address is badClass)
    // return:  the ipClass of the address

  static ipNumber ipS2ipN (const ipString& S);
  // converts ipString to ipNumber
  // checks for correct "dot" notation syntax and field sizes

private: // one variable - this is an adaptor class

  typedef fsu::Entry     < ipNumber, ipNumber >         EntryType;
  typedef fsu::List      < EntryType >                  BucketType;
  typedef fsu::HashTable < ipNumber, ipNumber, ipHash > TableType;

  TableType * tablePtr_;

private: // helper methods

  // may add static or non-static helper methods here

} ; // class RouteTable

#endif
