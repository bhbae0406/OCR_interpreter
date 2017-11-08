#include "segment.h"
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
#include <unordered_map>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

using namespace std;
using namespace cv;
using namespace rapidxml;

double Segment::convertToXML_h(double in)
{
  return in * (double) pageWidth / (double)dimX;
}

double Segment::convertToXML_v(double in)
{
  return in * (double) pageHeight / (double)dimY;
}

double Segment::convertToImage_X(double inX)
{
  return inX * (double) dimX / (double)pageWidth;
}

double Segment::convertToImage_Y(double inY)
{
  return inY * (double) dimY / (double)pageHeight;
}

void Segment::splitByColumn() 
{
  //double - x coord of middle of each column
  //int - column id
  unordered_map <double,int> column_dict;

  //stores the minimum xCoord for each column
  vector<int> rangeMin;

  //stores the maximum xCoord for each column
  vector<int> rangeMax;

  int num_columns = 0;
  int count_notACol = 0;

  //PARAMETERS 
  
  //best slackupperW = 0.4
  //best slacklowerW = 0.6
  double slackupper = 1.0 + (slackupperW * 0.5); 
  //min is 1.0, max is 1.5 -- best is 1.2
  
  double slacklower = 0.5 + (slacklowerW * 0.5); 
  //max is 1.0, min is 0.5 -- best is 0.8

  columnLengthComparator comp;
  std::sort(this->lines.begin(), this->lines.end(), comp);

  //store the average width of all the lines
  double column_len = lines[lines.size()/2].getWidth();
  bool debug = false;
  for (auto curLine : this->lines) 
  {
    //this line is a temporary fix for removing the header of the newspaper
    //NOTE - this only works for 0033.xml
    
    
    /*
    
       if ((curLine.getVPOS() + curLine.getHeight()) <= (3616 + 20))
       {
       continue;
       }
    
    */

    // is a valid column - determined by width of line (with slack)
    //  This mitigates the effect of OCR errors
    
    if (curLine.isLine("National", 3))
    {
      cout << "HERE!" << '\n';
    }

    if ((curLine.getWidth() < column_len*slackupper)
        && (curLine.getWidth() > column_len*slacklower)) 
    {
      bool newColumn = true;
      debug = false;
      for (auto mid : column_dict) 
      {
        //cout << this->columns[column_dict[mid.first]][0].words[0].content << '\n';

        // searching for column in the dictionary...
        if((curLine.getHPOS() < mid.first) 
            && ((curLine.getHPOS() + curLine.getWidth()) > mid.first)) 
        {
          //when the line is classified as being part of more than one column 
          
          if (debug == true) 
          {
            /*
            cout << "error!! " << endl;
            cout << "current column median: " << mid.first << endl;
            cout << "current block x: " << curLine.getHPOS() << endl;
            cout << "current block width: " << curLine.getWidth() << endl;
            cout << "columns should be of width: " << column_len << endl;
            */

            continue;
          }
         

          this->columns[column_dict[mid.first]].push_back(curLine);
          debug = true;
          newColumn = false;

          if (curLine.getHPOS() < rangeMin[column_dict[mid.first]])
          {
            rangeMin[column_dict[mid.first]] = curLine.getHPOS();
          }

          if ((curLine.getHPOS() + curLine.getWidth())
              > rangeMax[column_dict[mid.first]])
          {
            rangeMax[column_dict[mid.first]] = curLine.getHPOS() + curLine.getWidth();
          }
        }
      }

      if (newColumn) 
      {
        vector<Textline> column;
        column.push_back(curLine);
        this->columns.push_back(column);
        column_dict[curLine.getHPOS() + curLine.getWidth()/2.0] = num_columns;
        num_columns++;

        rangeMin.push_back(curLine.getHPOS());
        rangeMax.push_back(curLine.getHPOS() + curLine.getWidth());
      }
    } 

    else 
    {
      count_notACol++;
    
      if ((curLine.getWidth() > column_len*slackupper)
          && (curLine.getWidth() > column_len*slacklower)) 
      {
        curLine.setMultiCol();

        this->nonSingleLines.push_back(curLine);
      }
      else
      {
        this->smallWidthLines.push_back(curLine);
      }
    }
  }

  debug = false;

  /* Second Pass:
   *    Accounts for lines that failed the mid check, but are
   *    still in the column.
   */

  //PARAMETERS
  
  //best rangeSlackLowerW = 0.95
  //best rangeSlackHigherW = 0.0
  double rangeSlackLower = 0.8 + (rangeSlackLowerW * 0.2); 
  //max is 1.0, min is 0.8 -- best is 0.99
  double rangeSlackHigher = 1.0 + (rangeSlackHigherW * 0.4); 
  //min is 1.0, max is 1.4 -- best is 1.00

  for (auto curLine : this->smallWidthLines) 
  {
    debug = false;
    for (auto mid : column_dict) 
    {
      // searching for column in the dictionary...
      if((curLine.getHPOS() >= ((rangeMin[column_dict[mid.first]] * rangeSlackLower))) 
          && ((curLine.getHPOS() + curLine.getWidth()) <= 
            (rangeMax[column_dict[mid.first]] * rangeSlackHigher))) 
      {
        /*
        if (curLine.isLine("nati.", 1))
        {
          cout << "GOT HERE - nati, passed small width test" << '\n';
          cout << "MIN RANGE = " << rangeMin[column_dict[mid.first]] * rangeSlackLower << '\n';
          cout << "MAX RANGE = " << rangeMax[column_dict[mid.first]] * rangeSlackLower << '\n';
        }
  
        if (curLine.isLine("territory", 4))
        {
          cout << "GOT HERE - passed small width test" << '\n';
        }
        */

        //when the line is classified as being part of more than one column 
        
        if (debug == true) 
        {
          /*
          cout << "error!! " << endl;
          cout << "current column median: " << mid.first << endl;
          cout << "current block x: " << curLine.getHPOS() << endl;
          cout << "current block width: " << curLine.getWidth() << endl;
          cout << "current range Min: " << rangeMin[column_dict[mid.first]] << endl;
          cout << "current range Max: " << rangeMax[column_dict[mid.first]] << endl;
          cout << "columns should be of width: " << column_len << endl;
          */

          continue;
        }
        

        this->columns[column_dict[mid.first]].push_back(curLine);
        debug = true;
      }
    }
  }

  //now place multi-line columns inside the column vector
  //repeat them for every column the line spans

  //PARAMETERS 
  
  //best minThresholdW = 0.6
  //best maxThresholdW = 0.225
  double minThreshold = 0.9 + (minThresholdW * 0.1); 
  //min 0.9, max 1.0 -- best is 0.96
  double maxThreshold = 1.0 + (maxThresholdW * 0.4);
  //min 1.0, max 1.4 -- best is 1.09

  for (int i = 0; i < this->columns.size(); i++)
  {
    for (auto curLine: this->nonSingleLines)
    {
      if ( (rangeMin[i] < ((curLine.getHPOS() + curLine.getWidth()) * minThreshold))
          && (rangeMax[i] > (curLine.getHPOS() * maxThreshold)))
      {
        this->columns[i].push_back(curLine);
      }
    }
  }

  /* Now sort the column vectors */
  vector<int> indexSort;

  for (int i = 0; i < rangeMin.size(); i++)
  {
    indexSort.push_back(i);
  }

  std::sort(indexSort.begin(), indexSort.end(),
      [&rangeMin](int a, int b){ return (rangeMin[a] < rangeMin[b]);});

  for (int i = 0; i < indexSort.size(); i++)
  {
    sortedColumns.push_back(columns[indexSort[i]]);
  }

  for (int i = 0; i < sortedColumns.size(); i++)
  {
    sort(sortedColumns[i].begin(), sortedColumns[i].end(),
        [](Textline a, Textline b){ return(a.getVPOS() < b.getVPOS());});
  }

}

