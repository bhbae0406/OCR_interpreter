#include "groundTruth.h"
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
   if (argc != 2)
   {
      cerr << "Program Usage: ./program_name [xmlfile]" << '\n';
      exit(1);
   }

   GroundTruth* obj = new GroundTruth();

   obj->initImage(argv[1]);
   obj->beginTruthing(argv[1]);
   obj->writeToFile();

   return 0;
}

