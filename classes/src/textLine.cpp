#include "textLine.h"

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

Textline::Textline(rapidxml::xml_node<>* textLine)
{
   /* Attributes given by xml; should remove comments once the code is working
    * with the alternative attributes given the words in the line.
    *
    this->hPos = textLine->first_attribute("HPOS")->value();
    this->vPos = textLine->first_attribute("VPOS")->value();
    this->height = textLine->first_attribute("HEIGHT")->value();
    this->width = textLine->first_attribute("WIDTH")->value();
    */   

   this->numWords = 0;
   //these value will store the best attributes for the line
   int bestHPOS = 0;
   int bestVPOS = 0;
   int bestRightHPOS = 0;
   int bestBottomVPOS = 0;

   for (rapidxml::xml_node<>* word = textLine->first_node("String");
         word != 0; word = word->next_sibling("String"))
   {
      Word newWord;
      newWord.hPos = atoi(word->first_attribute("HPOS")->value());
      newWord.vPos = atoi(word->first_attribute("VPOS")->value());
      newWord.height = atoi(word->first_attribute("HEIGHT")->value());
      newWord.width = atoi(word->first_attribute("WIDTH")->value());
      newWord.content = word->first_attribute("CONTENT")->value();
      if ((word->first_attribute("SUBS_CONTENT")) != NULL)
      {
         newWord.subContent = true;
         this->hasHyphen = true;
      }
      else
      {
         newWord.subContent = false;
      }

      this->words.push_back(newWord);

      /* Assigning textLine attributes as the follows:
       * HPOS - least hPos of all the words
       * VPOS - least vPos of all the words
       * HEIGHT - minimum height required to fit all words
       * WIDTH - minimum width required to fit all words
       *
       * This prevents lines from being extra long with too much
       * unneccessary white space. It also allows those lines that
       * have sub-contents to fit the second half of hypenated words.
       */

      if (this->numWords == 0)
      {
         bestHPOS = newWord.hPos;
         bestVPOS = newWord.vPos;
         bestRightHPOS = newWord.hPos + newWord.width;
         bestBottomVPOS = newWord.vPos + newWord.height;      
      }
      else
      {
         if (newWord.hPos < bestHPOS)
            bestHPOS = newWord.hPos;
         if (newWord.vPos < bestVPOS)
            bestVPOS = newWord.vPos;
         if ((newWord.hPos + newWord.width) > bestRightHPOS)
            bestRightHPOS = newWord.hPos + newWord.width;
         if ((newWord.vPos + newWord.height) > bestBottomVPOS)
            bestBottomVPOS = newWord.vPos + newWord.height;
      }

      this->numWords += 1;
   }

   this->hPos = bestHPOS;
   this->vPos = bestVPOS;
   this->height = bestBottomVPOS - bestVPOS;
   this->width = bestRightHPOS - bestHPOS;

   //initialize title to false
   this->title = false;
}

int Textline::getHPOS()
{
   return this->hPos;
}

int Textline::getVPOS()
{
   return this->vPos;
}

int Textline::getWidth()
{
   return this->width;
}

int Textline::getHeight()
{
   return this->height;
}

bool Textline::getLabel()
{
   return this->title;
}

double Textline::charAreaRatio()
{
   double area = 0.0;
   int numChar = 0.0;

   vector<double> ratio;

   for (int i = 0; i < this->numWords; i++)
   {
      area = this->words[i].width * this->words[i].height;
      numChar = this->words[i].content.length();
      ratio.push_back(area / numChar);
   }

   sort(ratio.begin(), ratio.end());

   int midIdx = (ratio.size()) / 2;

   return ratio[midIdx];
}

bool Textline::capLine(bool allCaps)
{
   int numChar = 0;
   int numCap = 0;
   int numFirstCap = 0;
   int numWord = 0;

   string text;

   for (int i = 0; i < this->numWords; i++)
   {
      text = this->words[i].content;

      if (isalpha(text[0]))
      {
         numWord++;
      }

      if (isalpha(text[0]) && (isupper(text[0])) && (text.length() >= 1))
      {
         numFirstCap++;
      }

      if (allCaps)
      {
         for (size_t i = 0; i < text.length(); i++)
         {
            ++numChar;
            if (isalpha(text[i]) && !(ispunct(text[i])) && isupper(text[i]))
            {
               ++numCap;
            }
         }
      }
   }

   if (allCaps)
   {
      if ((((double)numCap)/numChar) > ALLCAP)
         return true;
      else
         return false;
   }

   else
   {
      if ((((double)numFirstCap) / numWord) >= FIRSTCAP)
      {
         return true;
      }
      else
      {
         return false;
      }
   }
}

bool Textline::isLine(string key, int wordCount)
{
   string text;
   bool found = false;

   for (int i = 0; i < this->numWords; i++)
   {
      text = words[i].content;

      if (!(text.compare(key)))
         found = true;
   }

   if (found && (this->numWords == wordCount))
      return true;
   else
      return false;
}

void Textline::printLine()
{
   for (int i = 0; i < (this->numWords) - 1; i++)
   {
      cout << this->words[i].content << " ";
   }

   cout << this->words[this->numWords - 1].content << '\n';
}

/*
void Textline::PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, const cv::Scalar& color, int fontFace, double fontScale, int thickness = 1, int lineType = 8)
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
*/

vector<Textline::Word>& Textline::getWords()
{
   return this->words;
}
