#ifndef DECODE_H
#define DECODE_H
#include "types.h" // Contains user defined types
#include "common.h"
/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

/* Get file extention size by decoding 32 bytes */
#define DECODE_FILE_EXTN_SIZE 32

/* Get file size by decoding 32 bytes*/
#define DECODE_FILE_SIZE 32

typedef struct _DecodeInfo
{
    /* Encoded stego image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    uint image_data_size;
    char image_data[MAX_IMAGE_BUF_SIZE];
	long output_file_extn_size;
	long output_file_data_size;

    /* Decoded output file info */
    char output_file_name[100];
    FILE *fptr_output_file;
    char output_file_extn[MAX_FILE_SUFFIX];
    char decode_data[MAX_SECRET_BUF_SIZE];
    char magic_string[100];

    /* Passcode */
    char password[100];
    uint password_size;

}DecodeInfo;

//Read and validate Decode args from argv
Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo);

//Get File pointers for i/p and o/p files //
Status open_decode_files(DecodeInfo *decInfo);

//Perform the decoding //
Status do_decoding(DecodeInfo *decInfo);

//Decode the magic string(DecodeInfo *decinfo)//
Status decode_magic_string( char *password,DecodeInfo *decInfo);

//Decode the size of the secret file extension from the source image//
Status decode_output_file_extn_size(DecodeInfo *decInfo);

//Decode the secret file extension from the source image //
Status decode_output_file_extn( uint size, DecodeInfo *decInfo);

//Decode the size of the secret file data from the source image//
Status decode_file_size(DecodeInfo *decInfo);

//Decode the secret file data from the source image //
Status decode_file_data(DecodeInfo *decInfo);

//Decode the data from the source image //
Status decode_lsb_to_byte(char *decode_data, char *image_data);

Status decode_lsb_to_size(char *decode_data,long* size);


//Decode the size from the least significant bits(LSB)//
  //Status decode_size_from_lsb(char image_buffer, DecodeInfo *decInfo);

#endif


