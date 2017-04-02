#include <string.h>
#include <string>
#include <cstring>
#include <stdio.h>
#include <fstream>
#include <vector>
#include "rapidxml.hpp"
#include "linker.h"
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
//#include "parser.h"

using namespace rapidxml;
using namespace std;
using namespace cv;

int heightThresh = 0;
int numThresh = 0;
int blockThresh = 0;
int widthCharRatio = 0;
int widthWordRatio = 0;
int distThresh = 0;
int charArea = 0;
int distAbove = 0;

int leftThresh = 143;
int rightThresh = 63;

int nextThresh = 63;
int prevThresh = 108;

int diffThresh = 11;

int pageWidth = 19268;
int pageHeight = 28892;
int Xdimension = 9500;
int Ydimension = 9600;
int invalidLinesThresh = 50;

Mat blank(Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));

rapidxml::xml_node<>* first_textblock;

void PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, const cv::Scalar& color, int fontFace, double fontScale, int thickness = 1, int lineType = 8)
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


double getWidthCharRatio(rapidxml::xml_node<>* textLine, bool countWord)
{
   string text;
   int charCount = 0;
   double width = 0;

   int wordCount = 0;

   for (xml_node<>* word = textLine->first_node("String"); word != 0;
         word = word->next_sibling("String"))
   {
      text = word->first_attribute("CONTENT")->value();
      width += atof(word->first_attribute("WIDTH")->value());
      charCount += text.length();
      wordCount += 1;
   }

   width = Xdimension * (width / (double)pageWidth);

   if (countWord)
      return (width / wordCount);
   else
      return (width / charCount);
}

double getObjectHeight(rapidxml::xml_node<>* block)
{       
   double xmlHeight = 0;
   xmlHeight = atof(block->first_attribute("HEIGHT")->value());

   return (Ydimension * (xmlHeight/pageHeight));
}

double getObjectWidth(rapidxml::xml_node<>* block)
{       
   double xmlHeight = 0;
   xmlHeight = atof(block->first_attribute("WIDTH")->value());

   return (Ydimension * (xmlHeight/pageHeight));
}

double getVPOS(rapidxml::xml_node<>* block) 
{
   int Vpos = atoi(block->first_attribute("VPOS")->value());
   return (Ydimension * (Vpos/(double)pageHeight));
}

double getHPOS(rapidxml::xml_node<>* block) 
{
   int Vpos = atoi(block->first_attribute("HPOS")->value());
   return (Ydimension * (Vpos/(double)pageHeight));
}

void drawBlock(rapidxml::xml_node<>* block, cv::Scalar color)
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
   rectangle(blank, rP1, rP2, color, 7);
}

bool isLine(rapidxml::xml_node<>* textLine, String key, int numWords)
{
   String text;
   int wordCount = 0;
   bool found = false;
   
   for (rapidxml::xml_node<>* word = textLine->first_node("String"); word != 0;
         word = word->next_sibling("String"))
   {
      wordCount++;
      text = word->first_attribute("CONTENT")->value();

      if (!(text.compare(key)))
         found = true;
   }
   
   if (found && (wordCount == numWords))
      return true;
   else
      return false;
}


