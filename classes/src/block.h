#ifndef BLOCK_H
#define BLOCK_H

class Block
{
   public:
      /* CONSTRUCTORS - DEFAULT AND INITIALIZED */
      Block(): x(0.0), y(0.0), height(0.0), width(0.0), ID(0) {};
      Block(double in_x, double in_y, double in_height, double in_width,
            int in_ID): x(in_x), y(in_y), height(in_height), width(in_width),
                        ID(in_ID) {};

      /* Retrieving private variabes for use by functions */
      double getX();
      double getY();
      double getHeight();
      double getWidth();
      int getID();

   private:
      double x;
      double y;
      double height;
      double width;

      int ID;
};

#endif
