#ifndef PARSER_H
#define PARSER_H

#include <string.h>
#include <string>
#include <cstring>
#include <stdio.h>
#include <fstream>
#include <vector>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

class Parser
{
   public:

      //constructor for Parser object
      /* Initializes pageWidth, pageHeight,
         Xdimension, and Ydimension
      */
      Parser(int pWidth, int pHeight, int Xdim, int Ydim):
         pageWidth{pWidth}, pageHeight{pHeight}, Xdimension{Xdim}, Ydimension{Ydim} {};

      void PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi,
            const cv::Scalar& color, int fontFace, double fontScale,
            int thickness, int lineType);

      /* readImage

         Input -- name of xmlFile (will be passed in as argv[1]
         Output -- pointer to node of first textblock

         1) First takes in file name and calls doc.parse<0>
         2) Returns a pointer to the first textblock in the xml file

       */

      rapidxml::xml_node<>* readImage(const char* xmlFile);

      //constructs outImg using Xdimension and Ydimension
      void initPicture();

      /* drawBlock will be used to draw textBlocks, textLines, and Strings
         --- Since drawing each of these three objects requires similar
         implementations, the general implementation is included in this function.
         The user can specify the color of the rectangle by passing in a
         Scalar object as a parameter
       */
      void drawBlock(rapidxml::xml_node<>* object, cv::Scalar color);

      //void outImage(cv::Mat& outImg, const std::string& filename);

      /* Determines if the current textBlock should be considered. It discards
         all textblocks that have no textlines (and are therefore erroneously
         identified as textBlocks)
      */
      bool validTextBlock(rapidxml::xml_node<>* textBlock);

      /* Returns the width of an object (whether it be a text block, text line,
         or string). This will be used when distinguishing between titles
         and bodies of articles using font size
      */
      int getObjectWidth(rapidxml::xml_node<>* block);

      /* Returns the number of strings within one textLine. This will be used in
         conjunction with the width of the textLine to determine the font size
         of the strings
      */
      int getNumStrings(rapidxml::xml_node<>* textLine);

      //void reDraw();

      //used to display the results after moving the trackbar
      //static void displayImage(int, void*);

      /* Uses two trackbars created in OpenCV to determine which thresholds
         for the width of textLine and number of strings will allow the best
         segmentation of the page into titles and articles
      */
      //void testFontThreshold(const char* xmlFile);

      cv::Mat outImg;
      
      //this is very ugly - but only used for testing
      rapidxml::xml_node<>* first_textBlock;

      int widthThresh;
      int numThresh;

   private:
      /* Page parameters (in pixels):

         1) Page Width - width of page in pixels
         2) Page Height - height of page in pixels
         3) Xdimension - used to make inch/1200
         to number of pixels in horizontal direction
         4) Ydimension - used to map inch/1200 
         to number of pixels in vertical direction
       */

      int pageWidth;
      int pageHeight;
      int Xdimension;
      int Ydimension;

      /*
      int widthThresh;
      int numThresh;
      */

      std::string fileName; //used when writing output jpeg file
     
      cv::Mat outImg;
      
      //this is very ugly - but only used for testing
      rapidxml::xml_node<>* first_textBlock;
};




#endif
