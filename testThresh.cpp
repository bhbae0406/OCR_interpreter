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

int pageWidth = 19268;
int pageHeight = 28892;
int Xdimension = 9500;
int Ydimension = 9600;

Mat blank(Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));

rapidxml::xml_node<>* first_textblock;

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

   //double width = atoi(textLine->first_attribute("WIDTH")->value());

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

double getVPOS(rapidxml::xml_node<>* block) {
   int Vpos = atoi(block->first_attribute("VPOS")->value());
   return (Ydimension * (Vpos/(double)pageHeight));
}

double getHPOS(rapidxml::xml_node<>* block) {
   int Vpos = atoi(block->first_attribute("HPOS")->value());
   return (Ydimension * (Vpos/(double)pageHeight));
}


bool capLine(rapidxml::xml_node<>* textLine, bool allCaps)
{
   int numChar = 0;
   int numCap = 0;

   bool cap = true;

   int numWord = 0;

   string text;

   for (rapidxml::xml_node<>* word = textLine->first_node("String"); word != 0;
         word = word->next_sibling("String"))
   {
      numWord++;

      text = word->first_attribute("CONTENT")->value();
      
      
      if (isalpha(text[0]) && !(isupper(text[0])) && (text.length() > 1))
         cap = false;
    
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
      if ((((double)numCap)/numChar) > 0.3)
         return true;
      else
         return false;
   }
   else
   {
      if (cap == true) //&& numWord > 1)
         return true;
      else
         return false;
   }
}

double distNextFour(rapidxml::xml_node<>* textLine)
{
   double prevLoc = getVPOS(textLine);
   double curLoc = getVPOS(textLine);

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

            
            /*
            if ((tempLine = (tempLine->next_sibling("TextLine"))) != NULL)
            {
               curLoc = getVPOS(tempLine);
            }
            */
         }
         
         
      }
   }

   dist = curLoc - prevLoc;

   return dist;
}

/*
bool distSecond(rapidxml::xml_node<>* textLine)
{
   double curLoc = getVPOS(textLine) + getObjectHeight(textLine);
   double nextLoc = curLoc;

   rapidxml::xml_node<>* tempLine = NULL;

   if ((tempLine = (textLine->next_sibling("TextLine"))) != NULL)
   {
      nextLoc = getVPOS(tempLine);
   }

   if (

*/

double charAreaRatio(rapidxml::xml_node<>* textLine)
{
   string text;
   double area = 0.0;
   int numChar = 0.0;

   //double avgRatio = 0.0;

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

   //cout << "Ratio = " << ratio[midIdx] << '\n';
   return ratio[midIdx];
}


void displayImage(int, void*)
//void displayImage()
{
   //double min = 1000.0;
   //double max = 0.0;
   //double ratio = 0.0;
   double tempRatio = 0.0;
   double tempWordRatio = 0.0;
   double tempHeight = 0.0;
   double tempDist = 0.0;
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

   for (rapidxml::xml_node<>* textBlock = first_textblock; textBlock != 0;
         textBlock = textBlock->next_sibling("TextBlock"))
   {
      numLine = 0;

      for (rapidxml::xml_node<>* textLine = textBlock->first_node("TextLine");
            textLine != 0; textLine = textLine->next_sibling("TextLine"))
      {
         prevLoc = getVPOS(textBlock->first_node("TextLine"));

         numLine++;

         //check previous 4 lines
         
         
         if ((numLine >= 2) && (numLine % 2 == 0))
         {
            curLoc = getVPOS(textLine);
            dist1 = curLoc - prevLoc;
            prevLoc = curLoc;
         }
         
         //Idea - check all lines whose words begin with capital letter and
         //check the min distance of that line from the line above and below it.
         //Make this a threshold -- TRY THIS
         
         //check next 4 lines
         dist2 = distNextFour(textLine);

         
         if (dist1 < dist2) 
            maxDist = dist1;
         else
            maxDist = dist2;
         
         cArea = charAreaRatio(textLine);
         temp = getWidthCharRatio(textLine, true);

         if (temp > gMaxDist)
         {
            gMaxDist = temp;
         }
         if (temp < gMinDist)
         {
            gMinDist = temp;
         }
         
         //distThresh = 2734;
         distThresh = 2698;
         tempDist = distThresh / (double)10;

         //widthCharRatio = 341;
         tempRatio = widthCharRatio / (double)10;
         tempRatio += 1.64348; //so min is 1.64348

         tempWordRatio = widthWordRatio / (double)10;
         tempWordRatio += 60.0;

         heightThresh = 444;
         tempHeight = heightThresh / (double)10;
         tempHeight += 11.9618;

         charArea = 5561;
         tempCA = charArea / (double)10;
         tempCA += 315;


         /* Absolute - if all words are capitalized, then no false negatives
          * for titles. Also, if all letters are capitalized, it is indeed a title.
          */
          
         /*
         if (capLine(textLine, false) || capLine(textLine, true))
         {
            drawBlock(textLine, Scalar(0,0,255));
         }

         else
         {
            //false positives - when article text is considered a title
            drawBlock(textLine, Scalar(255,0,0));
         }
         */
         
        
         if (((getObjectHeight(textLine) > tempHeight) ||
               (capLine(textLine, true))))

         {
            drawBlock(textLine, Scalar(0,0,255));
         }

         else
         {
            //if ((maxDist >= tempDist) && ((cArea > tempCA) || 
             //        getWidthCharRatio(textLine) > tempRatio))
            if (capLine(textLine, false) && (cArea > tempCA))
            {
               if ((maxDist >= tempDist))// || 
                  /*
                  (getWidthCharRatio(textLine, false) > tempRatio) || 
                     (getWidthCharRatio(textLine, true) > tempWordRatio))
                     */
               //if (capLine(textLine, false))
                  drawBlock(textLine, Scalar(0,0,255));
               else
                  drawBlock(textLine, Scalar(255,0,0));
            }

            /*
            else if (capLine(textLine, false) && 
                  (getWidthCharRatio(textLine) > tempRatio))
            {
               drawBlock(textLine, Scalar(0,0,255));
            }
            */
            
            else
               drawBlock(textLine, Scalar(255,0,0));
         }
         
      }
   }

   
   cout << "Max Dist: " << gMaxDist << '\n';
   cout << "Min Dist: " << gMinDist << '\n';
   

   /*
   vector<int> compression_params;
   compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
   compression_params.push_back(95);
   
   imwrite("segImage.jpg", blank, compression_params);
   */

   imshow("Threshold Result", blank);
}