void Segment::readParam(const string& param_json)
{
  std::stringstream ss;
  std::ifstream file(param_json);
  bool notfound = false;
  vector<Block> invalid_zones;
  if (file) {
    ss << file.rdbuf(); 
    file.close();
  } else {
    std::cerr << "Parameter JSON not found!" << std::endl;
    exit(1);
  }

  const std::string tmp = ss.str();
  const char* json = tmp.c_str();
  rapidjson::Document d;
  d.Parse(json);

  const rapidjson::Value& xmlParam = d["xml"];

  slackupperW = static_cast<double>(xmlParam["slackupperW"].GetDouble());
  slacklowerW = static_cast<double>(xmlParam["slacklowerW"].GetDouble());
  rangeSlackLowerW = static_cast<double>(xmlParam["rangeSlackLowerW"].GetDouble());
  rangeSlackHigherW = static_cast<double>(xmlParam["rangeSlackHigherW"].GetDouble());
  minThresholdW = static_cast<double>(xmlParam["minThresholdW"].GetDouble());
  maxThresholdW = static_cast<double>(xmlParam["maxThresholdW"].GetDouble());
  weightHeight = static_cast<double>(xmlParam["weightHeight"].GetDouble());
  weightCapitalWords = static_cast<double>(
                          xmlParam["weightCapitalWords"].GetDouble());
  weightCapitalLetters = static_cast<double>(
                          xmlParam["weightCapitalLetters"].GetDouble());
  weightCharArea = static_cast<double>(xmlParam["weightCharArea"].GetDouble());
}

vector<Block> Segment::generate_invalid_zones(const string& json_file)
{
  std::stringstream ss;
  std::ifstream file(json_file);
  bool notfound = false;
  vector<Block> invalid_zones;
  if (file) {
    ss << file.rdbuf(); 
    file.close();
  } else {
    std::cout << "JSON result from image processing not found!!!" << std::endl;
    return invalid_zones;
  }
  const std::string tmp = ss.str();
  const char* json = tmp.c_str();
  rapidjson::Document d;
  d.Parse(json);

  const rapidjson::Value& annotations = d["annotations"];
  for (rapidjson::SizeType i = 0; i < annotations.Size(); i++){

    Block curBlock(convertToXML_h(static_cast<double> (annotations[i]["x"].GetDouble())), 
        convertToXML_v(static_cast<double> (annotations[i]["y"].GetDouble())), 
        convertToXML_v(static_cast<double> (annotations[i]["height"].GetDouble())), 
        convertToXML_h(static_cast<double> (annotations[i]["width"].GetDouble())));
    invalid_zones.push_back(curBlock);
  }
  cout << json_file << " had " << invalid_zones.size() << " invalid zones!" << endl;
  return invalid_zones;
}

void Segment::setDim(char* dimXcoord, char* dimYcoord)
{
  string dimXString(dimXcoord);
  string dimYString(dimYcoord);

  this->dimX = stoi(dimXString);
  this->dimY = stoi(dimYString);
}

