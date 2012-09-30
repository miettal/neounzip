#include "deflate.h"
#include "zip_info.h"
#include "debug.h"

void extractZipFile(ZipFile *zipfile);

int main(int argc, char *argv[])
{
  if(argc != 2){
    fprintf(stderr, "Invalid argument.\n");
    fprintf(stderr, "Usage: unzip filename\n");
    exit(1);
  }
  
  char *filename = argv[1];

  ZipFile *zipfile = openZipFile(filename);

  extractZipFile(zipfile);

  closeZipFile(zipfile);

  return 0;
}

void extractZipFile(ZipFile *zipfile)
{
  int r;

  for (uint16_t i = 0; i < zipfile->zipendheader->number_of_entries; i++) {
    info1("-----%s-----\n", zipfile->zipheaders[i]->file_name);

    FILE *fp = fopen(zipfile->zipheaders[i]->file_name, "w+");

    fseek(zipfile->fp, zipfile->zipheaders[i]->data_pos, SEEK_SET);
    switch(zipfile->zipheaders[i]->compression){
      case ZIP_COMPRESSION_TYPE_NO_COMPRESSION:
        for (uint16_t j = 0; j < zipfile->zipheaders[i]->compressed_size; j++) {
          fputc(fgetc(zipfile->fp), fp);
        }
        break;
      case ZIP_COMPRESSION_TYPE_DEFLATE:
        r = decodeDeflate(zipfile->fp, zipfile->zipheaders[i]->compressed_size, fp);

        if(r == -2){
          error("Can't extract %s\n", zipfile->zipheaders[i]->file_name);
          error("Unsupported Format\n");
        }else if(r < 0){
          error("Can't extract %s\n", zipfile->zipheaders[i]->file_name);
          error("Invalid Format\n");
        }
        break;
      default :
        //TODO
        break;
    }

    fclose(fp);
  }
}

