#ifndef BLOCK_H
#define BLOCK_H

#include <string>

class Block
{
   public:
      /* CONSTRUCTORS - DEFAULT AND INITIALIZED */
      Block(): x(0.0), y(0.0), height(0.0), width(0.0), ID(0) {};
      Block(double in_x, double in_y, double in_height, 
          double in_width): x(in_x), y(in_y), height(in_height), width(in_width) {};

      /* Retrieving private variabes for use by functions */
      double getX();
      double getY();
      double getHeight();
      double getWidth();
      bool getLabel();
      double getConfidence();
      int getID();

      /* Set labels */
      void setLabel(bool curLabel);
      void setConfidence(double level);
      void setID(int id);

      void setContent(std::string blockContent);

      std::string& getContent();

   private:
      double x;
      double y;
      double height;
      double width; 
      std::string content;

      double confidence;

      int ID;
      bool isTitle;
};

#endif
