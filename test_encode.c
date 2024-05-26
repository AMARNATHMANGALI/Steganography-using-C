#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "common.h"
#include "types.h"

int main(int argc, char *argv[])
{
	int ret;
	if(argc == 1)
	{
		printf("Please select encoding or decoding ""./a.out -e beautiful.bmp secret.txt\n");
		return 0;
	}

	if((argc == 1 || argc == 2) && (argv[1][1] == 'e'))
	{
		printf("Error please pass ./a.out -e beautiful.bmp secret.txt\n");//Checking whether option is encoding and printing error message
		return 0;
	}
	if((argc == 1 || argc == 2) && (argv[1][1] == 'd'))
	{
		printf("Error please pass ./a.out -d stego_img.bmp output.txt\n");//Checking whether option is decoding and printing error
		return 0;
	}
	if ( argc > 2 )    //Checking if arguments are greater than 2
	{
		ret = check_operation_type(argv);   //Checking operation type
		if( ret == e_encode)
		{
			EncodeInfo encInfo;    //Creating a variable
			printf("Selected encoding\n");
			if(read_and_validate_encode_args(argv,&encInfo) == e_success)  //Checking whether read and validate of arguments
			{
				printf("read and validate is successfully done\n");
				if(do_encoding(&encInfo) == e_success)      //Doing encoding
				{
					printf("Encoding successfully done\n");
				}
				else
				{
					printf("Encoding FAILURE\n");    //Printing error message
				}
			}
			else
			{
				printf("Read and validation is FAILURE\n");  //Printing error message of read and validation failure
			}
		}
		else if (ret == e_decode)

		{
			DecodeInfo DecInfo;  //Creating decode variable
			printf("Selected decoding\n");
			if(read_and_validate_decode_args(argc,argv,&DecInfo) == e_success)  //Checking the decode arguments 
			{
				printf("Read and validate is successfully done\n");
				if(do_decoding(&DecInfo) == e_success)   //Doing decoding
				{
					printf("Decoding successfully done\n");
				}
				else
				{
					printf("Decoding FAILURE\n");   //Printing error 
				}
			}
			else
			{
				printf("Read and validation FAILURE\n");  //printing that read and validation is failure
			}
		}
		else
		{
			printf("Unsupported\n");  //Printing unsupported
		}
	}
	return 0;
}
