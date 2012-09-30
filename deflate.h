#ifndef DEFLATE_H_INCLUDED
#define DEFLATE_H_INCLUDED

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#include "zip_info.h"
#include "debug.h"

#define DEFLATE_STREAM_IN_FINAL             0
#define DEFLATE_STREAM_IN_TYPE              1
#define DEFLATE_STREAM_IN_DATA              2
#define DEFLATE_STREAM_IN_LENGTH_EXTRA_BITS 3
#define DEFLATE_STREAM_IN_DIST              4
#define DEFLATE_STREAM_IN_DIST_EXTRA_BITS   5

#define DEFLATE_FINAL_LAST_BLOCK     0
#define DEFLATE_FINAL_NOT_LAST_BLOCK 1

#define DEFLATE_TYPE_NO_COMPRESSION  0
#define DEFLATE_TYPE_FIXED_HUFFMAN   1
#define DEFLATE_TYPE_DYNAMIC_HUFFMAN 2
#define DEFLATE_TYPE_RESERVED        3

int decodeDeflate(FILE *src, uint32_t src_len, FILE *dst);

#endif   /* DEFLATE_H_INCLUDED */
