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
   
   /*
   Point rP1((int)(getHPOS(block)), (int)(getVPOS(block)));
   Point rP2(rP1.x + (int)(getObjectWidth(block)),
         rP1.y + (int)(getObjectHeight(block)));

   */
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
         if (isLine(textLine, "Police.", 3))
         {
            cout << "Word Count Text = " << text << '\n';
         }
         numWord++;
      }

      text = word->first_attribute("CONTENT")->value();
      
      
      if (isalpha(text[0]) && (isupper(text[0])) && (text.length() >= 1))
      {
         if (isLine(textLine, "Police.", 3))
         {
            cout << "Cap Word Text = " << text << '\n';
         }

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

   
   if (isLine(textLine, "Police.", 3))
   {
      cout << "numFirstCap = " << numFirstCap << '\n';
      cout << "numWord = " << numWord << '\n';
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
      //if (cap == true) //&& numWord > 1)
      if (((double)numFirstCap / numWord) >= 0.8)
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


//void displayImage(int, void*)
//bool displayImage(int charA)
void displayImage()
{
   //double min = 1000.0;
   //double max = 0.0;
   //double ratio = 0.0;
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

   for (rapidxml::xml_node<>* textBlock = first_textblock; textBlock != 0;
         textBlock = textBlock->next_sibling("TextBlock"))
   {
      numLine = 0;
      numBlock++;

      if (process)
      {
            //tempVpos = prevLoc;
            Block temp;
            temp.label = "Title";
            temp.y = vpos;
            temp.x = hpos;
            temp.height = tempVpos - vpos;
            temp.width = width;

            master.push_back(temp);

            Point rP1((int)(Xdimension * (temp.x/(double)pageWidth)), (int)(Ydimension * (temp.y/(double)pageHeight)));

            Point rP2(rP1.x + (int)(Xdimension * (temp.width/(double)pageWidth))
         ,rP1.y + (int)(Ydimension * (temp.height/(double)pageHeight)));

            //rectangle(blank, rP1, rP2, Scalar(0,0,0), 17);

            process = false;
      }

      vpos = getOrigVPOS(textBlock);
      hpos = getOrigHPOS(textBlock);
      width = getOrigWidth(textBlock);
      tempVpos = vpos;

      for (rapidxml::xml_node<>* textLine = textBlock->first_node("TextLine");
            textLine != 0; textLine = textLine->next_sibling("TextLine"))
      {
         if (!process)
         {
            vpos = getOrigVPOS(textLine);
            hpos = getOrigHPOS(textLine);
         }

         
         /*
         if (numLine == 0)
         {
            prevLoc = getOrigVPOS(textBlock->first_node("TextLine")) +
               getOrigHeight(textBlock->first_node("TextLine"));
         }
         */

         numLine++;
         /*

         if (isLine(textLine, "Townseiul", 3))
         {
            cout << "Townseiul bottomL = " << getOrigVPOS(textLine)
               + getOrigHeight(textLine) << '\n';
         }

         if (isLine(textLine, "Nears.", 3))
         {
            cout << "Nears. topL = " << getOrigVPOS(textLine) << '\n';
            cout << "Nears. bottomL = " << getOrigVPOS(textLine) +
               getOrigHeight(textLine) << '\n';
            //cout << "Nears. height = " << getObjectHeight(textLine) << '\n';
         }

         if (isLine(textLine, "Dies", 5))
         {
            cout << "Canton topL = " << getOrigVPOS(textLine) << '\n';
            cout << "Canton bottomL = " << getOrigVPOS(textLine) +
               getOrigHeight(textLine) << '\n';
         }

         if (isLine(textLine, "Quell", 3))
         {
            cout << "Quell topL = " << getOrigVPOS(textLine) << '\n';
            cout << "Quell bottomL = " << getOrigVPOS(textLine) +
               getOrigHeight(textLine) << '\n';
         }

         if (isLine(textLine, "Fracas", 3))
         {
            cout << "Fracas topL = " << getOrigVPOS(textLine) << '\n';
            cout << "Fracas bottomL = " << getOrigVPOS(textLine) + 
               getOrigHeight(textLine) << '\n';
         }

         */

         //check previous 4 lines
         
         /*
         if (numLine == 0)
         {
            dist1 = 0;
         }
         else
         {
            curLoc = getOrigVPOS(textLine);
            dist1 = curLoc - prevLoc;
            prevLoc = curLoc + getOrigHeight(textLine);
         }
         */

         cout << "Dist1 = " << dist1 << '\n';

         //tempDist1 = distAbove / (double)10;
         tempDist1 = 100.0;

         cout << "TempDist1 = " << tempDist1 << '\n';

         
         //Idea - check all lines whose words begin with capital letter and
         //check the min distance of that line from the line above and below it.
         //Make this a threshold -- TRY THIS
         
         //check next 4 lines
         dist2 = distNextFour(textLine);

         /*
         if (dist1 < dist2) 
            maxDist = dist1;
         else
            maxDist = dist2;
         */
         
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
         
         //distThresh = 2734;
         //distThresh = 2698;
         distThresh = 1611;
         //distThresh = charA;
         tempDist = distThresh / (double)10;
         //tempDist = 84.6;

         //widthCharRatio = 341;
         tempRatio = widthCharRatio / (double)10;
         tempRatio += 1.64348; //so min is 1.64348

         tempWordRatio = widthWordRatio / (double)10;
         tempWordRatio += 60.0;

         //heightThresh = heightVal;

         heightThresh = 438;
         //heightThresh = 444;
         tempHeight = heightThresh / (double)10;
         tempHeight += 11.9618;

         //charArea = 5561;
         //BELOW charArea - may work ???
         charArea = 4577;
         //charArea = charA; 
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
         
         /*
         if (isLine(textLine, "Police.", 3))
         {
            cout << "tempHeight = " << tempHeight << '\n';
            cout << "getObjectHeight = " << getObjectHeight(textLine) << '\n';
         }
         */
        
         if ((((getObjectHeight(textLine) > tempHeight) &&
                     capLine(textLine, false))||
               (capLine(textLine, true))))

         {
            drawBlock(textLine, Scalar(0,0,255));
            title = true;
         }

         else
         {
            //if ((maxDist >= tempDist) && ((cArea > tempCA) || 
             //        getWidthCharRatio(textLine) > tempRatio))
            /*
            if (isLine(textLine, "Police.", 3) && (capLine(textLine, false) == 0))
            {
               cout << "PROBLEM!!" << '\n';
            }
            */

            if (capLine(textLine, false) && (cArea > tempCA))
            {
               if ((dist2 >= tempDist))// || (dist1 >= tempDist1))
               {
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
                     drawBlock(textLine, Scalar(255,0,0));
                     title = false;
                  }
                  

                  else
                  {
                 
                     drawBlock(textLine, Scalar(0,0,255));
                     title = true;
                  } 
                  /*
                  (getWidthCharRatio(textLine, false) > tempRatio) || 
                     (getWidthCharRatio(textLine, true) > tempWordRatio))
                     */
                  //if (capLine(textLine, false))
               }
               else
               {
                  drawBlock(textLine, Scalar(255,0,0));
                  title = false;
               }
            }

            /*
            else if (capLine(textLine, false) && 
                  (getWidthCharRatio(textLine) > tempRatio))
            {
               drawBlock(textLine, Scalar(0,0,255));
            }
            */
            
            else
            {
               drawBlock(textLine, Scalar(255,0,0));
               title = false;
            }
         }

         //if process if false - we are starting a new block
         if (!process)
         {
            prevLoc = getOrigVPOS(textLine);
            prevCat = title; //remember - title is a boolean
            curCat = title;
         }

         process = true;

         else
         {
            curLoc = getOrigVPOS(textLine) + getObjectHeight(textLine);
            curCat = title;
         }

         //if (dist1 > tempDist1)
         if (curCat != prevCat)
         {
            cout << "FOUND!" << '\n';
            //tempVpos = prevLoc;
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
            //temp.height = tempVpos - vpos;
            temp.height = curLoc - prevLoc;
            temp.width = width;

            master.push_back(temp);

            //vpos = curLoc;
            //tempVpos = curLoc + getObjectHeight(textLine);

            Point rP1((int)(Xdimension * (temp.x/(double)pageWidth)), (int)(Ydimension * (temp.y/(double)pageHeight)));

            Point rP2(rP1.x + (int)(Xdimension * (temp.width/(double)pageWidth))
         ,rP1.y + (int)(Ydimension * (temp.height/(double)pageHeight)));

            if (prevCat == true)
            {
               //rectangle(blank, rP1, rP2, Scalar(100,100,0), 17);
            }
            else
            {
               //rectangle(blank, rP1, rP2, Scalar(200,100,200), 17);
            }

            process = false;
         }

         /*
         else
         {
            tempVpos = curLoc + getOrigHeight(textLine);
         }
         */

         prevCat = curCat;

      } //for

      if (numBlock == 7)
      {
         break;
      }
      
   } //for textblock

   if (process)
   {
      tempVpos = prevLoc;
      Block temp;
      temp.label = "Title";
      temp.y = vpos;
      temp.x = hpos;
      temp.height = tempVpos - vpos;
      temp.width = width;

      master.push_back(temp);

      Point rP1((int)(Xdimension * (temp.x/(double)pageWidth)), (int)(Ydimension * (temp.y/(double)pageHeight)));

      Point rP2(rP1.x + (int)(Xdimension * (temp.width/(double)pageWidth))
   ,rP1.y + (int)(Ydimension * (temp.height/(double)pageHeight)));

      rectangle(blank, rP1, rP2, Scalar(0,0,0), 17);

      process = false;
   }
   
   cout << "Max Dist: " << gMaxDist << '\n';
   cout << "Min Dist: " << gMinDist << '\n';
   
   
   vector<int> compression_params;
   compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
   compression_params.push_back(50);
   
   imwrite("segImage.jpg", blank, compression_params);
     

   //imshow("Threshold Result", blank);

   //return reached;
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
   
   /*
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

   */
   
   
   //createTrackbar("distThresh", "Threshold Result", &dist, 
   
   //createTrackbar("BlockHeight", "Threshold Result", &blockThresh, 7164, displayBlock);
   
   //cout << "Height = " << Ydimension * (168.0/pageHeight) << '\n';


   //displayImage(0,0);
   
   /*
   for (int i = 2698; i > 400; i--)
   {
      cout << "charArea = " << i << '\n';
      if(displayImage(i))
      {
         cout << "Reached, I = " << i << '\n';
         break;
      }
   }
   */

   displayImage();
   
   //waitKey();

   return 0;
}


