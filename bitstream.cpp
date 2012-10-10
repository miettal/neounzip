#include "bitstream.h"

BitStream::BitStream(std::string filename)
  :std::ifstream(filename.c_str())
{
  offset = 0;
}

int BitStream::getbit()
{
  int bit;

  if(this->offset >= 8){
    this->offset = 0;
  }

  if(this->offset == 0){
    this->read(&this->byte, 1);
  }

  bit = (this->byte>>this->offset)&1;
  this->offset++;

  return bit;
}
