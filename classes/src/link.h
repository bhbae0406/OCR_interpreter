#ifndef LINK_H
#define LINK_H

#include "textline.h"
#include "segment.h"
#include <vector>

using namespace std;

//Linking will be done after segmentation
class Link
{
   public:

      Link();
      /* REGULAR LINKING
       *
       * Going from top to bottom of each column. This also accounts
       * for multi-column articles. It is critical that copies of
       * multi-column articles be included in "columns" vector.
       */
      void topDownLink();

      //arguments are the first instance of the multi-column title
      void multiColumn(int col, int idx);

      int findLine(int prevCol, int prevIdx, int col);

   private:

      //assumes we include copies of mulit-column titles
      //and the lines are sorted based on VPOS in columns
      //
      //The copies of multi-column titles will really help
      //once we want to add exceptions. For instance, when a
      //multi-column title directly follows another multi-
      //column title.
      vector <vector<Textline> > columns;
      
      int numColumns;

};

#endif
