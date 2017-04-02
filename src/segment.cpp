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

Segment::Segment(char* filename)
{
   rapidxml::file<> xmlFile(filename);
   rapidxml::xml_document<> doc;
   doc.parse<0>(xmlFile.data());

   xml_node<>* root_node = doc.first_node();
   xml_node<>* layout_node = root_node->first_node("Layout");
   xml_node<>* page_node = layout_node->first_node("Page");

   this->pageWidth = page_node->first_attribute("WIDTH")->value();
   this->pageHeight = page_node->first_attribute("HEIGHT")->value();
   this->numLines = 0; 

   first_textBlock = page_node->first_node("PrintSpace")->first_node("TextBlock");

   for (rapidxml::xml_node<>* textBlock = first_textBlock; textBlock != 0;
         textBlock = textBlock->next_sibling("TextBlock"))
   {
      for (rapidxml::xml_node<> textLine = textBlock->first_node("TextLine");
            textLine != 0; textLine = textLine->next_sibling("TextLine"))
      {
         Textline newLine(textLine);

         this->lines.push_back(newLine);
         this->numLines += 1
      }
   }
}

void Segment::printLines()
{
   for (int i = 0; i < numLines; i++)
   {
      lines[i].printLine();
   }
}

void Segment::sortLines()
{
       
}

void Segment::PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, 
      const cv::Scalar& color, int fontFace, double fontScale, int thickness = 1, 
      int lineType = 8)
{


}

void Segment::drawBlock(cv::Scalar color)
{


}


   
