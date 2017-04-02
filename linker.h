#include <unordered_map>

/*struct block {
  double hpos;
   double vpos;
   double width;
   double heigth;
   string type;
};*/



vector <vector<Block>> splitByColumn(vector<Block>& master) {
   unordered_map <double,int> column_dict;
   vector <vector<Block> columns; 
   num_columns = 0;
   for (auto block : master) {
      if (column_dict.find(block.hpos) != column_dict.end()) {
         columns[column_dict[block.hpos]].push_back(block); 
      } else {
         vector<Block> column;
         column.push_back(block);
         columns.push_back(column);
         column_dict[block.hpos] = num_column;
         num_columns++;
      }
   }
}

void print_columns(vector<vector<Block>> columns) {
   int num = 0;
   for (auto column : columns) {
      std::cout << "column " << num << std::endl;
      for (auto block : column) {
         std::cout << block.hpos << std::endl;
      }
      num++;
      std::cout << "////////////////////////////////////////////////////" << std::endl;
   }
}
