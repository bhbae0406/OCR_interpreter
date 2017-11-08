#ifndef SEGMENT_H
#define SEGMENT_H

#include "textLine.h"
#include "block.h"
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class Segment
{
  public:

    double convertToXML_h(double in);
    double convertToXML_v(double in);

    double convertToImage_X(double inX);
    double convertToImage_Y(double inY);

    void readParam(const string& param_json);

    vector<Block> generate_invalid_zones(const string& json_file);
    void splitByColumn();
    void setDim(char* dimXcoord, char* dimYcoord);
    //CONSTRUCTOR
    /* This will use rapidxml to read through the document and:
     * 1) Construct all Textline objects and place in vector "lines"
     * 2) Initialize the page attributes, pageWidth and pageHeight
     */
    Segment(char* filename, char* dimX, char* dimY, char* param_json);

    /* The following four functions are used to normalize the height, width,
     * VPOS, and HPOS. This is necessary because different newspapers have
     * different pageWidths and pageHeights. Therefore, for the thresholds
     * to work correctly, these attributes need to be normalized.
     */

    void segmentWithColumns();

    void segment();

    void groupIntoBlocks();

    /* FUNCTIONS FOR DETERMINING CONFIDENCE */

    int wordsInWindow(int column, vector<int>& rows);
    int heightWindow(int column, vector<int>& rows);
    int titleArticleTitle(int column, vector<int>& rows);
    int maxGapWords(int column, vector<int>& rows);
    //int numConsecArticleLines(int column, vector<int>& rows);
    void setConfNonVisited(double level, int column, vector<int>& rows);

    void determineConfidence();

    /* FUNCTIONS FOR DETERMINING CONFIDENCE */

    double xmlHeight(Textline& line);
    double xmlWidth(Textline& line);
    double xmlVPOS(Textline& line);
    double xmlHPOS(Textline& line);

    //distance from current line to the third line below it. May need to fix later.
    double distNextFour(int idx);
    bool centeredLine(int idx, int leftThresh, int rightThresh);
    double distPrevOne(int idx);
    double distNextOne(int idx);

    /* DEBUGGING TOOL
     * Will print out the content of the lines in the order of
     * the vector "lines". 
     */

    void printLines();

    
    void PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, 
        const cv::Scalar& color, int fontFace, double fontScale, 
        int thickness, int lineType);

    void drawOriginal(char* filename, std::vector<Block>& zones);

    void drawBlocks();

    void drawLines(bool orig);
    void drawWords(bool orig);
    
    void writeImage(char* filename);
    
      
    void writeJSON(char* filename, char* outDir);

  private:
    vector<Textline> lines;
    vector<Textline> origLines;
    vector<vector<Textline>> columns;
    vector<vector<Textline>> sortedColumns;
    vector<Textline> nonSingleLines;
    vector<Textline> smallWidthLines;

    //TITLE AND ARTICLE REGIONS
    vector<Block> regions; 

    //IMAGE
    //cv::Mat img;

    //IMAGE ATTRIBUTE CONSTANTS
    int Xdimension;
    int Ydimension;

    //PAGE ATTRIBUTE CONSTANTS
    //UNKNOWN - must get from XML layout
    int pageWidth;
    int pageHeight;
    int numLines;
    int dimX;
    int dimY;

    //THRESHOLD CONTANTS
    double heightThresh;
    int diffThresh;
    double CAThresh;
    double distThresh;
    int prevThresh;
    int nextThresh;
    
    //ALL PARAMETERS - taken from parameter.json
    double slackupperW;
    double slacklowerW;
    double rangeSlackLowerW;
    double rangeSlackHigherW;
    double minThresholdW;
    double maxThresholdW;
    double weightHeight;
    double weightCapitalWords;
    double weightCapitalLetters;
    double weightCharArea;

};

// Comparators
struct columnLengthComparator 
{
  bool operator()(const Textline& a , const Textline& b)
  {
    return a.getWidth() < b.getWidth();
  }
};

struct lineHeightComparator
{
  bool operator()(const Textline& a, const Textline& b)
  {
    return a.getHeight() < b.getHeight();
  }

};

struct columnSortComparator
{
  bool operator()(vector<Textline>& a, vector<Textline>& b)
  {
    return a[0].getHPOS() < b[0].getHPOS();
  }
};

struct lineSortComparator 
{
  bool operator()(Textline& a , Textline& b)
  {
    return a.getVPOS() < b.getVPOS();
  }
};

#endif