/*
void displayBlock(int, void*)
{
   //double prev_v = getVPOS(first_textblock);
   xml_node<>* temp = NULL;
   double prev_v = 0.0;
   //double prev_h1 = getHPOS(first_textblock);
   double prev_h1 = 0.0;
   double prev_h2 = prev_h1 + getObjectWidth(first_textblock);
   double tempBlock = 10000.0;

   //Point p1(prev_h1, prev_v);
   //Point p2(prev_h2, prev_v);
   Point p1;
   Point p2;
   //Point p3;
   //Point p4;

   double min = 100000;
   double max = 0;
   double diff = 0;
   double lineV = 0;
   double horizPos = 0;
   double bWidth = 0;
   
   for (xml_node<>* textBlock = first_textblock; textBlock != 0;
         textBlock = textBlock->next_sibling("TextBlock"))
   {
      temp = textBlock->first_node("TextLine");
      bWidth = getObjectWidth(textBlock);
      horizPos = getHPOS(textBlock);

      if (temp != NULL)
      {
         prev_v = getVPOS(textBlock->first_node("TextLine"));
         //cout << "H" << '\n';
         
         for (rapidxml::xml_node<>* textLine = temp->
               next_sibling("TextLine"); textLine != 0; 
               textLine = textLine->next_sibling("TextLine"))
         {
            tempBlock = blockThresh / (double)10;
            //tempBlock += 6.64544;
            
            lineV = getVPOS(textLine);
            diff = abs(prev_v - lineV);
            
            
           
            if (diff < min)
            {
               min = diff;
            }
            if (diff > max)
            {
               max = diff;
            }
            
            
            
            if (abs(prev_v - getVPOS(textLine)) > tempBlock) 
            {
               line(blank, Point(horizPos, lineV), Point(horizPos + bWidth, lineV),
                     Scalar(255,0,0), 10);
               

               
               p1 = Point(
               p1 = Point(prev_h1, prev_v);
               p4 = Point(prev_h2, prev_v);
               rectangle(blank, p1, p4, Scalar(0,0,255), 7);
               // segmentBlock(p1, p2, p3, p4 , Scalar(0,0,255));

               prev_v = getVPOS(textLine);
               prev_h1 = getHPOS(textLine);
               prev_h2 = prev_h1 + getObjectWidth(textLine);
               p1 = Point(prev_h1, prev_v);
               p2 = Point(prev_h2, prev_v);
            } else {
               prev_v = getVPOS(textLine);
               prev_h1 = getHPOS(textLine);
               prev_h2 = prev_h1 + getObjectWidth(textLine);
            }
           
             }
            prev_v = getVPOS(textLine);
            
         }
      }
   }

   cout << "Min: " << min << '\n';
   cout << "Max: " << max << '\n';
  
   imshow("Threshold Result", blank);
}
*/


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

   //the root node is alto
   xml_node<> * root_node = doc.first_node();
   xml_node<> * layout_node = root_node->first_node("Layout");
   first_textblock = layout_node->first_node("Page")->
      first_node("PrintSpace")->first_node("TextBlock");

   //Mat blank(Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));

   namedWindow("Threshold Result", WINDOW_NORMAL);
   
   
   
   createTrackbar("heightThresh", "Threshold Result", &heightThresh, 3071,
         displayImage);

   
   createTrackbar("widthChar", "Threshold Result", &widthCharRatio, 3019,
         displayImage);
   
   createTrackbar("widthWord", "Threshold Result", &widthWordRatio, 19960,
         displayImage);
   
   createTrackbar("distThresh", "Threshold Result", &distThresh, 9100,
         displayImage);

   createTrackbar("charArea", "Threshold Result", &charArea, 606540, displayImage);
   
   /*
   createTrackbar("BlockHeight", "Threshold Result", &blockThresh, 7164,
         displayBlock);
   */

   displayImage(0,0);
   //displayImage();
   waitKey();

   return 0;
}


