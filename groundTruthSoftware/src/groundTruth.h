#ifndef GROUNDTRUTH_H
#define GROUNDTRUTH_H

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

using namespace std;
using namespace rapidxml;
using namespace cv;

class GroundTruth
{
   public:
      GroundTruth(): pageWidth(19268), pageHeight(28892), Xdimension(9500),
         Ydimension(9600) {}

      void PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi,
         const cv::Scalar& color, int fontFace, double fontScale, 
         int thickness, int lineType);

      void drawLine(rapidxml::xml_node<>* block, cv::Scalar color);

      //creates main_image using constructor
      //also draws original image with words
      void initImage(char* filename);

      void reDraw(int index);

      void beginTruthing(char* filename);

      void writeToFile();
      
   private:
      /* vector "lines" stores pointers to each line in page.
       * Will be stored in the same order as in XML
       */
      vector<rapidxml::xml_node<>*> lines;
      
      //stores labels assigned by user for each line
      //The indices in "labels" will correspond to the indices in "lines"
      vector<char> labels;

      Mat main_image;

      int pageWidth;
      int pageHeight;
      int Xdimension;
      int Ydimension;

};

#endif