bool capLine(rapidxml::xml_node<>* textLine, bool allCaps)
{
   int numChar = 0;
   int numCap = 0;

   int numFirstCap = 0;

   bool cap = true;

   int numWord = 0;

   string text;

   for (rapidxml::xml_node<>* word = textLine->first_node("String"); word != 0;
         word = word->next_sibling("String"))
   {
      if (isalpha(text[0]))
      {
         numWord++;
      }

      text = word->first_attribute("CONTENT")->value();
      
      
      if (isalpha(text[0]) && (isupper(text[0])) && (text.length() >= 1))
      {
         numFirstCap++;
         //cap = false;
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
      //account for those nuances when a word in a title is hypenated,
      //the second word in the hyphen is lower-cased and regarded as a separate
      //word
      if ((((double)numCap)/numChar) > 0.7)
         return true;
      else
         return false;
   }
   else
   {
      if (isLine(textLine, "Speers", 3))
      {
         cout << "RATIO = " << ((double)numFirstCap / numWord) << '\n';
      }

      //if (cap == true) //&& numWord > 1)
      if (((double)numFirstCap / numWord) >= 0.74)
         return true;
      else
         return false;
   }
}

double distNextFour(rapidxml::xml_node<>* textLine)
{
   double prevLoc = getVPOS(textLine) + getObjectHeight(textLine);
   double curLoc = prevLoc;

   double dist = 0.0;

   rapidxml::xml_node<>* tempLine = NULL;

   if ((tempLine = (textLine->next_sibling("TextLine"))) != NULL)
   {
      curLoc = getVPOS(tempLine);

      if ((tempLine = (tempLine->next_sibling("TextLine"))) != NULL)
      {
         curLoc = getVPOS(tempLine);
          
         if ((tempLine = (tempLine->next_sibling("TextLine"))) != NULL)
         {
            curLoc = getVPOS(tempLine);
         }
         
         
      }
   }

   dist = curLoc - prevLoc;

   return dist;
}

double getOrigVPOS(rapidxml::xml_node<>* block)
{
   return double(atoi(block->first_attribute("VPOS")->value()));
}

double getOrigHPOS(rapidxml::xml_node<>* block)
{
   return double(atoi(block->first_attribute("HPOS")->value()));
}

double getOrigHeight(rapidxml::xml_node<>* block)
{
   return double(atoi(block->first_attribute("HEIGHT")->value()));
}

double getOrigWidth(rapidxml::xml_node<>* block)
{
   return double(atoi(block->first_attribute("WIDTH")->value()));
}

double convertToPixel(double val)
{
   //assuming val is measured in 1/1200th of an inch
   return (72.0 * (val / 1200.0));
}

double distNextOne(rapidxml::xml_node<>* textLine)
{
   double prevLoc = getOrigVPOS(textLine) + getOrigHeight(textLine);

   double curLoc = prevLoc;
   
   rapidxml::xml_node<>* tempLine = NULL;

   //will be NULL if at bottom of textBlock
   if ((tempLine = (textLine->next_sibling("TextLine"))) != NULL)
   {
      curLoc = getOrigVPOS(tempLine);
   }

   return (curLoc - prevLoc);
}

double distPrevOne(rapidxml::xml_node<>* textLine,
      rapidxml::xml_node<>* prevLine)
{
   double prevLoc = getOrigVPOS(prevLine) + getOrigHeight(prevLine);
   double curLoc = getOrigVPOS(textLine);

   return (curLoc - prevLoc);
}

double charAreaRatio(rapidxml::xml_node<>* textLine)
{
   string text;
   double area = 0.0;
   int numChar = 0.0;

   vector<double> ratio;

   for (rapidxml::xml_node<>* word = textLine->first_node("String"); word != 0;
         word = word->next_sibling("String"))
   {
      text = word->first_attribute("CONTENT")->value();
      
      area = getObjectWidth(word) * getObjectHeight(word);

      numChar = text.length();

      ratio.push_back((area / numChar));
   }

   sort(ratio.begin(), ratio.end());

   int midIdx = (ratio.size()) / 2;

   return ratio[midIdx];
}

bool centeredLine(rapidxml::xml_node<>* textLine, int leftThresh,
      int rightThresh)
{
   double curHPOS = getOrigHPOS(textLine);

   double newHPOS = curHPOS; //if tempLine is NULL, the difference will be 0

   double curRight = getOrigHPOS(textLine) + getOrigWidth(textLine);

   double newRight = curRight;

   rapidxml::xml_node<>* tempLine = NULL;

   if ((tempLine = (textLine->next_sibling("TextLine"))) != NULL)
   {
      newHPOS = getOrigHPOS(tempLine);
      newRight = getOrigHPOS(tempLine) + getOrigWidth(tempLine); 
   }

   double leftDiff = curHPOS - newHPOS;

   double rightDiff = newRight - curRight;

   if ((leftDiff > leftThresh) && (rightDiff > rightThresh))
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool includeHypen(rapidxml::xml_node<>* textLine)
{
   //does the textLine include a hypenated word?
   //need to check for SUB_CONTENT

   for (rapidxml::xml_node<>* word = textLine->first_node("String"); word != 0;
         word = word->next_sibling("String"))
   {
      if ((word->first_attribute("SUBS_CONTENT")) != NULL)
         return true;
   }

   return false;
}

//BLOCK SEGEMENTATION

struct Block
{
   string label;
   double x;
   double y;
   double height;
   double width;
   string block_content;
};

//void displayImage(int, void*)
//bool displayImage(int charA)
void displayImage(string filename)
{
   double tempRatio = 0.0;
   double tempWordRatio = 0.0;
   double tempHeight = 0.0;
   double tempDist = 0.0;
   double tempDist1 = 0.0;
   double tempCA = 0.0;

   double temp = 0.0;

   int numLine = 0;
   
   double prevLoc = 0.0;
   double curLoc = 0.0;
   double dist1 = 0.0;
   double dist2 = 0.0;

   double gMaxDist = -1.0; 
   double gMinDist = 1000000.0; 

   double maxDist = 0.0;

   double cArea = 0.0;


   bool reached = false;

   String word1;
   String word2;

   vector<Block> master;

   double vpos, tempVpos;
   double hpos = 0;

   double width = 0;

   bool title = false;

   bool process = false;

   int numBlock = 0;

   bool prevCat = false;
   bool curCat = false;

   double newEdge = 0;
   double curEdge = 0;

   double printHPOS = 0;
   double printVPOS = 0;
   double printHeight = 0;
   double printWidth = 0;
   int countInvalidLines = 0;

   double nextDistance = 0.0;
   double prevDistance = 0.0;

   bool possPancake = false;
   int numPancake = 0;

   bool allCaps = false;

   bool firstIf = false;

   int numCategory = 0; //number of lines in a particular category

   rapidxml::xml_node<>* prevLine = NULL;

   for (rapidxml::xml_node<>* textBlock = first_textblock; textBlock != 0;
         textBlock = textBlock->next_sibling("TextBlock"))
   {
      numBlock++;

      if (process && (numLine > 0))
      {
            Block temp;

            if (prevCat)
               temp.label = "Title";
            else
               temp.label = "Article";

            temp.y = vpos;
            temp.x = hpos;
            temp.height = prevLoc - vpos;
            temp.width = width;

            Point rP1((int)(Xdimension * (temp.x/(double)pageWidth)), (int)(Ydimension * (temp.y/(double)pageHeight)));

            Point rP2(rP1.x + (int)(Xdimension * (temp.width/(double)pageWidth))
         ,rP1.y + (int)(Ydimension * (temp.height/(double)pageHeight)));

            //DRAW BLOCK
            /*
            if (temp.label == "Title")
               rectangle(blank, rP1, rP2, Scalar(0,0,255), 17);
            else
               rectangle(blank, rP1, rP2, Scalar(255,0,0), 17);
            */

            temp.y = convertToPixel(vpos);
            temp.x = convertToPixel(hpos);
            temp.height = convertToPixel(prevLoc - vpos);
            temp.width = convertToPixel(width);
      
            master.push_back(temp);

            process = false;
            width = 0;
      }
      
      numLine = 0;

      vpos = getOrigVPOS(textBlock);
      hpos = getOrigHPOS(textBlock);
      tempVpos = vpos;

      prevLine = NULL; //resetting prevLine to NULL to mark new block

      numCategory = 0;

      for (rapidxml::xml_node<>* textLine = textBlock->first_node("TextLine");
            textLine != 0; textLine = textLine->next_sibling("TextLine"))
      {
         possPancake = false;

         firstIf = false;
         allCaps = false;
         
	 if(numLine) {
	     if(prevLoc >= getOrigVPOS(textLine)) 
		countInvalidLines++;
  	 }        
	 
         // FILE WRITE
	 if(countInvalidLines > invalidLinesThresh){
	     // write to error.txt
            std:cout << "Document" << filename << " is not worth processing!" << std::endl;	
	    ofstream invalidFiles;
            invalidFiles.open("invalidFiles.txt", std::ios::app);
            invalidFiles << filename << '\n';
            invalidFiles.close();
            exit(1);
	 } 
         
         prevCat = curCat;
         numLine++;
         //tempDist1 = distAbove / (double)10;
         tempDist1 = 100.0;
         dist2 = distNextFour(textLine);
         cArea = charAreaRatio(textLine);
         temp = getWidthCharRatio(textLine, true);

         if (dist1 > gMaxDist)
         {
            gMaxDist = dist1;
         }
         if (dist1 < gMinDist)
         {
            gMinDist = dist1;
         }
         
         distThresh = 1611;
         tempDist = distThresh / (double)10;
         tempRatio = widthCharRatio / (double)10;
         tempRatio += 1.64348; //so min is 1.64348
         tempWordRatio = widthWordRatio / (double)10;
         tempWordRatio += 60.0;
         heightThresh = 438;
         tempHeight = heightThresh / (double)10;
         tempHeight += 11.9618;
         charArea = 4577;
         tempCA = charArea / (double)10;
         tempCA += 315;
         //changes
         tempCA = 785;
         
         if (capLine(textLine, true))
         {
            if (isLine(textLine, "Old", 6))
            {
               cout << "ALL CAPS!" << '\n';
            }

            allCaps = true;
         }

         if (((getObjectHeight(textLine) - tempHeight) > diffThresh) ||
                  ((getObjectHeight(textLine) > tempHeight) &&
                     capLine(textLine, false)) || allCaps)

         {
            if (isLine(textLine, "Old", 6))
            {
               cout << "GOT -1" << '\n';
            }

            title = true;
            drawBlock(textLine, Scalar(0,0,255));

            firstIf = true;
         }

         else
         {
            if (isLine(textLine, "Old", 6))
            {
               cout << "GOT 0" << '\n';
               //cout << capLine(textLine, false) << '\n';
            }

            if (capLine(textLine, false) && (cArea > tempCA))
            {
               if (isLine(textLine, "Old", 6))
               {
                  cout << "GOT 1" << '\n';
                  cout << "cArea = " << cArea << '\n';
                  cout << "tempCA = " << tempCA << '\n';
               }

               if ((dist2 >= tempDist))// || (dist1 >= tempDist1))
               {
                  if (isLine(textLine, "Old", 6))
                  {
                     cout << "GOT 2" << '\n';
                  }

                  if (textLine->first_node("String") != NULL)
                  {
                     word1 = textLine->first_node("String")->
                        first_attribute("CONTENT")->value(); 

                     if (textLine->first_node("String")->next_sibling("String")
                           != NULL)
                     {
                        word2 = textLine->first_node("String")
                           ->next_sibling("String")->
                           first_attribute("CONTENT")->value(); 
                     }
                  }
                  
                  if (boost::iequals(word1, "continued") &&
                        boost::iequals(word2, "on"))
                  {
                     title = false;
                     drawBlock(textLine, Scalar(255,0,0));
                  }
                  
                  else
                  {
                     title = true;
                     drawBlock(textLine, Scalar(0,0,255));
                  } 
               }
               
               else
               {
                  title = false;
                  drawBlock(textLine, Scalar(255,0,0));
               }
            }

            else
            {
               title = false;
               drawBlock(textLine, Scalar(255,0,0));
            }
         }

         /*
         if (centeredLine(textLine, leftThresh, rightThresh) &&
               !includeHypen(textLine))
         {
            drawBlock(textLine, Scalar(0,255,0));
         }
         */

         if (isLine(textLine, "Old", 6))
         {
            cout << "Title = " << title << '\n';
         }

         if (title == prevCat)
         {
            numCategory++;
         }

         if ((title != prevCat) && (title == true) && (numCategory > 4)
               && (distPrevOne(textLine, prevLine) < prevThresh)
               && !(((getObjectHeight(textLine) - tempHeight) > diffThresh)
                && capLine(textLine, false) || (allCaps)))
         {
            if (isLine(textLine, "Old", 6))
            {
               cout << "First Change" << '\n';
            }

            drawBlock(textLine, Scalar(255,0,0));
            title = false;
         }

         //if we change from article to title
         //and prevLine is not NULL (at least one line as been segmented)
         if ((title != prevCat) && (prevCat == false) && (prevLine != NULL)
               && !firstIf)
         {

            if (isLine(textLine, "Old", 6))
            {
               cout << "Second Change" << '\n';
            }

            prevDistance = distPrevOne(textLine, prevLine);
            nextDistance = distNextOne(textLine);

            if ((prevDistance < nextDistance)
                  && (nextDistance > nextThresh)
                  && (prevDistance < prevThresh))
            {
               if (isLine(textLine, "CACHED", 3))
               {
                  cout << "REACHED HERE!" << '\n';
               }

               drawBlock(textLine, Scalar(255,0,0));
               title = false;
            }
         }

         if ((title != prevCat) && (prevCat == true) && (prevLine != NULL))
         {
            prevDistance = distPrevOne(textLine, prevLine);
            nextDistance = distNextOne(textLine);

            //absolute value argument is for outliers when next line is simply
            //to the right of the current line
            if ((prevDistance < nextDistance)
                  && (nextDistance > nextThresh)
                  && (prevDistance < prevThresh)
                  & (abs(getOrigHPOS(prevLine) - getOrigHPOS(textLine)) < 500)
                  && (centeredLine(textLine, 20, 20) && !includeHypen(textLine)))
            {
               drawBlock(textLine, Scalar(0,0,255));
               title = true;
            }
         }

         if (title != prevCat)
         {
            numCategory = 0;
         }

         if (isLine(textLine, "Old", 6))
         {
            cout << "New Title = " << title << '\n';
         }

         /* TRYING PANCAKE METHOD -- Doesn't work though

         if (title && !possPancake)
         {
            possPancake = true;
         }

         if (isLine(textLine, "Aid", 5))
         {
            cout << "Hello" << '\n';
         }

         if (possPancake && (prevLine != NULL))
         {
            if (((getOrigHPOS(textLine) - getOrigHPOS(prevLine)) > 10)
                  && (((getOrigHPOS(prevLine) + getOrigWidth(prevLine))
                     - (getOrigHPOS(textLine) + getOrigWidth(textLine))) > 10))
            {
               numPancake++;
            }
            else
            {
               possPancake = false;
            }
         }

         if (possPancake && (numPancake >= 1) && (prevLine != NULL))
         {
            if (((getOrigHPOS(textLine) - getOrigHPOS(prevLine)) < 0)
                  || (((getOrigHPOS(prevLine) + getOrigWidth(prevLine))
                     - (getOrigHPOS(textLine) + getOrigWidth(textLine))) > 10))

            {
               drawBlock(textLine, Scalar(0,255,0));
            }
         }
         */

         curCat = title;

         if ((process) && (curCat != prevCat))
         {
            Block temp;

            if (prevCat == true)
            {
               temp.label = "Title";
            }
            else
            {
               temp.label = "Article";
            }

            temp.y = vpos;
            temp.x = hpos;
            temp.height = prevLoc - temp.y;
            temp.width = width;

            Point rP1((int)(Xdimension * (temp.x/(double)pageWidth)), (int)(Ydimension * (temp.y/(double)pageHeight)));

            Point rP2(rP1.x + (int)(Xdimension * (temp.width/(double)pageWidth))
         ,rP1.y + (int)(Ydimension * (temp.height/(double)pageHeight)));
         
            //DRAW BLOCK
            //if (temp.label == "Title")
               //rectangle(blank, rP1, rP2, Scalar(0,0,255), 17);
            //else
               //rectangle(blank, rP1, rP2, Scalar(255,0,0), 17);

            temp.y = convertToPixel(vpos);
            temp.x = convertToPixel(hpos);
            temp.height = convertToPixel(prevLoc - temp.y);
            temp.width = convertToPixel(width);

            master.push_back(temp);
            process = false;
            width = 0;
         }

         if (!process)
         {
            vpos = getOrigVPOS(textLine);
            hpos = getOrigHPOS(textLine);
            process = true;
         }

         if (getOrigHPOS(textLine) < hpos)
            hpos = getOrigHPOS(textLine);

         newEdge = getOrigHPOS(textLine) + getOrigWidth(textLine);
         curEdge = hpos + width;

         if (newEdge > curEdge)
            width += (newEdge - curEdge);

         prevLoc = getOrigVPOS(textLine) + getOrigHeight(textLine);

	for (xml_node<> * word = textLine->first_node("String"); word != 0;
               word = word->next_sibling("String"))
         {
            printHPOS = atoi(word->first_attribute("HPOS")->value());
            printVPOS = atoi(word->first_attribute("VPOS")->value());
            printHeight = atoi(word->first_attribute("HEIGHT")->value());
            printWidth = atoi(word->first_attribute("WIDTH")->value());

            Point p1((int)(Xdimension * (printHPOS/(double)pageWidth)), (int)(Ydimension * (printVPOS/(double)pageHeight)));

            Point p2(p1.x + (int)(Xdimension * (printWidth/(double)pageWidth))
                  , p1.y + (int)(Ydimension * (printHeight/(double)pageHeight)));


            cv::Rect roi(p1.x, p1.y, (p2.x-p1.x), (p2.y-p1.y));

            	PutText(blank, word->first_attribute("CONTENT")->value(), roi, Scalar(0,0,0), FONT_HERSHEY_SIMPLEX,2,8);
         }

         prevLine = textLine;
      } //for textLine

   } //for textblock

   // FILE WRITE

   int countBlock = 0;
   ofstream o;
   o.open("./output/blockOut/output_" + filename + ".txt");

   cout << "./output/blockOut/output_" + filename + ".txt" << endl;
   for (size_t i = 0; i < master.size(); i++)
   {
      o << "Class = " << master[i].label << '\n';
      o << "height = " << master[i].height << '\n';
      o << "id = " << countBlock << '\n';
      o << "type = " << "rect" << '\n';
      o << "width = " << master[i].width << '\n';
      o << "x = " << master[i].x << '\n';
      o << "y = " << master[i].y << '\n';
      o << '\n';

      countBlock++;
   }

   o.close();
   
    
   vector<int> compression_params;
   compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
   compression_params.push_back(50);
  
   std::cout << "Number of invalid lines detected was : " << countInvalidLines << std::endl; 
   imwrite("output/segImage/segImage_" + filename + ".jpg", blank, compression_params);
   
   //imshow("Threshold Result", blank);
}

int main(int argc, char* argv[])
{
   if (argc != 3)
   {
      cerr << "Program Usage: xml_image [xml_file.xml]" << '\n';
      exit(1);
   }
   string inputFile(argv[1]);

   auto const found = inputFile.find_last_of('.');
   auto filename = inputFile.substr(0,found);
   filename = argv[2];
   cout << "processing file: " << filename << endl;
   rapidxml::file<> xmlFile(argv[1]);
   rapidxml::xml_document<> doc;
   doc.parse<0>(xmlFile.data());

   //the root node is alto
   xml_node<> * root_node = doc.first_node();
   xml_node<> * layout_node = root_node->first_node("Layout");
   first_textblock = layout_node->first_node("Page")->
      first_node("PrintSpace")->first_node("TextBlock");

   //Mat blank(Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));

   namedWindow("Threshold Result", WINDOW_NORMAL);
   
   
   /* To Display the trackbars, uncommment me! 
   createTrackbar("heightThresh", "Threshold Result", &heightThresh, 3071,
         displayImage);

   
   createTrackbar("widthChar", "Threshold Result", &widthCharRatio, 3019,
         displayImage);
   
   createTrackbar("widthWord", "Threshold Result", &widthWordRatio, 19960,
         displayImage);
   
   createTrackbar("distThresh", "Threshold Result", &distThresh, 9100,
         displayImage);
   createTrackbar("charArea", "Threshold Result", &charArea, 606540, displayImage);
   createTrackbar("distAbove", "Threshold Result", &distAbove, 15080,
         displayImage);
   
   createTrackbar("leftThresh", "Threshold Result", &leftThresh, 300,
         displayImage);
   createTrackbar("rightThresh", "Threshold Result", &rightThresh, 300,
         displayImage);

   createTrackbar("nextThresh", "Threshold Result", &nextThresh, 300, displayImage);
   createTrackbar("prevThresh", "Threshold Result", &prevThresh, 300, displayImage);
   

   createTrackbar("diffThresh", "Threshold Result", &diffThresh, 200, displayImage);

   displayImage(0,0);
   */
   

   //To Display the trackbars, uncommment me! 

   displayImage(filename);
   
  // FILE WRITE
   
   ofstream o;
   o.open("validFiles.txt", std::ios::app);
   o << filename << std::endl;
  
   
   //waitKey();

   return 0;
}
