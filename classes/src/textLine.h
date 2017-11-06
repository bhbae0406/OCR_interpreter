#ifndef TEXTLINE_H
#define TEXTLINE_H

#include "block.h"
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
      //Textline(rapidxml::xml_node<>* textLine);
      Textline(vector<rapidxml::xml_node<>*>& words);

      //found in stack overflow
      //http://stackoverflow.com/questions/306316/determine-if-two-rectangles-overlap-each-other
      bool valueInRange(double value, double min, double max);

      bool lineInBlock(Block invalid);

      //retrieve private variables
      int getHPOS() const;
      int getVPOS() const;
      int getWidth() const;
      int getHeight() const;
      bool getLabel() const;
      bool hyphen() const;

      bool isMulti();
      bool isVisited();

      bool isConfDone();

      //sets label to true or false based on classification
      void setLabel(bool value);
      void setVisited();

      void setID(int value);
      void setSubID(int value);

      void setMultiCol();

      void setConfidence(double val);

      void setConfDone();

      /* For each word in line, calculates (Area of Word) / (Num Characters in Word).
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

      double capitalWordsRatio(bool allCaps);

      /* DEBUGGING TOOL
       * Determines if this line contains the word and is of numWords in length
       * 
       * Note -- if wordCount < 0, then isLine does not consider wordCount
       */
      bool isLine(string word, int wordCount);

      void printLine();

      bool continuedTag();

      Textline& operator=(Textline const &rhs);
      bool operator==(const Textline& rhs);

      //struct for each word in line

      struct Word
      {
         int hPos;
         int vPos;
         int width;
         int height;
         string content; //actual word
         bool subContent;
      };

      vector<Word>& getWords();
   
      vector<Word> words;

   private:
      //attributes of line
      int hPos;
      int vPos;
      int width;
      int height;
      int numWords;
      bool hasHyphen;

      bool multiColumn;

      double confidence;

      //THRESHOLD CONSTANTS
      const double ALLCAP = 0.7;
      const double FIRSTCAP = 0.74;

      /* LABEL FOR LINE
       * If title, title = true
       * If article, title = false
       */
      bool title;

      bool visited; //will be used in linking stage

      bool confDone; //"visited" for determining confidence levels

      //used for linking and evaluation
      int subID;
      int ID;
};

#endif
