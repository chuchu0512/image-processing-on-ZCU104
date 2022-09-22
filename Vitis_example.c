/*----- include headers -----*/
#include "platform.h" // hello world module
#include "xgpio.h" // axi gpio in vivado

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "xil_printf.h" // using xil_printf
#include "xparameters.h" // important parameters
#include "ff.h" // FatFs using
#include "xil_io.h" // PS-DDR address
#include "sleep.h"
#include "xtime_l.h" //timer

/*----- define bmp file -----*/

typedef long UINT32;
typedef unsigned short int UINT16;
typedef unsigned char BYTE ;

#define HEADER_SIZE 14
#define INFO_SIZE 40
#define DataSize width*height*3

#define U16(x)    ((unsigned short) (x))
#define U32(x)    ((int) (x))
#define B2U16(bytes,offset)  (U16(bytes[offset]) | U16(bytes[offset+1]) << 8)
#define B2U32(bytes,offset)  (U32(bytes[offset]) | U32(bytes[offset+1]) << 8 | U32(bytes[offset+2]) << 16 | U32(bytes[offset+3]) << 24)

/*----- define fatfs sd card -----*/
FATFS FatFs ;
FIL fil ;
FRESULT fr ;

/*----- define ddr address -----*/
#define BASE 0x10000000

