#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**

This program converts a text file into a .BMP image, if the image is opened in a text editor the original text appears

Written by Adrian Blanco

**/

//Pads a specified amount of zero byte to the file
void padZeroBytes(int num, FILE * wfile){
	
	for (int i = 0; i < num; ++i)
	{
		putc(0, wfile);
	}
}

//Writes an int to the file in little endian
void writeLittleEndian(int num, FILE * wfile){

	putc(num & 0xFF, wfile);
	putc((num >> 8) & 0xFF, wfile);
	putc((num >> 16) & 0xFF, wfile);
	putc((num >> 24) & 0xFF, wfile);
}

int main()
{
	char ch, file_name[40];
	int textfilesize, picside, arrayside, bitmapsize;
	bool eof = false;
	FILE * rFILE, * wFILE;
	
	printf("Enter the name of file\n");
	gets(file_name);
	rFILE = fopen(file_name,"r");

	if( rFILE == NULL ){
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}
	
	fseek(rFILE, 0L, SEEK_END); //Seeks to the end
	textfilesize = ftell(rFILE); //Counts the amount of chars in the file
	fseek(rFILE, 0L, SEEK_SET); //Seeks to the start

	printf("The size of the text is: %d kB\n", textfilesize/1000);

	//Each pixel requires 3 bytes, since we don't want any padding between bytes each side must be a multiple of 4 pixels (12 bytes)
	//Square to get the side, divide by 3 to get amount of pixels, then round up to the nearest multiple of 4
	picside = (int) ceil(sqrt(textfilesize) / (3 * 4)) * 4; 

	//Each pixel requires 3 chars
	arrayside = picside * 3;

	//Size of raw data in bytes in the bitmap (pixel array) including padding
	bitmapsize = (arrayside + arrayside % 4) * arrayside;

	//The array where we store all text
	char textarray[arrayside][arrayside];

	//We read the contents of the file into our array
	for (int i = 0; i < arrayside; ++i)
	{
		for (int j = 0; j < arrayside; ++j)
		{	
			//If the file has not stopped reading, we add the character to our array
			if(!eof){

				ch = fgetc(rFILE);
				
				//We do not write the EOF, since that's already taken care of
				if(ch == EOF){
					eof = true;
					textarray[i][j] = 0;
				}
				else{
					textarray[i][j] = ch;
				}
			}

			//Otherwise we pad it with a zero
			else{
				textarray[i][j] = 0;
			}

		}
	}


	//We lazily append .bmp to the file name, so our image has the same name as the text
	for (int i = 0; i < sizeof(file_name)/sizeof(char); ++i)
	{
		if(file_name[i] == '.'){

			file_name[i + 1] = 'b';
			file_name[i + 2] = 'm';
			file_name[i + 3] = 'p';

		}
	}

	wFILE = fopen(file_name, "w+");	
	if( wFILE == NULL ){
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	
	//Now we get to the fun part, writing the BMP file data

	/// BMP Header ///

	//ID field
	putc((int) 'B', wFILE);
	putc((int) 'M', wFILE);

	//Total size of the bmpfile
	int bmpfilesize = 54 + bitmapsize; //54 bytes of headers

	//We write it into the file as little endian
	writeLittleEndian(bmpfilesize, wFILE);

	//Unused application specific data
	padZeroBytes(4, wFILE);

	//54 bytes offset where the pixel array (bitmap data) can be found
	putc(54, wFILE);
	padZeroBytes(3, wFILE);


	/// DIB Header ///

	//There are 40 bytes in the DIB header
	putc(40, wFILE);
	padZeroBytes(3, wFILE);

	//Picture width in little endian
	writeLittleEndian(picside, wFILE);

	//Picture height in little endian 
	writeLittleEndian(picside, wFILE);

	//One color plane will be used
	putc(1, wFILE);
	putc(0, wFILE);

	//24 bits of information per pixel (8 bits per RGB color)
	putc(24, wFILE);
	putc(0, wFILE);

	//No pixel array compression is used
	padZeroBytes(4, wFILE);

	//Size of raw data in the bitmap
	writeLittleEndian(bitmapsize, wFILE);

	//Horizontal pixel resolution, doesn't matter in this case
	padZeroBytes(4, wFILE);

	//Vertical pixel resolution, doesn't matter in this case
	padZeroBytes(4, wFILE);

	//Number of colors in the palette
	padZeroBytes(4, wFILE);

	//Number of important colours
	padZeroBytes(4, wFILE);


	/// Bitmap Data ///

	//Now we write our text as pixel data in side the bmp file
	for (int i = 0; i < arrayside; ++i)
	{
		for (int j = 0; j < arrayside; ++j)
		{	
			putc(textarray[i][j], wFILE);
		}

		//We pad each row of pixels with zeroes so that the bytes are a multiple of 4
		//We made sure to have size the image so that it wouldn't need padding, but it's good to have just in case
		for (int p = 0; p < arrayside % 4; ++p)
		{
			putc(0, wFILE);
		}
	}

	fclose(rFILE);
	fclose(wFILE);
	return 0;
}
