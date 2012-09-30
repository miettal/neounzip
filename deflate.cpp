#include "deflate.h"

int decodeDeflate(FILE *src, uint32_t src_len, FILE *dst)
{
  unsigned char byte;
  unsigned char buf[BUFFER_MAX_SIZE];
  int buf_size = 0;

  int state = DEFLATE_STREAM_IN_FINAL;
  int bit_count;
  int bit;
  int final;
  int type;
  int data;
  int decode_data;
  int extra_bits;
  int extra_bits_data;
  int offset;
  int length;
  int dist;
  int ch;
  int end_data_flag;

  for (uint32_t i = 0, bit_count = 0; i < src_len; i++) {
    fread(&byte, sizeof(unsigned char), 1, src);

    for (int j = 0; j < 8; j++) {
      bit = (byte>>j)&1;
      bit_count++;
      switch(state){
        case DEFLATE_STREAM_IN_FINAL :
          if(bit_count == 1){
            final = 0;
          }
          final = (final<<1) + bit;
          if(bit_count == 1){
            state = DEFLATE_STREAM_IN_TYPE;
            bit_count = 0;
            switch(final){
              case DEFLATE_FINAL_LAST_BLOCK :
                info2("final:%d->not last block\n", final);
                break;
              case DEFLATE_FINAL_NOT_LAST_BLOCK :
                info2("final:%d->last block\n", final);
                break;
              default :
                return -1;
            }
          }
          break;
        case DEFLATE_STREAM_IN_TYPE :
          if(bit_count == 1){
            type = 0;
          }
          type += bit<<(bit_count-1);
          if(bit_count == 2){
            state = DEFLATE_STREAM_IN_DATA;
            bit_count = 0;
            switch(type){
              case DEFLATE_TYPE_NO_COMPRESSION:
                info2("type:%d->no compression\n", type);
                return -2;
                break;
              case DEFLATE_TYPE_FIXED_HUFFMAN:
                info2("type:%d->fixed huffman\n", type);
                break;
              case DEFLATE_TYPE_DYNAMIC_HUFFMAN:
                info2("type:%d->dynamic huffman\n", type);
                return -2;
                break;
              case DEFLATE_TYPE_RESERVED:
                info2("type:%d->reserved\n", type);
                return -2;
                break;
              default :
                error("unknown type:%d\n", type);
                return -1;
            }
            if(type != DEFLATE_TYPE_FIXED_HUFFMAN){
              return -2;
            }
          }
          break;
        case DEFLATE_STREAM_IN_DATA :
          if(bit_count == 1){
            data = 0;
          }

          end_data_flag = 0;
          data = (data<<1) + bit;
          if(bit_count == 7){
            if(0x00 <= data && data <= 0x17){
              end_data_flag = 1;
              decode_data = data+0x100;
            }
          }else if(bit_count == 8){
            if(0x30 <= data && data <= 0xbf){
              end_data_flag = 1;
              decode_data = data-0x30;
            }else if(0xc0 <= data && data <= 0xc7){
              end_data_flag = 1;
              decode_data = data-0xc0+0x118;
            }
          }else if(bit_count == 9){
            if(0xd0 <= data && data <= 0xff){
              end_data_flag = 1;
              decode_data = data-0x190+0x90;
            }else{
              error("unknown data:%03x\n", data);
              return -1;
            }
          }
          if(end_data_flag){
            if(0x00 <= decode_data && decode_data <= 0xff){
              state = DEFLATE_STREAM_IN_DATA;
              bit_count = 0;

              fputc(decode_data, dst);

              if(isgraph(decode_data)){
                info1("データ:0x%03x(%c)\n", decode_data, decode_data);
                info2("data:0x%03x->decode_data:0x%02x(%c)\n",
                    data, decode_data, decode_data);
              }else if(isspace(decode_data)){
                info1("データ:0x%03x( )\n", decode_data);
                info2("data:0x%03x->decode_data:0x%02x( )\n", data, decode_data);
              }else{
                info1("データ:0x%03x(?)\n", decode_data);
                info2("data:0x%03x->decode_data:0x%02x(?)\n", data, decode_data);
              }
            }else if(decode_data == 0x100){
              info1("データ:0x%03x(END)\n", decode_data);
              info2("data:0x%03x->decode_data:0x%02x(END)\n", data, decode_data);
              if(final == 1){
                return 0;
              }else{
                state = DEFLATE_STREAM_IN_FINAL;
                bit_count = 0;
              }
            }else if(0x101 <= decode_data && decode_data <= 0x11d){
              state = DEFLATE_STREAM_IN_LENGTH_EXTRA_BITS;
              bit_count = 0;

              switch(decode_data){
                case 257:
                  extra_bits = 0; offset = 3; break;
                case 258:
                  extra_bits = 0; offset = 4; break;
                case 259:
                  extra_bits = 0; offset = 5; break;
                case 260:
                  extra_bits = 0; offset = 6; break;
                case 261:
                  extra_bits = 0; offset = 7; break;
                case 262:
                  extra_bits = 0; offset = 8; break;
                case 263:
                  extra_bits = 0; offset = 9; break;
                case 264:
                  extra_bits = 0; offset = 10; break;
                case 265:
                  extra_bits = 1; offset = 11; break;
                case 266:
                  extra_bits = 1; offset = 13; break;
                case 267:
                  extra_bits = 1; offset = 15; break;
                case 268:
                  extra_bits = 1; offset = 17; break;
                case 269:
                  extra_bits = 2; offset = 19; break;
                case 270:
                  extra_bits = 2; offset = 23; break;
                case 271:
                  extra_bits = 2; offset = 27; break;
                case 272:
                  extra_bits = 2; offset = 31; break;
                case 273:
                  extra_bits = 3; offset = 35; break;
                case 274:
                  extra_bits = 3; offset = 43; break;
                case 275:
                  extra_bits = 3; offset = 51; break;
                case 276:
                  extra_bits = 3; offset = 59; break;
                case 277:
                  extra_bits = 4; offset = 67; break;
                case 278:
                  extra_bits = 4; offset = 83; break;
                case 279:
                  extra_bits = 4; offset = 99; break;
                case 280:
                  extra_bits = 4; offset = 115; break;
                case 281:
                  extra_bits = 5; offset = 131; break;
                case 282:
                  extra_bits = 5; offset = 163; break;
                case 283:
                  extra_bits = 5; offset = 195; break;
                case 284:
                  extra_bits = 5; offset = 227; break;
                case 285:
                  extra_bits = 0; offset = 258; break;
                default:
                  error("unknown data:%03x\n", data);
                  return -1;
                  break;
              }
              if(extra_bits == 0){
                state = DEFLATE_STREAM_IN_DIST;

                length = offset;
                info1("長さ:%d\n", length);
                info2("data:0x%03x->decode_data:0x%03x\n", data, decode_data);
                info2(" offset:%d\n", offset);
                info2(" extra_bits:%d\n", extra_bits);
                info2(" extra_bits_data:0\n");
                info2("  ->length:%d\n", length);
              }
            }
          }
          break;
        case DEFLATE_STREAM_IN_LENGTH_EXTRA_BITS :
          if(bit_count == 1){
            extra_bits_data = 0;
          }
          extra_bits_data += bit<<(bit_count-1);
          if(bit_count == extra_bits){
            state = DEFLATE_STREAM_IN_DIST;
            bit_count = 0;

            length = offset+extra_bits_data;
            info1("長さ:%d\n", length);
            info2("data:0x%03x->decode_data:0x%03x\n", data, decode_data);
            info2(" offset:%d\n", offset);
            info2(" extra_bits:%d\n", extra_bits);
            info2(" extra_bits_data:%d\n", extra_bits_data);
            info2("  ->length:%d\n", length);
          }
          break;
        case DEFLATE_STREAM_IN_DIST :
          if(bit_count == 1){
            data = 0;
          }
          data = (data<<1) + bit;
          if(bit_count == 5){
            state = DEFLATE_STREAM_IN_DIST_EXTRA_BITS;
            bit_count = 0;

            switch(data){
              case 0:
                extra_bits = 0; offset = 1; break;
              case 1:
                extra_bits = 0; offset = 2; break;
              case 2:
                extra_bits = 0; offset = 3; break;
              case 3:
                extra_bits = 0; offset = 4; break;
              case 4:
                extra_bits = 1; offset = 5; break;
              case 5:
                extra_bits = 1; offset = 7; break;
              case 6:
                extra_bits = 2; offset = 9; break;
              case 7:
                extra_bits = 2; offset = 13; break;
              case 8:
                extra_bits = 3; offset = 17; break;
              case 9:
                extra_bits = 3; offset = 25; break;
              case 10:
                extra_bits = 4; offset = 33; break;
              case 11:
                extra_bits = 4; offset = 49; break;
              case 12:
                extra_bits = 5; offset = 65; break;
              case 13:
                extra_bits = 5; offset = 97; break;
              case 14:
                extra_bits = 6; offset = 129; break;
              case 15:
                extra_bits = 6; offset = 193; break;
              case 16:
                extra_bits = 7; offset = 257; break;
              case 17:
                extra_bits = 7; offset = 385; break;
              case 18:
                extra_bits = 8; offset = 513; break;
              case 19:
                extra_bits = 8; offset = 769; break;
              case 20:
                extra_bits = 9; offset = 1025; break;
              case 21:
                extra_bits = 9; offset = 1537; break;
              case 22:
                extra_bits = 10; offset = 2049; break;
              case 23:
                extra_bits = 10; offset = 3073; break;
              case 24:
                extra_bits = 11; offset = 4097; break;
              case 25:
                extra_bits = 11; offset = 6145; break;
              case 26:
                extra_bits = 12; offset = 8193; break;
              case 27:
                extra_bits = 12; offset = 12289; break;
              case 28:
                extra_bits = 13; offset = 16385; break;
              case 29:
                extra_bits = 13; offset = 24577; break;
              default :
                error("unknown data:%03x\n", data);
                return -1;
            }
            if(extra_bits == 0){
              state = DEFLATE_STREAM_IN_DATA;

              dist = offset;
              info1("距離:%d\n", dist);
              info2("data:0x%03x\n", data);
              info2(" offset:%d\n", offset);
              info2(" extra_bits:%d\n", extra_bits);
              info2(" extra_bits_data:0\n");
              info2("  ->dist:%d\n", dist);

              for (int k = 0; k < length; k++) {
                fseek(dst, -dist , SEEK_CUR);
                ch = fgetc(dst);
                info2("char:%c(%d)\n", ch, ch);
                fseek(dst, dist-1 , SEEK_CUR);
                fputc(ch, dst);
              }
            }
          }
          break;
        case DEFLATE_STREAM_IN_DIST_EXTRA_BITS :
          if(bit_count == 1){
            extra_bits_data = 0;
          }
          extra_bits_data += bit<<(bit_count-1);
          if(bit_count == extra_bits){
            state = DEFLATE_STREAM_IN_DATA;
            bit_count = 0;

            dist = offset+extra_bits_data;
            info1("距離:%d\n", dist);
            info2("data:0x%03x\n", data);
            info2(" offset:%d\n", offset);
            info2(" extra_bits:%d\n", extra_bits);
            info2(" extra_bits_data:%d\n", extra_bits_data);
            info2("  ->dist:%d\n", dist);
            for (int k = 0; k < length; k++) {
              fseek(dst, -dist , SEEK_CUR);
              ch = fgetc(dst);
              info2("char:%c(%d)\n", ch, ch);
              fseek(dst, dist-1 , SEEK_CUR);
              fputc(ch, dst);
            }
          }
          break;
        default :
          error("unknown data:%03x\n", data);
          return -1;
          break;
      }
    }
  }

  return -1;
}
