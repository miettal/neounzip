#ifndef ZIP_INFO_H_INCLUDED
#define ZIP_INFO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "deflate.h"

#define ZIP_COMPRESSION_TYPE_NO_COMPRESSION 0
#define ZIP_COMPRESSION_TYPE_DEFLATE        8

#define BUFFER_MAX_SIZE 32*1024

typedef struct ZipHeader{
  uint8_t signature[4];
  uint16_t version;
  uint16_t flags;
  uint16_t compression;
  uint16_t dos_time;
  uint16_t dos_date;
  uint32_t crc32;
  uint32_t compressed_size;
  uint32_t uncompressed_size;
  uint16_t file_name_length;
  uint16_t extra_field_length;
  char *file_name;
  long extra_field_pos;
  long data_pos;
} ZipHeader;

typedef struct ZipCentralHeader{
  uint8_t signature[4];
  uint16_t version_made;
  uint16_t version;
  uint16_t flags;
  uint16_t compression;
  uint16_t dos_time;
  uint16_t dos_date;
  uint32_t crc32;
  uint32_t compressed_size;
  uint32_t uncompressed_size;
  uint16_t file_name_length;
  uint16_t extra_field_length;
  uint16_t file_comment_length;
  uint16_t disk_number_start;
  uint16_t internal_file_attributes;
  uint32_t external_file_attributes;
  uint32_t position;
  char *file_name;
  long extra_field_pos;
} ZipCentralHeader;

typedef struct ZipEndHeader{
  uint8_t signature[4];
  uint16_t number_of_disks;
  uint16_t disk_number_start;
  uint16_t number_of_disk_entries;
  uint16_t number_of_entries;
  uint32_t central_dir_size;
  uint32_t central_dir_offset;
  uint16_t file_comment_length;
} ZipEndHeader;

typedef struct ZipFile {
  FILE *fp;

  ZipHeader **zipheaders;
  ZipCentralHeader **zipcentralheaders;
  ZipEndHeader *zipendheader;
} ZipFile;

ZipEndHeader *readZipEndHeader(FILE *fp);
ZipCentralHeader *readZipCentralHeader(FILE *fp);
ZipHeader *readZipHeader(FILE *fp);
void printZipFileInfo(ZipFile *zipfile);
ZipFile *openZipFile(char *filename);
void closeZipFile(ZipFile *zipfile);

#endif   /* ZIP_INFO_H_INCLUDED */
