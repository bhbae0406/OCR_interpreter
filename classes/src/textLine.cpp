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

//Textline::Textline(rapidxml::xml_node<>* textLine)
Textline::Textline(vector<rapidxml::xml_node<>*>& words)
{
  this->numWords = 0;
  //these value will store the best attributes for the line
  int bestHPOS = 0;
  int bestVPOS = 0;
  int bestRightHPOS = 0;
  int bestBottomVPOS = 0;

  //assume every line is not multiColumn for now
  this->multiColumn = false;

  int prevHPOS = 0;

  //for (rapidxml::xml_node<>* word = textLine->first_node("String");
  //    word != 0; word = word->next_sibling("String"))
  for (rapidxml::xml_node<>* word : words)
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

  this->confDone = false;
  this->numConf = 0;
}

bool Textline::valueInRange(double value, double min, double max)
{
  return (value >= min) && (value <= max);
}

//taken from stack overflow
bool Textline::lineInBlock(Block invalid)
{
  bool xOverlap = valueInRange(double(this->hPos), invalid.getX(), 
      invalid.getX() + invalid.getWidth()) ||
    valueInRange(invalid.getX(), double(this->hPos), double(this->hPos + this->width));

  bool yOverlap = valueInRange(double(this->vPos), invalid.getY(), invalid.getY() 
      + invalid.getHeight()) ||
    valueInRange(invalid.getY(), double(this->vPos), double(this->vPos + this->height));

  return xOverlap && yOverlap;
}

int Textline::getHPOS() const
{
  return hPos;
}

int Textline::getVPOS() const
{
  return vPos;
}

int Textline::getWidth() const
{
  return width;
}

int Textline::getHeight() const
{
  return height;
}

int Textline::getNumConf() const
{
  return this->numConf;
}

double Textline::getConfidence() const
{
  return this->confidence;
}

bool Textline::getLabel() const
{
  return title;
}

bool Textline::hyphen() const
{
  return hasHyphen;
}

bool Textline::isMulti()
{
  return this->multiColumn;
}

bool Textline::isVisited()
{
  return this->visited;
}

bool Textline::isConfDone()
{
  return this->confDone;
}

void Textline::setLabel(bool value)
{
  this->title = value;
}

void Textline::setVisited()
{
  this->visited = true;
}

void Textline::setID(int value)
{
  this->ID = value;
}

void Textline::setSubID(int value)
{
  this->subID = value;
}

void Textline::setMultiCol()
{
  this->multiColumn = true;
}

void Textline::setConfidence(double val)
{ 
  this->confidence = val;
}

void Textline::setConfDone()
{
  this->confDone = true;
}

void Textline::setConfFalse()
{
  this->confDone = false;
}

void Textline::setNumConf(int val)
{
  this->numConf = val;
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

double Textline::capitalWordsRatio(bool allCaps)
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
    return ((double)numCap /numChar);
  }
  else
  {
    return ((double)numFirstCap / numWord);
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

  if (wordCount >= 0)
  {
    if (found && (this->numWords == wordCount))
      return true;
    else
      return false;
  }
  else
  {
    if (found)
      return true;
    else
      return false;
  }
}

void Textline::printLine()
{
  for (int i = 0; i < (this->numWords) - 1; i++)
  {
    cout << this->words[i].content << " ";
  }

  cout << this->words[this->numWords - 1].content << '\n';
}

bool Textline::continuedTag()
{
  if ((numWords >= 2) && (boost::iequals((this->words[0]).content, "continued")) &&
      (boost::iequals((this->words[1]).content, "on")))
  {
    return true;
  }
  else
    return false;
}

Textline& Textline::operator=(const Textline& rhs){
  this->words = rhs.words;
  this->hPos = rhs.hPos;
  this->vPos = rhs.vPos;
  this->width = rhs.width;
  this->height = rhs.height;
  this->numWords = rhs.numWords;
  this->hasHyphen = rhs.hasHyphen;
  this->multiColumn = rhs.multiColumn;
  this->title = rhs.title;
  this->visited = rhs.visited;
  this->subID = rhs.subID;
  this->ID = rhs.ID;
  return *this;
}

bool Textline::operator==(const Textline& rhs)
{
  if ((this->hPos == rhs.hPos)
      && (this->vPos == rhs.vPos)
      && (this->width == rhs.width)
      && (this->height == rhs.height))
  {
    return true;
  }
  else
  {
    return false;
  }
}

vector<Textline::Word>& Textline::getWords()
{
  return this->words;
}