Segment::Segment(char* filename, char* dimX, char* dimY, char* param_json) 
{
  bool skipLine = false;

  //initialize all thresholds

  heightThresh = (438 / (double)10) + 11.9618;
  diffThresh = 1611;
  CAThresh = (4577 / (double)10) + 315;
  distThresh = (1611 / (double)10); 
  prevThresh = 108;
  nextThresh = 63;
  
  string paramJson(param_json);

  readParam(paramJson);

  rapidxml::file<> xmlFile(filename);
  rapidxml::xml_document<> doc;
  doc.parse<0>(xmlFile.data());

  xml_node<>* root_node = doc.first_node();
  xml_node<>* layout_node = root_node->first_node("Layout");
  xml_node<>* page_node = layout_node->first_node("Page");

  this->pageWidth = atoi(page_node->first_attribute("WIDTH")->value());
  this->pageHeight = atoi(page_node->first_attribute("HEIGHT")->value());
  this->numLines = 0; 

  this->Xdimension = 9500;
  this->Ydimension = 9600;

  setDim(dimX, dimY);

  //vector<Block> invalidZones = this->generate_invalid_zones(json_file);
  
  int wordPrevVPOS = 0;
  int wordCurVPOS = 0;

  int wordPrevHeight = 0;
  int wordCurHeight = 0;
  int wordPrevWidth = 0;
  int wordCurWidth = 0;

  int curNumWords = 0;

  //temp variable
  int a = 0;
  string content;

  vector<rapidxml::xml_node<>*> words;
  rapidxml::xml_node<>* wordTemp;

  xml_node<>* first_textBlock = page_node->first_node("PrintSpace")
    ->first_node("TextBlock");

  for (rapidxml::xml_node<>* textBlock = first_textBlock; textBlock != 0;
      textBlock = textBlock->next_sibling("TextBlock"))
  {
    for (rapidxml::xml_node<>* textLine = textBlock->first_node("TextLine");
        textLine != 0; textLine = textLine->next_sibling("TextLine"))
    {
      curNumWords = 0;
      wordPrevVPOS = 0;
      wordCurVPOS = 0;
      wordPrevHeight = 0;
      wordCurHeight = 0;
      wordPrevWidth = 0;
      wordCurWidth = 0;

      words.clear();

      for (rapidxml::xml_node<>* word = textLine->first_node("String");
          word != 0; word = word->next_sibling("String"))
      {
        //words.push_back(word);

        content = word->first_attribute("CONTENT")->value();
        
        wordCurVPOS = atoi(word->first_attribute("VPOS")->value());
        wordCurHeight = atoi(word->first_attribute("HEIGHT")->value());
        wordCurWidth = atoi(word->first_attribute("WIDTH")->value());

        words.push_back(word);

        if (curNumWords > 0)
        {
          if (abs(wordCurHeight - wordPrevHeight) > 2000 ||
              abs(wordCurWidth - wordPrevWidth) > 2000)
          {
            words.pop_back(); //skip word
          }

          if (abs(wordCurVPOS - wordPrevVPOS) > 60) 
          {
            wordTemp = words.back();
            words.pop_back();
            Textline newLine(words);
            this->lines.push_back(newLine);
            this->numLines += 1;
            words.clear();
            words.push_back(wordTemp);
          }
        }

        wordPrevVPOS = wordCurVPOS;
        wordPrevHeight = wordCurHeight;
        wordPrevWidth = wordCurWidth;

        curNumWords++;
      }

      Textline newLine(words);
      //Textline newLine(textLine);

      //origLines.push_back(textLine);

      //used to ignore any lines that fall into invalid regions
      //as indicated by .json file given from image processing
      skipLine = false;

      /*
         for (int i = 0; i < static_cast<int>(invalidZones.size()); i++)
         {
         if (newLine.lineInBlock(invalidZones[i]))
         {
         skipLine = true;
         }
         }
         */

      if (!skipLine)
      {
        this->lines.push_back(newLine);
        this->numLines += 1;
      }
    }
  }
  Mat blank (Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));
  img = blank;

  //drawOriginal(filename, invalidZones);

  splitByColumn();

  segmentWithColumns();

  determineConfidence();

  groupIntoBlocks();
}

void Segment::segmentWithColumns()
{
  //deciding value
  double finalValue = 0.0;

  //ALL PARAMETERS (each ranging from 0 to 1)
  double weightHeight = 0.5;
  double weightCapitalWords = 0.35;
  double weightCapitalLetters = 0.7;
  double weightCharArea = 0.0;
  double heightThresh = 50;
  double gapThresh = 200;

  //values are either 0 or 1
  int inHeightRange = 0;

  //values are double (range is from 0 to 1)
  double ratioCapitalWords = 0.0;
  double ratioCapitalLetters = 0.0;
  double ratioCharArea = 0.0;
  
  double differenceHeight = 0.0;

  //temporary variables
  vector<Textline::Word> lineWords;

  //find the most average height of article lines
  lineHeightComparator comp;
  sort(this->lines.begin(), this->lines.end(), comp);

  double articleHeight = lines[lines.size()/2].getHeight();

  int prevVPOS = 0;
  int curVPOS = 0;

  for (int i = 0; i < sortedColumns.size(); i++)
  {
    prevVPOS = 0;
    curVPOS = 0;

    for (int j = 0; j < sortedColumns[i].size(); j++)
    {
      Textline curLine = sortedColumns[i][j];

      curVPOS = curLine.getVPOS();

      //HEIGHT CHECK
      differenceHeight = abs(curLine.getHeight() - articleHeight);

      if (differenceHeight <= heightThresh)
      {
        inHeightRange = 0;
      }
      else
      {
        inHeightRange = differenceHeight / heightThresh;
      }

      //FIND RATIO OF CAPITAL WORDS
      ratioCapitalWords = curLine.capitalWordsRatio(false);
      ratioCapitalLetters = curLine.capitalWordsRatio(true);
      
      //RATIO OF NUMBER OF CHARACTERS TO AREA
      ratioCharArea = curLine.charAreaRatio();

      //PARAMETERS - inside finalValue formula
      finalValue = (weightHeight * inHeightRange) +
                   (weightCapitalWords * ratioCapitalWords) +
                   (weightCapitalLetters * ratioCapitalLetters) +
                   (weightCharArea * ratioCharArea);

      if (finalValue > 0.6)
      {
        sortedColumns[i][j].setLabel(true);
      }
      else
      {
        sortedColumns[i][j].setLabel(false);
      }

      prevVPOS = curVPOS;
    }
  }
}

