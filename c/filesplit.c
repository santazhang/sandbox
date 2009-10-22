#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

typedef unsigned char byte;

#define MAX_BUF_SIZE 4194304

int main(int argc, char* argv[]) {
  int i;
  int block_size;
  int block_id;
  FILE* fp_src;
  FILE* fp_dst;
  byte* buffer;
  __int64 src_fsize;
  __int64 bytes_written;
  int buffer_size;
  char* fn;
  char num_ext[16];
  if (argc == 1) {
    printf("Usage: filesplit file block-size\n");
    printf("eg: filesplit movie.avi 32m\n");
    return 1;
  }
  fn = (char *) malloc(strlen(argv[1]) + 16);
  for (i = 0; '0' <= argv[2][i] && argv[2][i] <= '9'; i++)
    ;
  if (argv[2][i] == 'm' || argv[2][i] == 'M') {
    argv[2][i] = '\0';
    block_size = atoi(argv[2]) * 1024 * 1024;
  } else if (argv[2][i] == 'k' || argv[2][i] == 'K') {
    argv[2][i] = '\0';
    block_size = atoi(argv[2]) * 1024;
  } else {
    argv[2][i] = '\0';
    block_size = atoi(argv[2]);
  }
  printf("Spliting: block size = %d byte(s)\n", block_size);
  fp_src = fopen(argv[1], "rb");
  _fseeki64(fp_src, 0, SEEK_END);
  src_fsize = _ftelli64(fp_src);
  printf("Original file size = %ld byte(s)\n", src_fsize);
  bytes_written = 0;
  _fseeki64(fp_src, 0, SEEK_SET);
  buffer_size = min(MAX_BUF_SIZE, block_size);
  buffer = (byte *) malloc(buffer_size);
  block_id = 1;
  while (bytes_written < src_fsize) {
    int bytes_written_this_block = 0;
    strcpy(fn, argv[1]);
    num_ext[0] = '_';
    itoa(block_id, num_ext + 1, 10);
    strcat(fn, num_ext);
    printf("Creating split block %d:", block_id);
    fp_dst = fopen(fn, "wb");
    while (bytes_written_this_block < block_size) {
      if (bytes_written_this_block + buffer_size < block_size) {
        // not last one
        fread(buffer, 1, buffer_size, fp_src);
        fwrite(buffer, 1, buffer_size, fp_dst);
        bytes_written_this_block += buffer_size;
      } else {
        // last one
        fread(buffer, 1, block_size - bytes_written_this_block, fp_src);
        fwrite(buffer, 1, block_size - bytes_written_this_block, fp_dst);
        bytes_written_this_block = block_size;
      }
    }
    fclose(fp_dst);
    bytes_written += block_size;
    block_id++;
    printf("Size written = %d (bytes), progress = %d%%\n", bytes_written_this_block, (bytes_written * 100) / src_fsize);
    if (bytes_written + block_size > src_fsize) {
      block_size = src_fsize - bytes_written;
    }
  }
  fclose(fp_src);
  
  strcpy(fn, argv[1]);
  strcat(fn, "_merge.bat");
  fp_dst = fopen(fn, "w");
  
  // create merge bat
  // remove path
  i = strlen(argv[1]) - 1;
  while (i > 0 && argv[1][i] != '\\' && argv[1][i] != '/') {
    i--;
  }
  if (argv[1][i] == '\\' || argv[1][i] == '/')
    i++;
  strcpy(fn, &argv[1][i]);
  fprintf(fp_dst, "copy ");
  for (i = 1; i < block_id; i++) {
    if (i != 1) {
      fprintf(fp_dst, " + ");
    }
    fprintf(fp_dst, " \"%s_%d\" /b ", fn, i);
  }
  fprintf(fp_dst, " \"%s\"", argv[1]);
  fclose(fp_dst);
  free(fn);
  return 0;
}

