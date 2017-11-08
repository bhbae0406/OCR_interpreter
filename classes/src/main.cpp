#include "segment.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
   if (argc != 6)    
   {
      cerr << " 5 arguments needed: [xml_file.xml] [dimX] [dimY]"<<
        " [output directory] [param.json]" << '\n';
      exit(1);
   }
  
   Segment* newSegment = new Segment(argv[1], argv[2], argv[3], argv[5]);

   newSegment->drawWords(false);
   //newSegment->drawBlocks();
   //newSegment->drawLines(false);
   //newSegment->writeImage(argv[1]);
   newSegment->writeJSON(argv[1], argv[4]);
   return 0;
}

