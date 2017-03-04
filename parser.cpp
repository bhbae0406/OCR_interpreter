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

rapidxml::xml_node<>* Parser::readImage(const char* xmlFile)
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
   Mat temp(Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));

   outImg = temp.clone();
}

void Parser::drawBlock(rapidxml::xml_node<>* block, cv::Scalar color)
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

   if (count > 0)
      return true;
   else
      return false;
}

int Parser::getObjectWidth(rapidxml::xml_node<>* block)
{
   int xmlWidth = 0;

   xmlWidth = atoi(block->first_attribute("WIDTH")->value());

   return (Ydimension * (xmlWidth/(double)pageHeight));
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