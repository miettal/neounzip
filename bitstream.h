#ifndef BITSTREAM_H_INCLUDED
#define BITSTREAM_H_INCLUDED

#include <string>
#include <fstream>

class BitStream : public std::ifstream
{
  private:
    int offset;
    char byte;
  public:
    BitStream(std::string filename);
    int getbit();
};

#endif   /* BITSTREAM_H_INCLUDED */
