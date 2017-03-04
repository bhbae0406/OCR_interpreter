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
#include "parser.h"

using namespace cv;
using namespace rapidxml;
using namespace std;

Parser(pWidth, pHeight, Xdim, Ydim): 
   pageWidth{pWidth}, pageHeight{pHeight}, Xdimension{Xdim}, Ydimension{Ydim};

void Parser::PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, const cv::Scalar& color, int fontFace, double fontScale, int thickness = 1, int lineType = 8)
{
   CV_Assert(!img.empty() && (img.type() == CV_8UC3 || img.type() == CV_8UC1));
   CV_Assert(roi.area() > 0);
   CV_Assert(!text.empty());

   int baseline = 0;

   // Calculates the width and height of a text string
   cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);

   // Y-coordinate of the baseline relative to the bottom-most text point
   baseline += thickness;

   // Render the text over here (fits to the text size)
   cv::Mat textImg(textSize.height + baseline, textSize.width, img.type());

   if (color == cv::Scalar::all(0)) textImg = cv::Scalar::all(255);
   else textImg = cv::Scalar::all(0);

   // Estimating the resolution of bounding image
   cv::Point textOrg((textImg.cols - textSize.width) / 2, (textImg.rows + textSize.height - baseline) / 2);

   // TR and BL points of the bounding box
   cv::Point tr(textOrg.x, textOrg.y + baseline);
   cv::Point bl(textOrg.x + textSize.width, textOrg.y - textSize.height);

   cv::putText(textImg, text, textOrg, fontFace, fontScale, color, thickness);

   // Resizing according to the ROI
   cv::resize(textImg, textImg, roi.size());

   cv::Mat textImgMask = textImg;
   if (textImgMask.type() == CV_8UC3)
      cv::cvtColor(textImgMask, textImgMask, cv::COLOR_BGR2GRAY);

   // Creating the mask
   cv::equalizeHist(textImgMask, textImgMask);

   if (color == cv::Scalar::all(0)) cv::threshold(textImgMask, textImgMask, 1, 255, cv::THRESH_BINARY_INV);
   else cv::threshold(textImgMask, textImgMask, 254, 255, cv::THRESH_BINARY);

   // Put into the original image
   cv::Mat destRoi = img(roi);
   textImg.copyTo(destRoi, textImgMask);
}


rapidxml::xml_node<>* Parser::readImage(const std::string& xmlFile)
{
   rapidxml::file<> xmlDoc(xmlFile);
   rapidxml::xml_document<> doc;
   doc.parse<0>(xmlDoc.data());

   rapidxml::xml_node<> * root_node = doc.first_node();
   rapidxml::xml_node<> * layout_node = root_node->first_node("Layout");
   rapidxml::xml_node<> * first_textblock = layout_node->first_node("Page")->
      first_node("PrintSpace")->first_node("TextBlock");

   return first_textblock;
}

void Parser::initPicture()
{
   outImg(Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));
}

void Parser::drawBlock(rapidxml::xml_node<>* block, cv::Scalar& color)
{
   int rHpos = atoi(block->first_attribute("HPOS")->value());
   int rVpos = atoi(block->first_attribute("VPOS")->value());
   int rHeight = atoi(block->first_attribute("HEIGHT")->value());
   int rWidth = atoi(block->first_attribute("WIDTH")->value());

   Point rP1((int)(Xdimension * (rHpos/(double)pageWidth)), (int)(Ydimension * (rVpos/(double)pageHeight)));

   Point rP2(rP1.x + (int)(Xdimension * (rWidth/(double)pageWidth))
         , rP1.y + (int)(Ydimension * (rHeight/(double)pageHeight)));

   //note - for Scalar - openCV uses BGR color format
   //therefore - Scalar(0, 0, 255) is solid red 
   rectangle(outImg, rP1, rP2, color, 7);
}

bool Parser::validTextBlock(rapidxml::xml_node<>* textBlock)
{
   int count = 0;

   for (xml_node<> * textLine = textBlock->first_node("TextLine"); textLine != 0;
         textLine = textLine->next_sibling("TextLine"))
   {
      ++count;
   }

   return count;
}

int Parser::getObjectWidth(rapidxml::xml_node<>* block)
{
   return (atoi(textBlock->first_attribute("WIDTH")->value()));
}

int Parser::getNumStrings(rapidxml::xml_node<>* textLine)
{
   int count = 0;
   for (xml_node<> * word = textLine->first_node("String"); word != 0;
         word = word->next_sibling("String"))
   {
      ++count;
   }

   return count;
}