//need to further segment this part into functions. But ok for now.
/*
void Segment::segment()
{
  bool prevCat = false;

  int numCategory = 0;

  bool firstIf = false;

  double prevDistance = 0.0;
  double nextDistance = 0.0;

  for (int i = 0; i < numLines; i++)
  {
    if (((xmlHeight(lines[i]) - heightThresh) > diffThresh) ||
        ((xmlHeight(lines[i]) > heightThresh) && 
         (lines[i].capLine(false))) || (lines[i].capLine(true)))
    {
      lines[i].setLabel(true);
      firstIf = true;
    }

    else
    {
      if (lines[i].capLine(false) && (lines[i].charAreaRatio() > CAThresh))
      {
        if (distNextFour(i) > distThresh)
        {
          if (lines[i].continuedTag())
          {
            lines[i].setLabel(false);
          }
          else
          {
            lines[i].setLabel(true);
          }
        }
        else
        {
          lines[i].setLabel(false);
        }
      }
      else
      {
        lines[i].setLabel(false);
      }
    }

    if (i == 0)
    {
      prevCat = lines[i].getLabel();
    }
    else
    {
      if (lines[i].getLabel() == prevCat)
      {
        numCategory++;
      }
      else
      {
        numCategory = 0;
      }

      if ((lines[i].getLabel() != prevCat) 
          && (lines[i].getLabel()) 
          && (numCategory > 4) 
          && (distPrevOne(i) < prevThresh)
          && (!((((xmlHeight(lines[i]) - heightThresh) > diffThresh)
                && (lines[i].capLine(false)) ||
                (lines[i].capLine(true))))))
      {
        lines[i].setLabel(false);
      }

      if ((lines[i].getLabel() != prevCat) 
          && (prevCat == false)
          && (!firstIf))
      {
        prevDistance = distPrevOne(i);
        nextDistance = distNextOne(i);

        if ((prevDistance < nextDistance)
            && (nextDistance > nextThresh)
            && (prevDistance < prevThresh))
        {
          lines[i].setLabel(false);
        }
      }

      if ((lines[i].getLabel() != prevCat) && (prevCat == true) && ((i - 1) >= 0))
      {
        prevDistance = distPrevOne(i);
        nextDistance = distNextOne(i);

        if ((prevDistance < nextDistance)
            && (nextDistance > nextThresh)
            && (prevDistance < prevThresh)
            && (abs(lines[i-1].getHPOS() - lines[i].getHPOS()) < 500)
            && (centeredLine(i, 20, 20) && lines[i].hyphen()))
        {
          lines[i].setLabel(true);
        }
      }
    } //end else
  } //end 1st for 
}
*/

