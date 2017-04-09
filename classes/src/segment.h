#ifndef SEGMENT_H
#define SEGMENT_H

#include "textLine.h"
#include "block.h"
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class Segment
{
   public:

      double convertToXML_h(double in);

      double convertToXML_v(double in);

      vector<Block> generate_invalid_zones(const string& json_file);

      void setDim(char* dimXcoord, char* dimYcoord);
      //CONSTRUCTOR
       /* This will use rapidxml to read through the document and:
       * 1) Construct all Textline objects and place in vector "lines"
       * 2) Initialize the page attributes, pageWidth and pageHeight
       */
      Segment(char* filename, char* jsonFile, char* dimX, char* dimY);

      /* DEBUGGING TOOL
       * Will print out the content of the lines in the order of
       * the vector "lines". 
       */
      void printLines();

      void PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, 
            const cv::Scalar& color, int fontFace, double fontScale, 
            int thickness, int lineType);

      void drawLines();

      void drawWords();

      void writeImage();

      //double distNextFour(int idx);

   private:
      vector<Textline> lines;

      //IMAGE
      cv::Mat img;

      //IMAGE ATTRIBUTE CONSTANTS
      int Xdimension;
      int Ydimension;

      //PAGE ATTRIBUTE CONSTANTS
      //UNKNOWN - must get from XML layout
      int pageWidth;
      int pageHeight;
      int numLines;
      int dimX;
      int dimY;

      //THRESHOLD CONTANTS
};

#endif


