#include "groundTruth.h"
#include <string>
#include <iostream>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <locale>
#include <ctype.h>

using namespace rapidxml;
using namespace std;
using namespace cv;

void GroundTruth::PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, 
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

void GroundTruth::drawLine(rapidxml::xml_node<>* block, cv::Scalar color)
{
   int rHpos = atoi(block->first_attribute("HPOS")->value());
   int rVpos = atoi(block->first_attribute("VPOS")->value());
   int rHeight = atoi(block->first_attribute("HEIGHT")->value());
   int rWidth = atoi(block->first_attribute("WIDTH")->value());

   Point rP1((int)(Xdimension * (rHpos/(double)pageWidth)), (int)(Ydimension * (rVpos/(double)pageHeight)));

   Point rP2(rP1.x + (int)(Xdimension * (rWidth/(double)pageWidth))
         , rP1.y + (int)(Ydimension * (rHeight/(double)pageHeight)));

   rectangle(main_image, rP1, rP2, color, 10);
}

void GroundTruth::initImage(char* filename)
{
   double wordHPOS = 0.0;
   double wordVPOS = 0.0;
   double wordHeight = 0.0;
   double wordWidth = 0.0;

   //creates white canvas
   Mat temp(Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));

   //assigns temp to main_image
   main_image = temp;

   rapidxml::file<> xmlFile(filename);
   rapidxml::xml_document<> doc;
   doc.parse<0>(xmlFile.data());

   rapidxml::xml_node<>* root_node = doc.first_node();
   rapidxml::xml_node<>* layout_node = root_node->first_node("Layout");
   rapidxml::xml_node<>* first_textBlock = layout_node->first_node("Page")->
      first_node("PrintSpace")->first_node("TextBlock");

   for (rapidxml::xml_node<>* textBlock = first_textBlock; textBlock != 0;
         textBlock = textBlock->next_sibling("TextBlock"))
   {
      rapidxml::xml_node<>* first_textLine = textBlock->first_node("TextLine");

      for (rapidxml::xml_node<>* textLine = first_textLine; textLine != 0;
            textLine = textLine->next_sibling("TextLine"))
      {
         for (rapidxml::xml_node<>* word = textLine->first_node("String"); word != 0;
               word = word->next_sibling("String"))
         {
            wordHPOS = atoi(word->first_attribute("HPOS")->value());
            wordVPOS = atoi(word->first_attribute("VPOS")->value());
            wordHeight = atoi(word->first_attribute("HEIGHT")->value());
            wordWidth = atoi(word->first_attribute("WIDTH")->value());

            Point p1((int)(Xdimension * (wordHPOS/(double)pageWidth)), (int)(Ydimension * (wordVPOS/(double)pageHeight)));

            Point p2(p1.x + (int)(Xdimension * (wordWidth/(double)pageWidth))
                  , p1.y + (int)(Ydimension * (wordHeight/(double)pageHeight)));


            cv::Rect roi(p1.x, p1.y, (p2.x-p1.x), (p2.y-p1.y));

            PutText(main_image, word->first_attribute("CONTENT")->value(), roi,
                  Scalar(0,0,0), FONT_HERSHEY_SIMPLEX,2,8);
         }
      }
   }

   /*
      vector<int> compression_params;
      compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
      compression_params.push_back(50);
      imwrite("Sample.jpg", main_image, compression_params);
      */

   /*
   for (;;)
   {
      namedWindow("Ground Truth", WINDOW_NORMAL);
      imshow("Ground Truth", main_image);

      if (waitKey(0) == 97)
      {
         drawLine(first_textBlock, Scalar(0,0,255));
      }
   }
   */

   namedWindow("Ground Truth", WINDOW_NORMAL);
   //imshow("Ground Truth", main_image);
}

void GroundTruth::beginTruthing(char* filename)
{
   rapidxml::file<> xmlFile(filename);
   rapidxml::xml_document<> doc;
   doc.parse<0>(xmlFile.data());

   rapidxml::xml_node<>* root_node = doc.first_node();
   rapidxml::xml_node<>* layout_node = root_node->first_node("Layout");
   rapidxml::xml_node<>* first_textBlock = layout_node->first_node("Page")->
      first_node("PrintSpace")->first_node("TextBlock");

   int userInput = 0; 

   int numLines = 0;

   for (rapidxml::xml_node<>* textBlock = first_textBlock; textBlock != 0;
         textBlock = textBlock->next_sibling("TextBlock"))
   {
      rapidxml::xml_node<>* first_textLine = textBlock->first_node("TextLine");

      for (rapidxml::xml_node<>* textLine = first_textLine; textLine != 0;
            textLine = textLine->next_sibling("TextLine"))
      {
         lines.push_back(textLine);
      }
   }

   labels.resize(lines.size());

   for (size_t idx = 0; idx < lines.size(); idx++)
   {
      //draws bounding box of line in brown
      drawLine(lines[idx], Scalar(0,255,0));
      imshow("Ground Truth", main_image);

      for (;;)
      {
         userInput = waitKey(0);

         if (userInput == 97)
         {
            labels[idx] = 'a';

            for (;;)
            {
               userInput = waitKey(0);

               if (userInput < 49 || userInput > 57)
               {
                  cout << "INVALID NUMBER OF LINES. TRY AGAIN" << '\n';
               }
               else
                  break;
            }

            numLines = userInput % 48;

            for (size_t i = 0; i < (userInput % 48); i++)
            {
               drawLine(lines[idx + i], Scalar(255,0,0));
            }

            idx = idx + numLines - 1;

            break;
         }

         else if (userInput == 116)
         {
            labels[idx] = 't';
            drawLine(lines[idx], Scalar(0,0,255));
            break;
         }
         else if (userInput == 115)
         {
            labels[idx] = 's';
            drawLine(lines[idx], Scalar(0,255,255));
            break;
         }
         //press up arrow
         else if (userInput == 65362)
         {
            idx = idx - 2;
            break;
         }
         else
         {
            cout << "UNACCEPTABLE KEY. TRY AGAIN." << '\n';
            cout << "KEY: " << userInput << '\n';
            drawLine(lines[idx], Scalar(0,0,0));
         }
      }
   }
}
            

         


      




   