void Segment::groupIntoBlocks()
{
  /* Use the sortedColumns vector to group regions into title/article regions */

  double leftMostX = 0.0;
  double rightMostX = 0.0;
  double startY = 0.0;

  double curX = 0.0;
  double prevX = 0.0;

  double curRightX = 0.0;
  double prevRightX = 0.0;

  double curTopY = 0.0;
  double prevTopY = 0.0;

  double curBottomY = 0.0;
  double prevBottomY = 0.0;

  bool curLabel = false;
  bool prevLabel = false;

  bool startMultiBlock = false;
  int count = 1; 

  string blockContent = "";

  vector<Textline::Word> wordsInLine;

  double minGapLeft = 1300.0;
  double minGapRight = 1300.0;

  double minGapTop = 500.0;

  double gapTopHoriz = 100.0;

  int totalConfidence = 0;
  int numLines = 0;

  for (int i = 0; i < sortedColumns.size(); i++)
  {
    for (int j = 0; j < sortedColumns[i].size(); j++)
    {
      Textline curLine = sortedColumns[i][j];
      
      if (curLine.isVisited())
      {
        continue;
      }
      else
      {
        curLine.setVisited();
      }

      curX = curLine.getHPOS();
      curTopY = curLine.getVPOS();
      curBottomY = curLine.getVPOS() + curLine.getHeight();
      curRightX = curLine.getHPOS() + curLine.getWidth();
      /*
         curX = (curLine.getHPOS());
         curTopY = (curLine.getVPOS());
         curBottomY = (curLine.getVPOS() + curLine.getHeight());
         curRightX = (curLine.getHPOS() + curLine.getWidth());
      */

      curLabel = curLine.getLabel();

      if (j == 0)
      {
        if (curLine.isMulti())
        {
          startMultiBlock = true;
        }

        startY = curTopY;
        leftMostX = curX;
        rightMostX = curRightX;

        wordsInLine = curLine.getWords();

        for (size_t i = 0; i < wordsInLine.size(); i++)
        {
          blockContent += wordsInLine[i].content + " ";
        }

        numLines += 1;
        if (curLine.getConfidence() >= 0.9)
        {
          totalConfidence += 1;
        }
      }

      else
      {
        if (
            (curLabel != prevLabel) || 
            (startMultiBlock && (!curLine.isMulti())) ||
            ((abs(curTopY - prevBottomY) > gapTopHoriz) &&
            (abs(curX - prevX) > minGapLeft) && 
            (abs(curRightX - prevRightX) > minGapRight)) ||
            (abs(curTopY - prevBottomY) > minGapTop) ||
            (j == (sortedColumns[i].size() - 1))
            )
        { 
          startMultiBlock = false;

          //removing the extra space at the end of the line
          if (blockContent.size() > 0)
          {
            blockContent.pop_back();
          }

          /* End block, if not the very first block */
          Block tempBlock(
              convertToImage_X(leftMostX), //x
              convertToImage_Y(startY), //y
              convertToImage_Y((prevBottomY - startY)), //height
              convertToImage_X((rightMostX - leftMostX)) //width
              );

          tempBlock.setLabel(prevLabel);

          //setting confidence as average of all confidence in lines
          if ((double(totalConfidence) / numLines) > 0.5)
          {
            tempBlock.setConfidence(1);
          }
          else
          {
            tempBlock.setConfidence(0);
          }

          totalConfidence = 0;
          numLines = 0;

          tempBlock.setID(count);

          tempBlock.setContent(blockContent);

          regions.push_back(tempBlock);

          /* Start new block */
          if (curLine.isMulti())
          {
            startMultiBlock = true;
          }

          wordsInLine.clear();
          blockContent = "";

          wordsInLine = curLine.getWords();

          for (size_t i = 0; i < wordsInLine.size(); i++)
          {
            blockContent += wordsInLine[i].content + " ";
          }

          numLines += 1;
          if (curLine.getConfidence() >= 0.9)
          {
            totalConfidence += 1;
          }

          count++;
          startY = curTopY;
          leftMostX = curX;
          rightMostX = curRightX;
        }

        else
        {
          //update all block parameters
          if (curX < leftMostX)
            leftMostX = curX;
          if (curRightX > rightMostX)
            rightMostX = curRightX;

          wordsInLine.clear();

          wordsInLine = curLine.getWords();

          //removes the extra space in the string
          if (blockContent.size() > 0)
          {
            blockContent.pop_back();
          }

          //adds newline character to divide words into separate lines
          blockContent += "\n";

          for (size_t i = 0; i < wordsInLine.size(); i++)
          {
            blockContent += wordsInLine[i].content + " ";
          }

          numLines += 1;
          if (curLine.getConfidence() >= 0.9)
          {
            totalConfidence += 1;
          }

        }
      }

      prevX = curX;
      prevRightX = curRightX;
      prevTopY = curTopY;
      prevBottomY = curBottomY;
      prevLabel = curLabel;
    }
  }

}

int Segment::wordsInWindow(int column, vector<int>& rows)
{
  int countWords = 0;

  for (size_t i = 0; i < rows.size(); i++)
  {
    countWords += static_cast<int>(sortedColumns[column][rows[i]].words.size());
  }

  return countWords;
}

int Segment::heightWindow(int column, vector<int>& rows)
{
  int topVPOS = sortedColumns[column][rows[0]].getVPOS();
  int bottomVPOS = sortedColumns[column][rows[rows.size()-1]].getVPOS() +
                    sortedColumns[column][rows[rows.size()-1]].getHeight();

  return (bottomVPOS - topVPOS);
}

int Segment::titleArticleTitle(int column, vector<int>& rows)
{
  bool firstTitle = false;
  bool setFirst = false;
  bool secondTitle = false;

  for (size_t i = 0; i < rows.size(); i++)
  {
    if (!firstTitle && sortedColumns[column][rows[i]].getLabel())
    {
      if (((i+1) < rows.size()) &&
          (sortedColumns[column][rows[i+1]].getLabel() == false))
      {
        firstTitle = true;
        setFirst = true;
      }
    }

    if (!setFirst && firstTitle)
    {
      if (sortedColumns[column][rows[i]].getLabel())
      {
        secondTitle = true;
        
        //setConfDone because twoTitle is a very good indicator of an image
        for (size_t j = 0; j < rows.size(); j++)
        {
          sortedColumns[column][rows[j]].setConfDone();
        }

        return 1;
      }
    }

    setFirst = false;
  }

  return 0;
}

int Segment::maxGapWords(int column, vector<int>& rows)
{
  int maxGap = 0;
  int pHpos = 0;
  int aHpos = 0;

  for (size_t i = 0; i < rows.size(); i++)
  {
    if (sortedColumns[column][rows[i]].getLabel() == false)
    {
      for (size_t j = 0; j < sortedColumns[column][rows[i]].words.size() - 1; j++)
      {
        pHpos = sortedColumns[column][rows[i]].words[j].hPos +
              sortedColumns[column][rows[i]].words[j].width;
        aHpos = sortedColumns[column][rows[i]].words[j+1].hPos;

        if ((abs(aHpos - pHpos)) > maxGap)
        {
          maxGap = abs(aHpos - pHpos);
        }
      }
    }
  }

  return maxGap;
}


/*
int Segment::numConsecArticleLines(vector<Textline>& window)
{
  int numConsec = 0;
  bool isArticle = false;

  for (size_t i = 0; i < window.size(); i++)
  {
    if (i == 0) 
    {
      if (window[i].getLabel() == false)
      {
        isArticle = true;
        numConsec += 1;
      }
      
      else
        isArticle = false;
    }
    
    else
    {
      if (isArticle)
      {
        if (window[i].getLabel() == false)
          numConsec += 1;
        else
          isArticle = false;
      }
      else
      {
        if (window[i].getLabel() == false)
        {
          isArticle = true;
          numConsec += 1;
        }
      }
    }
  }
  
  return numConsec;
}
*/