int main(int argc, char* argv[])
{
   if (argc != 2)
   {
      cerr << "Program Usage: xml_image [xml_file.xml]" << '\n';
      exit(1);
   }
   string inputFile(argv[1]);

   auto const found = inputFile.find_last_of('.');
   auto filename = inputFile.substr(0,found);

   rapidxml::file<> xmlFile(argv[1]);
   rapidxml::xml_document<> doc;
   doc.parse<0>(xmlFile.data());

   //Parameters for strings
   int hpos, vpos, height, width;

   //Parameters for textBlock rectangle
   int rHpos, rVpos, rHeight, rWidth;

   //Parameters for textLine rectangle
   int lHpos, lVpos, lHeight, lWidth;
   //Point p1, p2;
   //up to mobilize -- Xdimension = 2000, Ydimension = 3500

   /* WORKING SOLUTION (jpeg image about 14 MB):
      Xdim - 9500
      Ydim - 9600
    */

   int pageWidth = 19268;
   int pageHeight = 28892;
   int Xdimension = 9500;
   int Ydimension = 9600;

   //the root node is alto
   xml_node<> * root_node = doc.first_node();
   xml_node<> * layout_node = root_node->first_node("Layout");
   xml_node<> * first_textblock = layout_node->first_node("Page")->
      first_node("PrintSpace")->first_node("TextBlock");

   Mat blank(Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));

   int counter = 0;

   for (xml_node<> * textBlock = first_textblock; textBlock != 0;
         textBlock = textBlock->next_sibling("TextBlock"))
   {
      rHpos = atoi(textBlock->first_attribute("HPOS")->value());
      rVpos = atoi(textBlock->first_attribute("VPOS")->value());
      rHeight = atoi(textBlock->first_attribute("HEIGHT")->value());
      rWidth = atoi(textBlock->first_attribute("WIDTH")->value());

      Point rP1((int)(Xdimension * (rHpos/(double)pageWidth)), (int)(Ydimension * (rVpos/(double)pageHeight)));

      Point rP2(rP1.x + (int)(Xdimension * (rWidth/(double)pageWidth))
            , rP1.y + (int)(Ydimension * (rHeight/(double)pageHeight)));

      //note - for Scalar - openCV uses BGR color format
      //therefore - Scalar(0, 0, 255) is solid red 
      rectangle(blank, rP1, rP2, Scalar(0, 255, 0), 7);


      for (xml_node<> * textLine = textBlock->first_node("TextLine"); textLine != 0;
            textLine = textLine->next_sibling("TextLine"))
      {
         lHpos = atoi(textLine->first_attribute("HPOS")->value());
         lVpos = atoi(textLine->first_attribute("VPOS")->value());
         lHeight = atoi(textLine->first_attribute("HEIGHT")->value());
         lWidth = atoi(textLine->first_attribute("WIDTH")->value());

         Point lP1((int)(Xdimension * (lHpos/(double)pageWidth)), (int)(Ydimension * (lVpos/(double)pageHeight)));

         Point lP2(lP1.x + (int)(Xdimension * (lWidth/(double)pageWidth))
               , lP1.y + (int)(Ydimension * (lHeight/(double)pageHeight)));

         //note - for Scalar - openCV uses BGR color format
         //therefore - Scalar(0, 0, 255) is solid red 
         rectangle(blank, lP1, lP2, Scalar(255, 0, 0), 5);

         for (xml_node<> * word = textLine->first_node("String"); word != 0;
               word = word->next_sibling("String"))
         {
            hpos = atoi(word->first_attribute("HPOS")->value());
            vpos = atoi(word->first_attribute("VPOS")->value());
            height = atoi(word->first_attribute("HEIGHT")->value());
            width = atoi(word->first_attribute("WIDTH")->value());

            Point p1((int)(Xdimension * (hpos/(double)pageWidth)), (int)(Ydimension * (vpos/(double)pageHeight)));

            Point p2(p1.x + (int)(Xdimension * (width/(double)pageWidth))
                  , p1.y + (int)(Ydimension * (height/(double)pageHeight)));

            counter++;

            cv::Rect roi(p1.x, p1.y, (p2.x-p1.x), (p2.y-p1.y));

            if ((p1.x < lP1.x) || (p1.y<lP1.y) || (p2.x > lP2.x) || (p2.y > lP2.y)) {
            	continue;
            } else {
            	PutText(blank, word->first_attribute("CONTENT")->value(), roi, Scalar(0,0,0), FONT_HERSHEY_SIMPLEX,2,8);
            }
            
         }
      }
   }
   cout << "NumWords: " << counter << '\n';
   vector<int> compression_params;
   compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
   compression_params.push_back(95);

   //putText(blank, "The", Point(50,100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0), 1);
   imwrite( filename + ".jpg", blank, compression_params);
   //imshow("Image",blank);

   return(0);
}












