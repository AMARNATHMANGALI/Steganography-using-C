#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"

Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
	if(argc > 2)
	{
		if(strcmp(strstr(argv[2], "."),".bmp") == 0)
		{
			decInfo->stego_image_fname = argv[2];      //Storing input the image name 
		}
		else
		{
			fprintf(stderr,"Error : Stego image %s format should be .bmp\n", argv[2]);  //printing error message
			return e_failure;
		}
	}
	else
	{
		fprintf(stderr,"Error : Arguments are misssing\n");
		printf("Test_encode: Encoding: ./a.out -e <.bmp file> <.txt file> [Output file]\n");  //Printing error message
		printf("./Test_encode: Deconding: ./a.out -d <.bmp file> [output file]\n");
		return e_failure;
	}
	if(argc > 3)
	{

		strcpy(decInfo->output_file_name,argv[3]);  //Copying output file name
	}
	else
	{
		strcpy(decInfo->output_file_name,"out");  //Giving default name
	}

	return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
	printf("INFO : Decoding Procedure Started\n");   
	printf("INFO : Opening required files\n");
	if(open_decode_files(decInfo) == e_success)   //Calling the file opening function
	{
		uint data;
		printf("Enter magic string\n");
		scanf("%s",decInfo->password);   //Taking user input for password
		decInfo->password_size = strlen(decInfo->password);   //Calculating size of password
		if(decode_magic_string(decInfo->password, decInfo) == e_success)   //Decoding the magic string
		{
			printf("INFO : Magic string Decoded\n");   //printing that magic string decoded
			if(decode_output_file_extn_size(decInfo ) == e_success) //Decoding file extension size
			{
				printf("Extension size successfully decoded\n");  //Printing success message
				if(decode_output_file_extn(decInfo->output_file_extn_size,decInfo) == e_success)  //Decoding outpu file extension
				{
					strcat(decInfo->output_file_name,decInfo->output_file_extn);  //Concatinate extension and file name
					printf("Output file extension is decoded\n");
					if(decode_file_size(decInfo) == e_success)  //Decoding file size
					{
						printf("File size is decoded\n");
						if(decode_file_data(decInfo) == e_success)   //Decoding file data
						{
							printf("File data decoded\n");
						}
						else
						{
							printf("Data not decoded\n");  //Returning failure
							return e_failure;
						}
					}
					else
					{
						printf("File size not decoded\n");
						return e_failure;  //Returning failure
					}
				}
				else
				{
					printf("Output file extension is not decoded\n");
					return e_failure;  //Returning failure
				}
			}
			else
			{
				printf("Extension size not decoded\n");
				return e_failure;   //Returning failure
			}
		}

			else
			{
				printf("Decoding magic string failed\n");
				return e_failure;  //Returning failure
			}
		}
	
	else
	{
		printf("ERROR opening in files\n");//print error message
		return e_failure;
	}
	return e_success;  //Returning success
}
Status open_decode_files(DecodeInfo *decInfo)
{
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname,"r");  //opeming input file and storing in filepointer
	if(decInfo->fptr_stego_image == NULL)  //opening file if it is not null
	{
		perror("fopen");
		fprintf(stderr,"Error : unable to open file %s\n",decInfo->stego_image_fname);
		return e_failure;  //return failure
	}
	else
	{
		printf("INFO : opened %s\n",decInfo->stego_image_fname); //Prompting user that file is opened
	}
	return e_success;  //return success
}
Status decode_magic_string( char* password,DecodeInfo *decInfo)
{
	fseek(decInfo->fptr_stego_image,54,SEEK_SET);   //Skipping 54 bytes becaue they are for header fils
	char arr[8];
	int i;
	for(i = 0;i<decInfo->password_size;i++)
	{
		fread(arr,8,1,decInfo->fptr_stego_image);
		decode_lsb_to_byte(arr,&(decInfo->magic_string[i]));   //decoding magic string byte to lsb
	}
	decInfo -> magic_string[i] = '\0';  //adding null character at last
	if( strcmp(password,decInfo->magic_string) == 0 )
	{
		printf("Password matched\n");   //Comparing magic string and user entered password
		return e_success;
	}
	else
	{
		printf("Password incorrect\n");  //Printing error and returning failure
		return e_failure;
	}

}
Status decode_lsb_to_byte(char*decode_data,char*magic_string)   //Function for lsb to byte
{
	int i,j=0;
	*magic_string=0;

	for(i=7;i>=0;i--)
	{
		*magic_string = (decode_data[j] & 1) << i | *magic_string;   //Converting the lsb to byte
		j++;
	}
}
Status decode_output_file_extn_size(DecodeInfo* decInfo)   //Decoding output file extension size
{
	char arr[32];
	fread(arr,32,1,decInfo->fptr_stego_image);  //Reading 32 bytes
	decode_lsb_to_size(arr,&(decInfo->output_file_extn_size));  //Calling lsb to size function
}
Status decode_lsb_to_size(char* decode_data,long* size )
{
	int i,j=0;
	*size=0;
	for(i=31;i>=0;i--)
	{
		*size = (decode_data[j] & 1) << i | *size;   //Lsb to byte function converting the lsb into a byte
		j++;
	}
	return e_success;
}   
Status decode_output_file_extn(uint size,DecodeInfo *decInfo)   //Decoding output file extension
{
	int i;
	char arr[8];
	for(i=0;i< size; i++)
	{
		fread(arr,8,1,decInfo->fptr_stego_image);    //Reading bytes based on size
		decode_lsb_to_byte(arr,&(decInfo->output_file_extn[i])); //Calling lsb to byte function
	}
	decInfo->output_file_extn[i] = '\0';   //adding null character at last
	return e_success;

}
Status decode_file_size(DecodeInfo *decInfo) //Decoding file size
{ 
	char arr[32];
	fread(arr,32,1,decInfo->fptr_stego_image);   //Reading 32 bytes from image
	decode_lsb_to_size(arr,&(decInfo->output_file_data_size));  //Calling lsb to size function
	return e_success;

}
Status decode_file_data(DecodeInfo *decInfo)     //Decoding file data
{
	int i;
	char ch,arr[8];
	FILE* fptr_output_file = fopen(decInfo->output_file_name,"w");  //Opening output file in write mode
	for(i=0;i<decInfo->output_file_data_size;i++)  //running loop for size times
	{
		fread(arr,8,1,decInfo->fptr_stego_image);
		decode_lsb_to_byte(arr,&ch);    //calling lsb to byte function
		fwrite(&ch,1,1,fptr_output_file);  //writing character by character
	}
	fclose(fptr_output_file);   //closing the file
	return e_success;

}

