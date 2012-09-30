#ifndef BITSTREAM_H_INCLUDED
#define BITSTREAM_H_INCLUDED

#include <string>
#include <fstream>

class BitStream
{
  private:
  public:
    virtual int getbit() = 0;
    virtual void ungetbit() = 0;
};

class BitStreamFile : public BitStream
{
  private:
    std::ifstream ifs;
    int offset;
  public:
    BitStreamFile(std::string filename);
    int getbit();
    void ungetbit();
};

#endif   /* BITSTREAM_H_INCLUDED */
