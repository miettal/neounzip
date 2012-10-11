#include "zip_info.h"

ZipEndHeader *readZipEndHeader(FILE *fp)
{
  ZipEndHeader *zipendheader = (ZipEndHeader*)malloc(sizeof(ZipEndHeader));

  fread(&zipendheader->signature, sizeof(uint8_t), 4, fp);
  fread(&zipendheader->number_of_disks, sizeof(uint16_t), 1, fp);
  fread(&zipendheader->disk_number_start, sizeof(uint16_t), 1, fp);
  fread(&zipendheader->number_of_disk_entries, sizeof(uint16_t), 1, fp);
  fread(&zipendheader->number_of_entries, sizeof(uint16_t), 1, fp);
  fread(&zipendheader->central_dir_size, sizeof(uint32_t), 1, fp);
  fread(&zipendheader->central_dir_offset, sizeof(uint32_t), 1, fp);
  fread(&zipendheader->file_comment_length, sizeof(uint16_t), 1, fp);

  return zipendheader;
}

ZipCentralHeader *readZipCentralHeader(FILE *fp)
{
  ZipCentralHeader *zipcentralheader = (ZipCentralHeader*)malloc(sizeof(ZipCentralHeader));

  fread(&zipcentralheader->signature, sizeof(uint8_t), 4, fp);
  fread(&zipcentralheader->version_made, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->version, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->flags, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->compression, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->dos_time, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->dos_date, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->crc32, sizeof(uint32_t), 1, fp);
  fread(&zipcentralheader->compressed_size, sizeof(uint32_t), 1, fp);
  fread(&zipcentralheader->uncompressed_size, sizeof(uint32_t), 1, fp);
  fread(&zipcentralheader->file_name_length, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->extra_field_length, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->file_comment_length, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->disk_number_start, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->internal_file_attributes, sizeof(uint16_t), 1, fp);
  fread(&zipcentralheader->external_file_attributes, sizeof(uint32_t), 1, fp);
  fread(&zipcentralheader->position, sizeof(uint32_t), 1, fp);
  zipcentralheader->file_name = (char*)malloc(sizeof(char) * (zipcentralheader->file_name_length+1));
  fread(zipcentralheader->file_name, sizeof(char), zipcentralheader->file_name_length, fp);
  zipcentralheader->file_name[zipcentralheader->file_name_length] = '\0';
  zipcentralheader->extra_field_pos = ftell(fp);
  fseek(fp, zipcentralheader->extra_field_length, SEEK_CUR);

  return zipcentralheader;
}

ZipHeader *readZipHeader(FILE *fp)
{
  ZipHeader *zipheader = (ZipHeader*)malloc(sizeof(ZipHeader));

  fread(&zipheader->signature, sizeof(uint8_t), 4, fp);
  fread(&zipheader->version, sizeof(uint16_t), 1, fp);
  fread(&zipheader->flags, sizeof(uint16_t), 1, fp);
  fread(&zipheader->compression, sizeof(uint16_t), 1, fp);
  fread(&zipheader->dos_time, sizeof(uint16_t), 1, fp);
  fread(&zipheader->dos_date, sizeof(uint16_t), 1, fp);
  fread(&zipheader->crc32, sizeof(uint32_t), 1, fp);
  fread(&zipheader->compressed_size, sizeof(uint32_t), 1, fp);
  fread(&zipheader->uncompressed_size, sizeof(uint32_t), 1, fp);
  fread(&zipheader->file_name_length, sizeof(uint16_t), 1, fp);
  fread(&zipheader->extra_field_length, sizeof(uint16_t), 1, fp);
  zipheader->file_name = (char*)malloc(sizeof(char) * (zipheader->file_name_length+1));
  fread(zipheader->file_name, sizeof(char), zipheader->file_name_length, fp);
  zipheader->file_name[zipheader->file_name_length] = '\0';
  zipheader->extra_field_pos = ftell(fp);
  fseek(fp, zipheader->extra_field_length, SEEK_CUR);
  zipheader->data_pos = ftell(fp);
  fseek(fp, zipheader->compressed_size, SEEK_CUR);

  return zipheader;
}


void printZipFileInfo(ZipFile *zipfile)
{
  printf("エントリ数：%d\n", zipfile->zipendheader->number_of_entries);
  for (uint16_t i = 0; i < zipfile->zipendheader->number_of_entries; i++) {
    printf("-ファイル名        ：%s\n", zipfile->zipcentralheaders[i]->file_name);
    printf(" ファイルサイズ    ：%u\n", zipfile->zipcentralheaders[i]->uncompressed_size);
    printf(" 圧縮ファイルサイズ：%u\n", zipfile->zipcentralheaders[i]->compressed_size);
    printf(" 圧縮形式          ：");
    switch(zipfile->zipcentralheaders[i]->compression){
      case ZIP_COMPRESSION_TYPE_NO_COMPRESSION:
        printf("非圧縮\n");
        break;
      case ZIP_COMPRESSION_TYPE_DEFLATE:
        printf("Deflate圧縮\n");
        break;
      default :
        printf("非対応圧縮\n");
        break;
    }
  }
}

ZipFile *openZipFile(char *filename)
{
  ZipFile *zipfile = (ZipFile*)malloc(sizeof(ZipFile));
  zipfile->fp = fopen(filename, "rb");

  fseek(zipfile->fp, -22, SEEK_END);
  zipfile->zipendheader = readZipEndHeader(zipfile->fp);
  zipfile->zipcentralheaders = (ZipCentralHeader**)malloc(sizeof(ZipCentralHeader)*zipfile->zipendheader->number_of_entries);
  zipfile->zipheaders = (ZipHeader**)malloc(sizeof(ZipHeader)*zipfile->zipendheader->number_of_entries);

  fseek(zipfile->fp, zipfile->zipendheader->central_dir_offset, SEEK_SET);
  for (uint16_t i = 0; i < zipfile->zipendheader->number_of_entries; i++) {
    zipfile->zipcentralheaders[i] = readZipCentralHeader(zipfile->fp);
  }

  fseek(zipfile->fp, 0, SEEK_SET);
  for (uint16_t i = 0; i < zipfile->zipendheader->number_of_entries; i++) {
    zipfile->zipheaders[i] = readZipHeader(zipfile->fp);
  }

#if DEBUG >= DEBUG_LEVEL_INFO1
  printZipFileInfo(zipfile);
#endif

  return zipfile;
}

void closeZipFile(ZipFile *zipfile)
{
  for (uint16_t i = 0; i < zipfile->zipendheader->number_of_entries; i++) {
    free(zipfile->zipcentralheaders[i]->file_name);
    free(zipfile->zipcentralheaders[i]);
  }

  for (uint16_t i = 0; i < zipfile->zipendheader->number_of_entries; i++) {
    free(zipfile->zipheaders[i]->file_name);
    free(zipfile->zipheaders[i]);
  }

  free(zipfile->zipcentralheaders);
  free(zipfile->zipheaders);
  free(zipfile->zipendheader);
  fclose(zipfile->fp);

  free(zipfile);
}
