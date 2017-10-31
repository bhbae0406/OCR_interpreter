#include "segment.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
   if (argc != 4)    
   {
      cerr << "4 arguments needed: [xml_file.xml] [.jpg image]"<<
        " [output directory]" << '\n';
      exit(1);
   }
  
   Segment* newSegment = new Segment(argv[1], argv[2]);

   newSegment->drawWords(false);
   //newSegment->drawLines(false);
   newSegment->drawBlocks();
   newSegment->writeImage(argv[1]);
   newSegment->writeJSON(argv[1], argv[3]);
   return 0;
}

