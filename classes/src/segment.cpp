#include "segment.h"
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
#include <locale>
#include <ctype.h>
#include <algorithm>
#include <functional>
#include <boost/algorithm/string/predicate.hpp>
#include <fstream>

using namespace std;
using namespace cv;
using namespace rapidxml;

Segment::Segment(char* filename)
{
   rapidxml::file<> xmlFile(filename);
   rapidxml::xml_document<> doc;
   doc.parse<0>(xmlFile.data());

   xml_node<>* root_node = doc.first_node();
   xml_node<>* layout_node = root_node->first_node("Layout");
   xml_node<>* page_node = layout_node->first_node("Page");

   this->pageWidth = atoi(page_node->first_attribute("WIDTH")->value());
   this->pageHeight = atoi(page_node->first_attribute("HEIGHT")->value());
   this->numLines = 0; 

   this->Xdimension = 9500;
   this->Ydimension = 9600;

   xml_node<>* first_textBlock = page_node->first_node("PrintSpace")
      ->first_node("TextBlock");

   for (rapidxml::xml_node<>* textBlock = first_textBlock; textBlock != 0;
         textBlock = textBlock->next_sibling("TextBlock"))
   {
      for (rapidxml::xml_node<>* textLine = textBlock->first_node("TextLine");
            textLine != 0; textLine = textLine->next_sibling("TextLine"))
      {
         Textline newLine(textLine);

         this->lines.push_back(newLine);
         this->numLines += 1;
      }
   }

   Mat blank (Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));
   img = blank;
}

void Segment::printLines()
{
   for (int i = 0; i < numLines; i++)
   {
      lines[i].printLine();
   }
}

void Segment::PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, 
      const cv::Scalar& color, int fontFace, double fontScale, int thickness = 1, 
      int lineType = 8)
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

void Segment::drawLines()
{
   int rHpos;
   int rVpos;
   int rHeight;
   int rWidth;

   for (int i = 0; i < numLines; i++)
   {
      rHpos = lines[i].getHPOS();
      rVpos = lines[i].getVPOS();
      rHeight = lines[i].getHeight();
      rWidth = lines[i].getWidth();

      Point rP1((int)(Xdimension * (rHpos/(double)pageWidth)), (int)(Ydimension * (rVpos/(double)pageHeight)));

      Point rP2(rP1.x + (int)(Xdimension * (rWidth/(double)pageWidth))
            , rP1.y + (int)(Ydimension * (rHeight/(double)pageHeight)));

      //if title, color red -- (255,0,0)
      if (lines[i].getLabel())
         rectangle(img, rP1, rP2, Scalar(0,0,255), 7);
      else
         rectangle(img, rP1, rP2, Scalar(255,0,0), 7);
   }
}

void Segment::drawWords()
{
   int printHPOS = 0;
   int printVPOS = 0;
   int printHeight = 0;
   int printWidth = 0;

   for (int i = 0; i < numLines; i++)
   {
      vector<Textline::Word> curWords = lines[i].getWords();

      for (int i = 0; i < static_cast<int>(curWords.size()); i++)
      {   
         printHPOS = curWords[i].hPos;
         printVPOS = curWords[i].vPos;
         printHeight = curWords[i].height;
         printWidth = curWords[i].width;

         Point p1((int)(Xdimension * (printHPOS/(double)pageWidth)), (int)(Ydimension * (printVPOS/(double)pageHeight)));

         Point p2(p1.x + (int)(Xdimension * (printWidth/(double)pageWidth))
               , p1.y + (int)(Ydimension * (printHeight/(double)pageHeight)));

         cv::Rect roi(p1.x, p1.y, (p2.x-p1.x), (p2.y-p1.y));

         PutText(img, curWords[i].content, roi, Scalar(0,0,0), FONT_HERSHEY_SIMPLEX,2,8);
      }
   }
}

void Segment::writeImage()
{
   vector<int> compression_params;
   compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
   compression_params.push_back(50);

   imwrite("segImage.jpg", img, compression_params);
}