void Segment::setConfNonVisited(double level, int column, vector<int>& rows)
{
  /* Sets all lines that have not been assigned confidence levels
   * with confidence levels.
   */

  for (size_t i = 0; i < rows.size(); i++)
  {
    if(sortedColumns[column][rows[i]].isConfDone() == false)
    {
      sortedColumns[column][rows[i]].setConfidence(level);
    }
    
    else
    {
      if (sortedColumns[column][rows[i]].getNumConf() == 0)
      {
        sortedColumns[column][rows[i]].setConfidence(level);
        sortedColumns[column][rows[i]].setNumConf(1); 
      }
    }
  }
}
    
void Segment::determineConfidence()
{
  vector<int> rows;
  /* THRESHOLDS */

  //article length must be >= 4 if at least one article line exists
  int numArticleThresh = 4; 
  int numWordsThresh = 8;
  int heightThresh = 2800;
  int gapThresh = 300;

  /* WEIGHT */
  double weightConfArticle = 0.9;
  double weightConfWords = 0.002;
  double weightTwoTitle = 0.2;
  double weightGap = 0.5;

  //difference from threshold - every multiple of 50 equates to
  //losing 0.5% in confidence
  double weightConfHeight = 0.3;

  /* VARIABLES USED IN LOOP */
  int curArticle = 0;
  int curWords = 0;
  int curHeight = 0;
  int curGap = 0;
  int diffArticle = 0;
  int diffWords = 0;
  int diffHeight = 0;
  int diffGap = 0;

  int twoTitle = 0;
  double finalConfDecrease = 0.0;

  //set all conf done to false
  for (int i = 0; i < sortedColumns.size(); i++)
  {
    for (int j = 0; j < sortedColumns[i].size(); j++)
    {
      sortedColumns[i][j].setConfFalse();
    }
  }

  //Now determine confidence levels
  
  for (int i = 0; i < sortedColumns.size(); i++)
  {
    //create a window of 5 consecutive lines
    for (int j = 0; j < sortedColumns[i].size(); j += 1) 
    {
      if (sortedColumns[i][j].isLine("ducers", -1))
      {
        cout << "GOT HERE CHARLIE" << '\n';
      }

      rows.push_back(j);
      if ((j+1) < sortedColumns[i].size())
      {
        rows.push_back(j+1);
      }
      if ((j+2) < sortedColumns[i].size())
      {
        rows.push_back(j+2);
      }
      
      if ((j+3) < sortedColumns[i].size())
      {
        rows.push_back(j+3);
      }
      /*
      if ((j+4) < sortedColumns[i].size())
      {
        rows.push_back(j+4);
      }
      */

      //curArticle = numConsecArticleLines(i, rows);
      curWords = wordsInWindow(i, rows);
      curHeight = heightWindow(i, rows);
      twoTitle = titleArticleTitle(i, rows);
      curGap = maxGapWords(i, rows);

      /*
      if ((curArticle > 0) && (curArticle < 4))
      {
        diffArticle = numArticleThresh - curArticle;
      }
      else
      {
        //do not consider article length if no article lines exist
        //or the number of article lines exceed 4
        diffArticle = 0;
      }
      */

      if (curWords > numWordsThresh)
      {
        diffWords = 0;
      }
      else
      {
        diffWords = abs(curWords - numWordsThresh);
      }

      if (curHeight < heightThresh)
      {
        diffHeight = 0;
      }
      else
      {
        diffHeight = abs(curHeight - heightThresh);
      }

      if (curGap < gapThresh)
      {
        diffGap = 0;
      }
      else
      {
        diffGap = abs(curGap - gapThresh);
      }

      diffHeight /= 100;

      finalConfDecrease = (weightTwoTitle * twoTitle) + 
                          (weightConfWords * diffWords) + 
                          (weightConfHeight * diffHeight) + 
                          (weightGap * diffGap);
  
      setConfNonVisited((1.0 - finalConfDecrease), i, rows);
      rows.clear();
    }
  }
}

double Segment::xmlHeight(Textline& line)
{
  return (Ydimension * (double(line.getHeight()) / pageHeight));
}

double Segment::xmlWidth(Textline& line)
{
  return (Xdimension * (double(line.getWidth()) / pageWidth));
}

double Segment::xmlVPOS(Textline& line)
{
  return (Ydimension * (double(line.getVPOS()) / pageHeight));
}

double Segment::xmlHPOS(Textline& line)
{
  return (Xdimension * (double(line.getHPOS()) / pageWidth));
}

double Segment::distNextFour(int idx)
{
  double prevLoc = xmlVPOS(lines[idx]) + xmlHeight(lines[idx]);

  if ((idx + 4) > numLines)
  {
    return 0.0;
  }

  else if (xmlVPOS(lines[idx + 1]) < prevLoc)
  {
    return 0.0;
  }

  else
  {
    return (double(xmlVPOS(lines[idx + 3])) - prevLoc);
  }
}

