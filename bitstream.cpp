#include "bitstream.h"

BitStreamFile::BitStreamFile(std::string filename)
  :ifs(filename.c_str())
{
  offset = 0;
}

int BitStreamFile::getbit()
{
}

void BitStreamFile::ungetbit()
{
}
