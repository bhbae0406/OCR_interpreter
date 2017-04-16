#include "link.h"

using namespace std;

Link::Link()
{
   numColumns = static_cast<int>(columns.size());
}

void Link::topDownLink()
{
   int curID = -1;
   int curSubID = 0;
   bool prevArticle = false;
   bool first = true;

   //going down each column
   for (int i = 0; i < numColumns; i++)
   {
      //going through each line in each column
      for (int j = 0; j < static_cast<int>(columns[i].size()); j++)
      {
         if (!(columns[i][j].visited()))
         {
            if ((first || prevArticle) && (columns[i][j].getLabel()))
            {
               ++curID;

               if (first)
                  first = false;
            }

            columns[i][j].setID(curID);
            columns[i][j].setSubID(curSubID);
            columns[i][j].setVisited();

            if (columns[i][j].getLabel() && columns[i][j].isMulti())
            {
               multiColumn(i, j, /*numCol*/, curID, curSubID);
            }

            if (!(columns[i][j].getLabel()))
            {
               prevArticle = true;
            }
         }
      }
   }
}

void Link::multiColumn(int col, int idx, int numCol, int curID, int curSubID)
{
   int prevCol = 0;
   int prevIdx = 0;
   bool first = true;

   while (numCol > 0)
   {
      ++idx;
      while (first && columns[col][idx].getLabel())
      {
         if (columns[col][idx].isMulti())
         {
            columns[col][idx].setID(curID);
            columns[col][idx].setSubID(curSubID);
            
            prevCol = col;
            prevIdx = idx;

            columns[col][idx].setVisited();
         }
         else //don't deal with this in multicolumn
         {
            return;
         }

         ++idx;
      }

      first = false;

      while (!(columns[col][idx].getLabel()))
      {
         columns[col][idx].setID(curID);
         columns[col][idx].setSubID(curSubID);
         columns[col][idx].setVisited();
      }

      ++col;
      --numCol;

      ++curSubID;

      idx = findLine(prevCol, prevIdx, col);
   }
}

int Link::findLine(int prevCol, int prevIdx, int col)
{
   for (int i = 0; i < static_cast<int>(columns[col].size()); i++)
   {
      if (columns[col][i] == columns[prevCol][prevIdx])
      {
         return i;
      }
   }
}