int main()
{
	/*----- initialize the platform -----*/
	init_platform() ;

	/*----- define timer -----*/
	XTime start_r = 0 ;
	XTime end_r = 0 ;
	XTime start_w = 0 ;
	XTime end_w = 0 ;
	XTime start_ip = 0 ;
	XTime end_ip = 0 ;

	/*----- define bmp file structure -----*/
	UINT16 signature; // Magic Number = "BM" = 0x4D42
	UINT32 fileSize; // File size in bytes
	UINT32 dataOffset; // File offset to Raster Data
	// InfoHeader
	UINT32 size; // Size of InfoHeader =40
	UINT32 width; // Bitmap Width
	UINT16 height; // Bitmap Height
	UINT16 planes; // Number of Planes (=1)
	UINT16 bitsPerPixel; // Bits per Pixel, 1, 4, 8, 16, 24
	UINT32 compression; // Type of Compression, 0 = BI_RGB no compression, 1 = BI_RLE8 8bit RLE encoding, 2 = BI_RLE4 4bit RLE encoding
	UINT32 imageSize; // (compressed) Size of Image, It is valid to set this =0 if Compression = 0
	UINT32 xPixelsPerM; // horizontal resolution: Pixels/meter
	UINT32 yPixelsPerM; // vertical resolution: Pixels/meter
	UINT32 colorsUsed; // Number of actually used colors
	UINT32 colorsImportant; // Number of important colors , 0 = all

	/*----- create header and info registers -----*/
	BYTE *header ;
	header = (BYTE *)malloc(14*sizeof(BYTE)) ;
	*header = 0 ;

	BYTE *info ;
	info = (BYTE *)malloc(40*sizeof(BYTE)) ;
	*info = 0 ;

	unsigned int count = 0 ; // f_read and f_write return value

	f_mount(&FatFs, "", 0) ; // mount sd card
	/*----- define ui -----*/
	xil_printf("now starting program ... \n \n \r") ;
	xil_printf("input bmp file name or enter \"exit\" to finish the program : ") ;
	char filename[100] ;
	scanf("%s", filename) ;
	xil_printf("\n \n \r") ;
	if(strcmp(filename, "exit") == 0)
	{
		xil_printf("finish the program ... \n \r") ;
		return 0 ;
	}

	/*----- input file -----*/
	char in_f[] = ".bmp" ;
	int in_len = strlen(filename)+strlen(in_f)+1 ;
	char *in_filename = malloc(in_len*sizeof(char)) ;
	*in_filename = 0 ;
	strcat(in_filename, filename) ;
	strcat(in_filename, in_f) ;

	/*----- open the sd card -----*/
	fr = f_open(&fil, in_filename, FA_OPEN_EXISTING | FA_READ) ;
	if(fr != FR_OK)
	{
		xil_printf("error while input bmp : %d !! \n \r", fr) ;
		return main() ;
	}
	xil_printf("input filename : %s \n \n \r", in_filename) ;
	xil_printf("program will keep go on in : ") ;
	xil_printf("5 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("4 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("3 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("2 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("1 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("0 seconds \n \r") ;
	sleep(1) ;
	free(in_filename) ;


	/*----- read sd card data -----*/
	xil_printf("start to read bmp info ... \n \r") ;

	fr = f_read(&fil, header, HEADER_SIZE, &count) ; // read header data
	fr = f_read(&fil, info, INFO_SIZE, &count) ; // read info data

	/*----- write structure -----*/
	signature = B2U16(header,0);
		assert(signature == 0x4D42) ; // signature must be 4D42 (ASCII code : BM)
	fileSize = B2U32(header,2);
	dataOffset = B2U32(header,10);
	size = B2U32(info,0);
	width = B2U32(info,4);
	height = B2U32(info,8);
	planes = B2U16(info,12);
	bitsPerPixel = B2U16(info,14);
		assert(bitsPerPixel==24); // BPP must be 24 bits
	compression = B2U32(info,16);
	imageSize = B2U32(info,20);
	xPixelsPerM = B2U32(info,24);
	yPixelsPerM = B2U32(info,28);
	colorsUsed = B2U32(info,32);
	colorsImportant = B2U32(info,36);

	/*----- set f_read pointer to data offset -----*/
	fr = f_lseek(&fil, dataOffset) ;

	/*----- read bmp data -----*/
	xil_printf("start to read bmp data ... \n \r") ;

	/*----- set timer read start -----*/
	XTime_GetTime(&start_r) ;

	/*----- create temporary row register -----*/
	BYTE *tmp ;
	tmp = (BYTE *)malloc((3*width)*sizeof(BYTE)) ;
	*tmp = 0 ;

	/*----- start to read bmp data -----*/
	u32 i1 = 0 ;
	u32 i2 = 0 ;
	xil_printf("complete : ") ;
	for(i1=0; i1<height; ++i1) // image row
	{
		fr = f_read(&fil, tmp, 3*width, &count) ; // read data once the row
		if(fr != FR_OK)
		{
			xil_printf("error while reading : %d !! \n \r", fr) ;
			return main() ;
		}
		for(i2=0; i2<(3*width); ++i2) // image number in the row
		{
			Xil_Out8(BASE+(3*width*i1+i2), tmp[i2]) ; // store the row data to DDR4
		}
		if(i1==0) xil_printf("0%%\b\b\b") ;
		if(i1==(int)(height/4)) xil_printf("25%%\b\b\b") ;
		if(i1==(int)(height/2)) xil_printf("50%%\b\b\b") ;
		if(i1==(int)(3*(height/4))) xil_printf("75%%\b\b\b") ;
		if(i1==(height-1)) xil_printf("100%%\n \n \r") ;
	}
	free(tmp) ;

	/*----- set timer read end -----*/
	XTime_GetTime(&end_r) ;
	xil_printf("loading successful \n \r") ;
	int time_r = (((end_r - start_r)*1000000)/COUNTS_PER_SECOND)/1000 ; // set timer in micro second
	xil_printf("reading take %d ms \n \n \r",time_r) ;

	/*----- print bmp header and info data -----*/
	xil_printf("====================\n \r") ;
	xil_printf("signature = %04x \n \r", signature) ;
	xil_printf("filesize = %ld \n \r", fileSize) ;
	xil_printf("dataoffset = %ld \n \r", dataOffset) ;
	xil_printf("size = %ld \n \r", size) ;
	xil_printf("width = %ld \n \r", width) ;
	xil_printf("height = %ld \n \r", height) ;
	xil_printf("planes = %d \n \r", planes) ;
	xil_printf("bits per pixel = %d \n \r", bitsPerPixel) ;
	xil_printf("compression = %ld \n \r", compression) ;
	xil_printf("image size = %ld \n \r", imageSize) ;
	xil_printf("x resolution = %ld \n \r", xPixelsPerM) ;
	xil_printf("y resolution = %ld \n \r", yPixelsPerM) ;
	xil_printf("colors used = %ld \n \r", colorsUsed) ;
	xil_printf("colors important = %ld \n \r", colorsImportant) ;
	xil_printf("====================\n \n \r") ;

	/*----- close the sd card -----*/
	f_close(&fil) ;

	/*============================== main gpio code start ==============================*/
	xil_printf("program will keep go on in : ") ;
	xil_printf("5 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("4 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("3 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("2 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("1 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("0 seconds \n \r") ;
	sleep(1) ;

	/*----- set gpio input/output -----*/
	XGpio input ;
	XGpio output ;

	/*----- initialize axi gpio (input/output, device ID) -----*/
	XGpio_Initialize(&input, XPAR_AXI_GPIO_0_DEVICE_ID) ;
	XGpio_Initialize(&output, XPAR_AXI_GPIO_0_DEVICE_ID) ;

	/*----- data direction register (input/output, channel, input=1/output=0) -----*/
	XGpio_SetDataDirection(&output, 1, 0) ;
	XGpio_SetDataDirection(&input, 2, 1) ;

	/*----- start IP work -----*/
	xil_printf("start IP processing ... \n \r") ;
	XTime_GetTime(&start_ip) ;

	/*----- create 24 bits data -----*/
	int *data ;
	data = (int*)malloc(1*sizeof(int)) ;
	data = 0 ;

	int cnt = 0 ;
	for(cnt=0; cnt<=(DataSize-3); cnt=cnt+3) // scan the image data number
	{
		data[0] = (Xil_In8(BASE+cnt) | Xil_In8(BASE+cnt+1)<<8 | Xil_In8(BASE+cnt+2)<<16) ; // put DDR4 data of R, G, B in to data
		XGpio_DiscreteWrite(&output, 1, data[0]) ; // put data into GPIO
		data[0] = XGpio_DiscreteRead(&input, 2) ; // through data from GPIO to register
		/*----- set DDR4 data of R, G, B to GRAY value -----*/
		Xil_Out8(BASE+cnt, data[0]) ;
		Xil_Out8(BASE+cnt+1, data[0]) ;
		Xil_Out8(BASE+cnt+2, data[0]) ;
	}

	XTime_GetTime(&end_ip) ;
	xil_printf("IP process successful \n \r") ;
	int time_ip = (((end_ip - start_ip)*1000000)/COUNTS_PER_SECOND)/1000 ; // set timer in micro second
	xil_printf("processing take %d ms \n \n \r",time_ip) ;

	/*============================== main gpio code end ==============================*/

	xil_printf("program will keep go on in : ") ;
	xil_printf("5 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("4 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("3 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("2 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("1 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("0 seconds \n \r") ;
	sleep(1) ;
	/*----- output file -----*/
	char out_f[] = "_o.bmp" ;
	int out_len = strlen(filename)+strlen(out_f)+1 ;
	char *out_filename = malloc(out_len*sizeof(char)) ;
	*out_filename = 0 ;
	strcat(out_filename, filename) ;
	strcat(out_filename, out_f) ;
	xil_printf("output filename : %s \n \n \r", out_filename) ;

	xil_printf("program will keep go on in : ") ;
	xil_printf("5 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("4 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("3 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("2 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("1 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("0 seconds \n \r") ;
	sleep(1) ;

	/*----- open the sd card -----*/
	fr = f_open(&fil, out_filename, FA_CREATE_ALWAYS | FA_WRITE) ;
	if(fr != FR_OK)
	{
		xil_printf("cannot create output bmp file : %d !! \n \r",fr) ;
		return main() ;
	}
	xil_printf("start to write bmp info ... \n \r") ;
	free(out_filename) ;

	/*----- write header and info data -----*/
	fr = f_write(&fil, header, HEADER_SIZE, &count) ;
	fr = f_write(&fil, info, INFO_SIZE, &count) ;

	/*----- set write pointer to data offset -----*/
	fr = f_lseek(&fil, dataOffset) ;
	xil_printf("start to write bmp data ... \n \r") ;

	/*----- set timer write start -----*/
	XTime_GetTime(&start_w) ;

	/*----- create temporary single register -----*/
	BYTE *tmp_o ;
	tmp_o = (BYTE *)malloc((3*width)*sizeof(BYTE)) ;
	*tmp_o = 0 ;

	/*----- start to write bmp data -----*/
	u32 o1 ;
	u32 o2 ;
	xil_printf("complete : ") ;
	for(o1=0; o1<height; ++o1)
	{
		for(o2=0; o2<(3*width); ++o2)
		{
			tmp_o[o2] = Xil_In8(BASE+(3*width*o1+o2)) ;
		}
		fr = f_write(&fil, tmp_o, 3*width, &count) ;
		if(fr != FR_OK)
		{
			xil_printf("error while writing : %d !! \n \r", fr) ;
			return main() ;
		}
		if(o1==0) xil_printf("0%%\b\b\b") ;
		if(o1==(int)(height/4)) xil_printf("25%%\b\b\b") ;
		if(o1==(int)(height/2)) xil_printf("50%%\b\b\b") ;
		if(o1==(int)(3*(height/4))) xil_printf("75%%\b\b\b") ;
		if(o1==(height-1)) xil_printf("100%%\n \n \r") ;
	}
	free(tmp_o) ;

	/*----- set timer write end -----*/
	XTime_GetTime(&end_w) ;
	xil_printf("writing successful \n \r") ;
	int time_w = (((end_w - start_w)*1000000)/COUNTS_PER_SECOND)/1000 ;
	xil_printf("reading take %d ms \n \n \r", time_w) ;

	/*----- close the sd card -----*/
	f_close(&fil) ;

	/*----- free the register -----*/
	free(header) ;
	free(info) ;

	/*----- dismount the sd card -----*/
	f_mount(NULL, "", 0) ;

	/*----- clean the platform -----*/
	xil_printf("the program are finished !! \n \r") ;
	cleanup_platform() ;

	xil_printf("program will keep go on in : ") ;
	xil_printf("5 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("4 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("3 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("2 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("1 seconds\b\b\b\b\b\b\b\b\b") ;
	sleep(1) ;
	xil_printf("0 seconds \n \r") ;
	sleep(1) ;
	return main() ;
}
