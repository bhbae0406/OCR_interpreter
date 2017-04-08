#include "segment.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
   if (argc != 2)
   {
      cerr << "Program Usage: [xml_file.xml]" << '\n';
   }

   Segment* newSegment = new Segment(argv[1]);

   newSegment->drawWords();
   newSegment->writeImage();
   return 0;
}