bool Segment::centeredLine(int idx, int leftThresh, int rightThresh)
{
  double curHPOS = lines[idx].getHPOS();
  double newHPOS = curHPOS;

  double curRight = lines[idx].getHPOS() + lines[idx].getWidth();
  double newRight = curRight;

  if ((idx + 1) < (numLines))
  {
    if (xmlVPOS(lines[idx + 1]) > xmlVPOS(lines[idx]))
    {
      newHPOS = lines[idx + 1].getHPOS();
      newRight = lines[idx + 1].getHPOS() + lines[idx + 1].getWidth();
    }
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

//distPrevOne and distNextOne use the original coordinates from XML file.
//This may need to be changed.
double Segment::distPrevOne(int idx)
{
  double prevLoc = 0.0;
  double curLoc = 0.0;

  if ((idx - 1) >= 0)
  {
    prevLoc = lines[idx - 1].getVPOS() +lines[idx - 1].getHeight();
    curLoc = lines[idx].getVPOS();
  }

  if (curLoc > prevLoc)
  {
    return (curLoc - prevLoc);
  }
  else
  {
    return 0.0;
  }
}

double Segment::distNextOne(int idx)
{
  double prevLoc = 0.0;
  double curLoc = 0.0;

  if (idx + 1 <= (numLines - 1))
  {
    prevLoc = lines[idx].getVPOS() + lines[idx].getHeight();
    curLoc = lines[idx + 1].getVPOS();
  }

  if (curLoc > prevLoc)
  {
    return (curLoc - prevLoc);
  }
  else
  {
    return 0.0;
  }
}

void Segment::printLines()
{
  for (int i = 0; i < numLines; i++)
  {
    lines[i].printLine();
  }
}


void Segment::PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, 
    const cv::Scalar& color, int fontFace, double fontScale, int thickness = 1, 
    int lineType = 8)
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


void Segment::drawOriginal(char* filename, std::vector<Block>& zones)
{
  drawWords(true);
  drawLines(true);

  for (int i = 0; i < static_cast<int>(zones.size()); i++)
  {
    Block tempBlock = zones[i];

    Point rP1((int)(Xdimension * (tempBlock.getX()/(double)pageWidth)), (int)(Ydimension * (tempBlock.getY()/(double)pageHeight)));

    Point rP2(rP1.x + (int)(Xdimension * (tempBlock.getWidth()/(double)pageWidth))
        ,rP1.y + (int)(Ydimension * (tempBlock.getHeight()/(double)pageHeight)));

    rectangle(img, rP1, rP2, Scalar(0,255,0), 17);
  }

  vector<int> compression_params;
  compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
  compression_params.push_back(50);

  string fileName(filename);

  fileName = fileName.substr(fileName.length() - 8, 4);

  imwrite("imageOrig_" + fileName + ".jpg", img, compression_params);

  Mat origImage(Xdimension, Ydimension, CV_8UC3, Scalar(255,255,255));

  img = origImage;
}

void Segment::drawBlocks()
{
  //ints because XML stores all parameters as integers
  int topX = 0;
  int topY = 0;
  int blockHeight = 0;
  int blockWidth = 0;

  int ntopX = 0;
  int ntopY = 0;
  int nblockHeight = 0;
  int nblockWidth = 0;

  //cout << regions.size() << '\n';

  double testValue = 100.0;

  //cout << convertToXML_h(convertToImage_X(testValue)) << '\n';

  for (auto curBlock: regions)
  {
    topX = convertToXML_h(curBlock.getX());
    topY = convertToXML_v(curBlock.getY());
    blockWidth = convertToXML_h(curBlock.getWidth());
    blockHeight = convertToXML_v(curBlock.getHeight());

    Point rP1((int)(Xdimension * (topX/(double)pageWidth)), 
        (int)(Ydimension * (topY/(double)pageHeight)));

    Point rP2(rP1.x + (int)(Xdimension * (blockWidth/(double)pageWidth))
        , rP1.y + (int)(Ydimension * (blockHeight/(double)pageHeight)));

    if (curBlock.getConfidence() == 0)
    {
      if (curBlock.getLabel())
        rectangle(img, rP1, rP2, Scalar(0,0,255), 7);
      else
        rectangle(img, rP1, rP2, Scalar(255,0,0), 7);
    }    
  }
}


void Segment::drawLines(bool orig)
{
  int rHpos;
  int rVpos;
  int rHeight;
  int rWidth;

  for (int i = 0; i < sortedColumns.size(); i++)
  {
  //int i = 2;
    for (int j = 0; j < sortedColumns[i].size(); j++)
    {
      rHpos = sortedColumns[i][j].getHPOS();
      rVpos = sortedColumns[i][j].getVPOS();
      rHeight = sortedColumns[i][j].getHeight();
      rWidth = sortedColumns[i][j].getWidth();

      Point rP1((int)(Xdimension * (rHpos/(double)pageWidth)), (int)(Ydimension * (rVpos/(double)pageHeight)));

      Point rP2(rP1.x + (int)(Xdimension * (rWidth/(double)pageWidth))
      , rP1.y + (int)(Ydimension * (rHeight/(double)pageHeight)));

      //rectangle(img, rP1, rP2, Scalar(blueCount,greenCount,redCount), 7);

      cout << sortedColumns[i][j].getConfidence() << '\n';

      if ((sortedColumns[i][j].getConfidence()) < 0.9)
      {
        if (sortedColumns[i][j].getLabel())
        {
          rectangle(img, rP1, rP2, Scalar(0,0,255), 7);
        }

        else
        {
          rectangle(img, rP1, rP2, Scalar(255,0,0), 7);
        }
      }
    }
  }
}

void Segment::drawWords(bool orig)
{
  int printHPOS = 0;
  int printVPOS = 0;
  int printHeight = 0;
  int printWidth = 0;
  int numOfLines = 0;

  /* orig - refers to the original set of lines BEFORE
   * removing lines that fell into invalid zones 
   */

  if (orig)
  {
    numOfLines = static_cast<int>(origLines.size());
  }
  else
  {
    numOfLines = static_cast<int>(lines.size());
  }

  for (int i = 0; i < numOfLines; i++)
  {
    vector<Textline::Word> curWords;
    if (orig)
    {
      curWords = origLines[i].getWords();
    }
    else
    {
      curWords = lines[i].getWords();
    }

    for (int i = 0; i < static_cast<int>(curWords.size()); i++)
    {   
      printHPOS = curWords[i].hPos;
      printVPOS = curWords[i].vPos;
      printHeight = curWords[i].height;
      printWidth = curWords[i].width;

      Point p1((int)(Xdimension * (printHPOS/(double)pageWidth)), (int)(Ydimension * (printVPOS/(double)pageHeight)));

      Point p2(p1.x + (int)(Xdimension * (printWidth/(double)pageWidth))
          , p1.y + (int)(Ydimension * (printHeight/(double)pageHeight)));

      cv::Rect roi(p1.x, p1.y, (p2.x-p1.x), (p2.y-p1.y));

      PutText(img, curWords[i].content, roi, Scalar(0,0,0), FONT_HERSHEY_SIMPLEX,2,8);
    }
  }
}

void Segment::writeImage(char* filename)
{
  vector<int> compression_params;
  compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
  compression_params.push_back(50);

  string fileName(filename);

  fileName = fileName.substr(fileName.length() - 8, 4);

  cout << "Written " << "segImage_" + fileName + ".jpg" << '\n';

  imwrite("segImage_" + fileName + ".jpg", img, compression_params);
}

void Segment::writeJSON(char* filename, char* outDir)
{
  string fileName(filename);

  fileName = fileName.substr(fileName.length() - 8, 4);

  rapidjson::Document d;
  d.SetObject();

  rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

  rapidjson::Value annotations(rapidjson::kArrayType);

  string label = "";
  string type = "rect";
  string className = "xml";
  string imageName = fileName + ".jpg";
  string content = "";

  //test using each line as its own block
  
  /*
  int count = 0;

  for (int i = 0; i < sortedColumns.size(); i++)
  {
    for (auto curLine: sortedColumns[i])
    {
      rapidjson::Value obj(rapidjson::kObjectType);
      rapidjson::Value val(rapidjson::kObjectType);

      if (curLine.getLabel())
      {
        label = "title";
      }
      else
      {
        label = "article";
      }

      val.SetString(label.c_str(), 
          static_cast<rapidjson::SizeType>(label.length()), allocator);
      obj.AddMember("class", val, allocator);
      obj.AddMember("confidence", 0.9, allocator);
      obj.AddMember("height", convertToImage_Y(curLine.getHeight()), allocator);
      obj.AddMember("id", count, allocator);

      //temp id
      count++;

      val.SetString(type.c_str(), 
          static_cast<rapidjson::SizeType>(type.length()), allocator);

      obj.AddMember("type", val, allocator);
      obj.AddMember("width", convertToImage_X(curLine.getWidth()), allocator);
      obj.AddMember("x", convertToImage_X(curLine.getHPOS()), allocator);
      obj.AddMember("y", convertToImage_Y(curLine.getVPOS()), allocator);

       
      content = regions[i].getContent();
      
      val.SetString(content.c_str(),
          static_cast<rapidjson::SizeType>(content.length()), allocator);

      obj.AddMember("content", val, allocator);
      

      annotations.PushBack(obj, allocator);
    }
  }
  */

  for (int i = 0; i < regions.size(); i++)
  {
    rapidjson::Value obj(rapidjson::kObjectType);
    rapidjson::Value val(rapidjson::kObjectType);

    if (regions[i].getLabel())
    {
      label = "title";
    }
    else
    {
      label = "article";
    }

    val.SetString(label.c_str(), 
        static_cast<rapidjson::SizeType>(label.length()), allocator);
    obj.AddMember("class", val, allocator);
    obj.AddMember("confidence", regions[i].getConfidence(), allocator);
    obj.AddMember("height", regions[i].getHeight(), allocator);
    obj.AddMember("id", regions[i].getID(), allocator);

    val.SetString(type.c_str(), 
        static_cast<rapidjson::SizeType>(type.length()), allocator);

    obj.AddMember("type", val, allocator);
    obj.AddMember("width", regions[i].getWidth(), allocator);
    obj.AddMember("x", regions[i].getX(), allocator);
    obj.AddMember("y", regions[i].getY(), allocator);

    content = regions[i].getContent();
    
    val.SetString(content.c_str(),
        static_cast<rapidjson::SizeType>(content.length()), allocator);

    obj.AddMember("content", val, allocator);

    annotations.PushBack(obj, allocator);
  }

  rapidjson::Value val(rapidjson::kObjectType);

  d.AddMember("annotations", annotations, allocator);

  val.SetString(className.c_str(), 
      static_cast<rapidjson::SizeType>(className.length()), allocator);

  d.AddMember("class", val, allocator);

  val.SetString(imageName.c_str(), 
      static_cast<rapidjson::SizeType>(imageName.length()), allocator);

  d.AddMember("filename", val, allocator);

  const rapidjson::Value& annotate = d["annotations"];
  //cout << "Number of Annotations = " << annotate.Size() << '\n';

  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);

  string outDirName(outDir);

  std::ofstream outFile;

  outFile.open(outDirName + fileName + "_xml.json");

  outFile << buffer.GetString() << std::endl;

  cout << "Written " << fileName + "_xml.json" << '\n';
}
