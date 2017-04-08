#ifndef TEXTLINE_H
#define TEXTLINE_H

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

class Textline
{
   public:

      /* CONSTRUCTOR
       * Takes in an xml_node<>*, gathers line attributes,
       * and stores them in private variables
       */
      Textline(rapidxml::xml_node<>* textLine);

      //retrieve private variables
      int getHPOS();
      int getVPOS();
      int getWidth();
      int getHeight();
      bool getLabel();

      /* For each word in line, calculates (Area of Word) / (Num Characers in Word).
       * Sorts these ratios and returns the median ratio
       */
      double charAreaRatio();

      /* If allCaps = false, determines if the majority of the words
       * in the line begin with a capital letter.
       *
       * If allCaps = true, determines if every letter in the word
       * is a capital letter.
       *
       * These are determined by a ratio and a threshold for the ratio
       * to account for possible OCR errors.
       */
      bool capLine(bool allCaps);

      /* DEBUGGING TOOL
       * Determines if this line contains the word and is of numWords in length
       */
      bool isLine(string word, int wordCount);

      void printLine();

      //void PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, const cv::Scalar& color, int fontFace, double fontScale, int thickness = 1, int lineType = 8);

      /* Determines if the position of the left edge of the line is 
       * greater than that of the next line's and if the position of the
       * right edge of the line is less than that of the next line.
       *
       * If so, return true. Else, return false.
       *
       * Need to include this function somewhere else, because it requires knowledge
       * of lines below it.
       */

      //bool centeredLine(int leftGap, int rightGap);

      //struct for each word in line
      struct Word
      {
         int hPos;
         int vPos;
         int width;
         int height;
         string content; //actual word
         bool subContent;

         /*
         int getHPOS()
         {
            return hpos;
         }

         int getVPOS()
         {
            return vpos;
         }

         int getWidth()
         {
            return width;
         }

         int getHeight()
         {
            return height;
         }

         string getContent
         {
            return content;
         }
         */
      };

      vector<Word>& getWords();

   private:
      vector<Word> words;

      //attributes of line
      int hPos;
      int vPos;
      int width;
      int height;
      int numWords;
      bool hasHyphen;


      //THRESHOLD CONSTANTS
      const double ALLCAP = 0.7;
      const double FIRSTCAP = 0.74;


      /* LABEL FOR LINE
       * If title, title = true
       * If article, title = false
       */
      bool title;
};

#endif
