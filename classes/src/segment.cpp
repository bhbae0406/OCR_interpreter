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
#include <unordered_map>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace cv;
using namespace rapidxml;

double Segment::convertToXML_h(double in)
{
   return in * (double) pageWidth / (double)dimX;
}

double Segment::convertToXML_v(double in)
{
   return in * (double) pageHeight / (double)dimY;
}

vector<Block> Segment::generate_invalid_zones(const string& json_file)
{
   std::stringstream ss;
   std::ifstream file(json_file);
   bool notfound = false;
   if (file) {
      ss << file.rdbuf(); 
      file.close();
   } else {
      notfound = true;
   }

   vector<Block> invalid_zones;
   if (notfound) {
      std::cout << "JSON result from image processing not found!!!" << std::endl;
      return invalid_zones;
   }
   const std::string tmp = ss.str();
   const char* json = tmp.c_str();
   rapidjson::Document d;
   d.Parse(json);

   const rapidjson::Value& annotations = d["annotations"];
   for (rapidjson::SizeType i = 0; i < annotations.Size(); i++){

      Block curBlock(convertToXML_h(static_cast<double> (annotations[i]["x"].GetDouble())), 
            convertToXML_v(static_cast<double> (annotations[i]["y"].GetDouble())), 
            convertToXML_v(static_cast<double> (annotations[i]["height"].GetDouble())), 
            convertToXML_h(static_cast<double> (annotations[i]["width"].GetDouble())), 0);
      invalid_zones.push_back(curBlock);
   }
   cout << json_file << " had " << invalid_zones.size() << " invalid zones!" << endl;
   return invalid_zones;
}

void Segment::setDim(char* dimXcoord, char* dimYcoord)
{
   this->dimX = stoi(dimXcoord);
   this->dimY = stoi(dimYcoord);
}

Segment::Segment(char* filename, char* jsonFile, char* dimX, char* dimY)
{
   bool skipLine = false;

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

   std::string json_file(jsonFile);
   setDim(dimX, dimY);

   vector<Block> invalidZones = this->generate_invalid_zones(json_file);

   xml_node<>* first_textBlock = page_node->first_node("PrintSpace")
      ->first_node("TextBlock");

   for (rapidxml::xml_node<>* textBlock = first_textBlock; textBlock != 0;
         textBlock = textBlock->next_sibling("TextBlock"))
   {
      for (rapidxml::xml_node<>* textLine = textBlock->first_node("TextLine");
            textLine != 0; textLine = textLine->next_sibling("TextLine"))
      {
         Textline newLine(textLine);

         origLines.push_back(textLine);

         skipLine = false;

         for (int i = 0; i < static_cast<int>(invalidZones.size()); i++)
         {
            if (newLine.lineInBlock(invalidZones[i]))
            {
               skipLine = true;
            }
         }

         if (!skipLine)
         {
            this->lines.push_back(newLine);
            this->numLines += 1;
         }
      }
   }


   Mat blank (Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));
   img = blank;

   drawOriginal(filename, invalidZones);
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

void Segment::drawOriginal(char* filename, std::vector<Block>& zones)
{
   drawWords(true);
   drawLines(true);

   for (int i = 0; i < static_cast<int>(zones.size()); i++)
   {
      Block tempBlock = zones[i];

      Point rP1((int)(Xdimension * (tempBlock.getX()/(double)pageWidth)), (int)(Ydimension * (tempBlock.getY()/(double)pageHeight)));

      Point rP2(rP1.x + (int)(Xdimension * (tempBlock.getWidth()/(double)pageWidth))
            ,rP1.y + (int)(Ydimension * (tempBlock.getHeight()/(double)pageHeight)));

      rectangle(img, rP1, rP2, Scalar(0,255,0), 17);
   }

   vector<int> compression_params;
   compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
   compression_params.push_back(50);

   string fileName(filename);

   fileName = fileName.substr(fileName.length() - 8, 4);

   imwrite("imageOrig_" + fileName + ".jpg", img, compression_params);

   Mat origImage(Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));

   img = origImage;
}

void Segment::drawLines(bool orig)
{
   int rHpos;
   int rVpos;
   int rHeight;
   int rWidth;
   int numOfLines = 0;

   if (orig)
   {
      numOfLines = static_cast<int>(origLines.size());
   }
   else
   {
      numOfLines = static_cast<int>(lines.size());
   }

   for (int i = 0; i < numOfLines; i++)
   {
      if (orig)
      {
         rHpos = origLines[i].getHPOS();
         rVpos = origLines[i].getVPOS();
         rHeight = origLines[i].getHeight();
         rWidth = origLines[i].getWidth();
      }
      else
      {
         rHpos = lines[i].getHPOS();
         rVpos = lines[i].getVPOS();
         rHeight = lines[i].getHeight();
         rWidth = lines[i].getWidth();
      }

      Point rP1((int)(Xdimension * (rHpos/(double)pageWidth)), (int)(Ydimension * (rVpos/(double)pageHeight)));

      Point rP2(rP1.x + (int)(Xdimension * (rWidth/(double)pageWidth))
            , rP1.y + (int)(Ydimension * (rHeight/(double)pageHeight)));

      //if title, color red -- (255,0,0)
      /*
         if (lines[i].getLabel())
         rectangle(img, rP1, rP2, Scalar(0,0,255), 7);
         else
         rectangle(img, rP1, rP2, Scalar(255,0,0), 7);
         */
      rectangle(img, rP1, rP2, Scalar(0,0,255), 7);
   }
}

void Segment::drawWords(bool orig)
{
   int printHPOS = 0;
   int printVPOS = 0;
   int printHeight = 0;
   int printWidth = 0;
   int numOfLines = 0;

   if (orig)
   {
      numOfLines = static_cast<int>(origLines.size());
   }
   else
   {
      numOfLines = static_cast<int>(lines.size());
   }

   for (int i = 0; i < numOfLines; i++)
   {
      vector<Textline::Word> curWords;
      if (orig)
      {
         curWords = origLines[i].getWords();
      }
      else
      {
         curWords = lines[i].getWords();
      }

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

void Segment::writeImage(char* filename)
{
   cout << "GOT HERE!" << '\n';
   vector<int> compression_params;
   compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
   compression_params.push_back(50);

   string fileName(filename);

   fileName = fileName.substr(fileName.length() - 8, 4);

   cout << "segImage_" + fileName + ".jpg" << '\n';

   imwrite("segImage_" + fileName + ".jpg", img, compression_params);
}
