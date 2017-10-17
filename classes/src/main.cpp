#include "segment.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
  
   if (argc != 5) //this should be 5, but using 4 for debugging
   {
      cerr << "4 arguments needed: [xml_file.xml] [jsonFile] [dimX] [dimY]" << '\n';
      exit(1);
   }
  
   Segment* newSegment = new Segment(argv[1], argv[2], argv[3], argv[4]);

   newSegment->drawWords(false);
   newSegment->drawLines(false);
   //newSegment->drawBlocks();
   newSegment->writeImage(argv[1]);
   newSegment->writeJSON(argv[1]);
   return 0;
}

