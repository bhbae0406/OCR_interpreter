#include "block.h"

double Block::getX()
{
  return this->x;
}

double Block::getY()
{
  return this->y;
}

double Block::getHeight()
{
  return this->height;
}

double Block::getWidth()
{
  return this->width;
}

bool Block::getLabel()
{
  return this->isTitle;
}

double Block::getConfidence()
{
  return this->confidence;
}

int Block::getID()
{
  return this->ID;
}

void Block::setLabel(bool curLabel)
{
  this->isTitle = curLabel;
}

void Block::setConfidence(double level)
{
  this->confidence = level;
}

void Block::setID(int id)
{
  this->ID = id;
}

void Block::setContent(std::string blockContent)
{
  this->content = blockContent;
}

std::string& Block::getContent()
{
  return this->content;
}
