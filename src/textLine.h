#ifndef TEXTLINE_H
#define TEXTLINE_H

#include <string>
#include <vector>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

using namespace std;

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

      /* If true --- line contains a hypenated word
       * Else, false
       */

      /* INCLUDED AS A PRIVATE VARIABLE - need to remove comment once code works */
      //bool includeHypen();

   private:
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
