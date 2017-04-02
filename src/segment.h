#ifndef SEGMENT_H
#define SEGMENT_H

#include "textLine.h"
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

class Segment
{
   public:

      //CONSTRUCTOR
      /* This will use rapidxml to read through the document and:
       * 1) Construct all Textline objects and place in vector "lines"
       * 2) Initialize the page attributes, pageWidth and pageHeight
       */
      Segment(char* filename);

      void sortLines();

      /* DEBUGGING TOOL
       * Will print out the content of the lines in the order of
       * the vector "lines". This will help test the sortLines() function
       */
      void printLines();

   private:
      vector<Textline> lines;

      //IMAGE ATTRIBUTE CONSTANTS
      int Xdimension;
      int Ydimension;

      //PAGE ATTRIBUTE CONSTANTS
      //UNKNOWN - must get from XML layout
      int pageWidth;
      int pageHeight;

      //THRESHOLD CONTANTS
};

#endif


