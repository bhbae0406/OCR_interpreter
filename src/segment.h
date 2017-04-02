#ifndef SEGMENT_H
#define SEGMENT_H

#include "textLine.h"
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
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

      /* DEBUGGING TOOL
       * Will print out the content of the lines in the order of
       * the vector "lines". This will help test the sortLines() function
       */
      void printLines();

      void sortLines();

      void PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, const cv::Scalar& color, int fontFace, double fontScale, int thickness = 1, int lineType = 8);

      void drawBlock(cv::Scalar color);

      double distNextFour(int idx);



   private:
      vector<Textline> lines;

      //IMAGE ATTRIBUTE CONSTANTS
      int Xdimension;
      int Ydimension;

      //PAGE ATTRIBUTE CONSTANTS
      //UNKNOWN - must get from XML layout
      int pageWidth;
      int pageHeight;
      int numLines;

      //THRESHOLD CONTANTS
};

#endif


