#include "textLine.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

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
      newWord.hPos = word->first_attribute("HPOS")->value();
      newWord.vPos = word->first_attribute("VPOS")->value();
      newWord.height = word->first_attribute("HEIGHT")->value();
      newWord.width = word->first_attribute("WIDTH")->value();
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

      words.push_back(newWord);

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
      text = this-words[i].content;

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
