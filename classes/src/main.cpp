#include "segment.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
   if (argc != 5)
   {
      cerr << "Program Usage: xml_image [xml_file.xml] [dimX] [dimY]" << '\n';
   }

   Segment* newSegment = new Segment(argv[1], argv[2], argv[3], argv[4]);

   newSegment->drawWords();
   newSegment->drawLines();
   newSegment->writeImage();
   return 0;
}

