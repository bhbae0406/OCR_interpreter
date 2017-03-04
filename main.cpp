#include "parser.h"
#include <iostream>
#include <string>

using namespace cv;
using namespace rapidxml;
using namespace std;

Parser parse(19268, 28892, 9500, 9600);

void displayImage(int, void*)
{
   for(rapidxml::xml_node<>* textblock = parse.first_textBlock; textblock != 0;
         textblock = textblock->next_sibling("textblock"))
   {   
      if(!(parse.validTextBlock(textblock)))
         continue;
      else
      {   
         for (rapidxml::xml_node<>* textline = textblock->first_node("textline");
               textline != 0; textline = textline->next_sibling("textline"))
         {
            cout << "Object Width: " << parse.getObjectWidth(textline) << '\n';

            if ((parse.getObjectWidth(textline) > parse.widthThresh) &&  
                  (parse.getNumStrings(textline) > parse.numThresh))
            {
               parse.drawBlock(textline, Scalar(0,0,255));
            }
            else
            {
               parse.drawBlock(textline, Scalar(255,0,0));
            }
         }

      }   
   }

   imshow("Output", parse.outImg);
}


void testFontThreshold(const char* xmlFile)
{
   parse.initPicture();

   parse.first_textBlock = parse.readImage(xmlFile);

   namedWindow("Threshold Result", WINDOW_AUTOSIZE);

   parse.widthThresh = 0;
   parse.numThresh = 0;

   createTrackbar("widthThresh", "Threshold Result", &(parse.widthThresh), 14, 
         displayImage);
   createTrackbar("numThresh", "Threshold Result", &(parse.numThresh), 14,
         displayImage);
   displayImage(0,0);
   waitKey();
}

int main(int argc, char* argv[])
{
   if (argc != 2)
   {
      cerr << "Program Usage: ./program_name [xml_file]" << '\n';
      exit(1);
   }

   testFontThreshold(argv[1]);

   return 0;
}

