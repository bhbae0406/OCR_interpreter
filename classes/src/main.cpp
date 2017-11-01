#include "segment.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
   if (argc != 5)    
   {
      cerr << "4 arguments needed: [xml_file.xml] [dimX] [dimY]"<<
        " [output directory]" << '\n';
      exit(1);
   }
  
   Segment* newSegment = new Segment(argv[1], argv[2], argv[3]);

   newSegment->drawWords(false);
   //newSegment->drawLines(false);
   newSegment->drawBlocks();
   newSegment->writeImage(argv[1]);
   newSegment->writeJSON(argv[1], argv[4]);
   return 0;
}

