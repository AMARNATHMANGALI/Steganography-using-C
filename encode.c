#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_stego_image)
{
	char str[100];
	rewind(fptr_src_image);
	fread(str,54,1,fptr_src_image);               //Copying header file
	fwrite(str,54,1,fptr_stego_image);
	return e_success;
}
OperationType check_operation_type(char *argv[])
{
	if( argv[1][1] == 'e')
	{
		return e_encode;
	}
	else if( argv[1][1] == 'd')           //Checking operation type using argument vector
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
}
Status encode_magic_string(const char* magic_string,EncodeInfo *encInfo)
{
	int i;
	char arr[8];
	for(i=0;magic_string[i] != 0;i++)
	{
		fread(arr,8,1,encInfo->fptr_src_image);     //Function for encoding the magic string 
		encode_byte_to_lsb(magic_string[i],arr);    //Calling the function byte to lsb
		fwrite(arr,8,1,encInfo->fptr_stego_image);
	}
	return e_success;
}
Status encode_byte_to_lsb(char data, char* buffer_image)
{
	int j=0,i=0;
	for(i=7;i>=0;i--)
	{
		buffer_image[j] = ((data & (1<<i)) >> i) | (buffer_image[j] & (~1));  //Converting 1 bit to 1 byte 
		j++;
	}
	return e_success;
}
Status encode_secret_file_extn_size(int size,EncodeInfo *encInfo)
{
	char arr[32];
	fread(arr,32,1,encInfo->fptr_src_image);           //Encoding secret file extension size
	encode_size_to_lsb(size,arr);                      //Calling the function encode size to lsb
	fwrite(arr,32,1,encInfo->fptr_stego_image);
	return e_success;
}
Status encode_size_to_lsb( int size,char* buff_image)
{
	int i,j=0;
	for(i=31;i>=0;i--)
	{
		buff_image[j] = ((size & (1<<i)) >> i) | (buff_image[j] & (~1));  //Converting integer value to bytes
		j++;
	}
	return e_success;

}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	char str[8];
	int i;
	for(i=0;file_extn[i] != 0;i++)
	{
		fread(str,8,1,encInfo->fptr_src_image);           //encoding secret file extension 
		encode_byte_to_lsb(file_extn[i],str);
		fwrite(str,8,1,encInfo->fptr_stego_image);
	}
	return e_success;
}
uint get_file_size(	FILE *fptr)
{
	fseek(fptr,0,SEEK_END);           //Getting file size using fseek function
	int len = ftell(fptr);
	return len;
}
Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
	char arr[32];
	fread(arr,32,1,encInfo->fptr_src_image);
	encode_size_to_lsb(file_size,arr);           //encoding secret file size 
	fwrite(arr,32,1,encInfo->fptr_stego_image);
	return e_success;

}
//Encoding the file date using its size until null character or less than size
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	int i;
	rewind(encInfo->fptr_secret);
	char arr[encInfo->size_secret_file],str[8];
	fread(arr,encInfo->size_secret_file,1,encInfo->fptr_secret);
	for(i=0; i < encInfo->size_secret_file;i++)
	{
		fread(str,8,1,encInfo->fptr_src_image);
		encode_byte_to_lsb(arr[i],str);
		fwrite(str,8,1,encInfo->fptr_stego_image);
	}
	return e_success;
}
//Copying remaing data of file until EOF
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char ch;
	while (fread(&ch,1,1,fptr_src) > 0)
	{
		fwrite(&ch,1,1,fptr_dest);
	}
	return e_success;

}
Status check_capacity(EncodeInfo *encInfo)   //Calling check capacity function
{
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);  //Calling get image size function

	strcpy(encInfo->extn_secret_file , strstr(encInfo->secret_fname,"."));   //Copying extension

	int size = strlen(encInfo->extn_secret_file);   //Finding length of extension

	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);  //Getting secret file size

	uint img_size;

	img_size = get_image_size_for_bmp(encInfo->fptr_src_image);  //Getting image size using get image size function

	printf("INFO : Image size = %u\n",img_size);  //Printing image size

	if(encInfo->image_capacity > (54 + 40 + 32 + (size*8) + 32 + ( encInfo->size_secret_file * 8))) //Checking capacity of image
	{
		return e_success;  //Returning success
	}
	else
	{
		return e_failure;  //returning failure
	}

}


uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);  

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}
Status do_encoding(EncodeInfo *encInfo)
{
	if (open_files(encInfo) == e_success)   //Opening required files
	{
		printf("SUCCESS: %s function COMPLETED\n", "open_files" );  //Printing success message
		if ( check_capacity(encInfo) == e_success)  //Calling check capacity function
		{
			printf("Capacity OK\n");  //Printing success message
			if (copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success) //Copying header file
			{
				printf("Success\n");   //Printing success message

				if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)		//Call magic string function
				{
					printf("Magic string encoded\n");  //Printing success message
					strcpy(encInfo->extn_secret_file , strstr(encInfo->secret_fname,"."));

					int size = strlen(encInfo->extn_secret_file);

					if(encode_secret_file_extn_size(size,encInfo) == e_success)    //Encode integer value
					{
						printf("Secret file extn size is encoded\n");    //Printing success message
						if(encode_secret_file_extn( encInfo->extn_secret_file, encInfo) == e_success) //getting secret file extension
						{
							printf("Secret file extn is encoded\n");  //Printing success message
					
						}
						encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

						if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)	//Getting secret file size
						{
							printf("Secret file size encoded\n");
							if(encode_secret_file_data(encInfo) == e_success)     //encoding secret file data
							{
								printf("Secret file data encoded\n");
								if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) //encoding remaining data
								{
									printf("Remaining data encoded\n");
								}
								else
								{
									printf("Remaining data not encoded\n");   //returning failure
									return e_failure;
								}
							}
							else
							{
								printf("Sectet file data not encoded\n");  //printing error message and returning failure
								return e_failure;
							}

						}
						else
						{
							printf("Secret file size not encoded\n"); //printing error message and return failure
							return e_failure;
						}

					}
					else
					{
						printf("Secret file extension not encoded\n");  //printing error message
						return e_failure;
					}
				}

				else
				{
					printf("Magic string not encoded\n");  //printing error
					return e_failure;
				}

			}
			else
			{
				printf("FAILED\n");  //Printing error message
				return e_failure;
			}
		}
			else
			{
				printf("Failure\n");  //Printing error mesage
				return e_failure;
			}
		//call all functions
	}
	else
	{
		printf("FAILED: %s function failure\n", "open_files" );  //Printing error message
		return e_failure;
	}
	return e_success;  //returning success
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)  //Reading and validating arguments
{
	char arr[5];
	strcpy( arr,strstr(argv[2] , "."));     //copying extension
	if(strcmp(arr,".bmp") == 0)
	{
		encInfo->src_image_fname = argv[2];    //storing the file name in a pointer
	}
	else
	{
		return e_failure;
	}
	if(argv[3] != 0)     //Checking whether argument is passed or not
	{
		strcpy(arr,strstr(argv[3],"."));   //copying extension
		if ( (strcmp(arr,".txt") == 0) || (strcmp(arr,".c") == 0) || (strcmp(arr,".sh") == 0 ))
		{
			encInfo->secret_fname = argv[3];   //Copying secret file name
		}
		else
		{
			return e_failure; //Return failure
		}
	}
	else
	{
		printf("Please pass secret file\n");  //Prompt user to enter secret file name
		return e_failure;
	}
	if(argv[4] != 0)
	{
		strcpy(arr,strstr(argv[4],"."));
		if(strcmp(arr,".bmp") == 0)
		{
			encInfo->stego_image_fname = argv[4];  //Copying output file name if passed
		}
		else
		{
			printf("Extension should be .bmp\n");  //Prompt user to give extension
			return e_failure;
		}
	}
	else
	{
		encInfo->stego_image_fname = "stego_img.bmp";  //Giving default name 
	}

	return e_success;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	printf("%s\n",encInfo->stego_image_fname);
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}
	// No failure return e_success
	return e_success;
}
