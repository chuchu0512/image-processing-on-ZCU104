#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xgrayscale.h"
#include "xil_printf.h"

XGrayscale do_gray ; // X.... 依照自己的IP名字
XGrayscale_Config *do_gray_config ;

void init_hls_grayscale() //functions are in xgrayscale.h
{
	int status ;

	// create HLS IP pointer

	do_gray_config = XGrayscale_LookupConfig(XPAR_XGRAYSCALE_0_DEVICE_ID) ; // ID 在 xparameters.h 裡面

	if (!do_gray_config)
	{
		xil_printf("Error loading configuration for do_gray_config \n \r") ;
	}

	status = XGrayscale_CfgInitialize(&do_gray, do_gray_config) ;

	if(status != XST_SUCCESS)
	{
		xil_printf("Error initializing for do_gray \n \r") ;
	}
	XGrayscale_Initialize(&do_gray, XPAR_XGRAYSCALE_0_DEVICE_ID) ;
}

void grayscale(int r, int g, int b) // r, g, b 為 int 但硬體內只有 8 bits
{
	int gray = 0 ;

	// write inputs

	XGrayscale_Set_R(&do_gray, r) ;
	XGrayscale_Set_G(&do_gray, g) ;
	XGrayscale_Set_B(&do_gray, b) ;
	xil_printf("R = %d \n \r", r) ;
	xil_printf("G = %d \n \r", g) ;
	xil_printf("B = %d \n \r", b) ;

	//start IP
	XGrayscale_Start(&do_gray) ;
	xil_printf("start grayscale IP \n \r") ;
	// wait
	while(!XGrayscale_IsDone(&do_gray)) ;

	//get output value

	gray = XGrayscale_Get_return(&do_gray) ;

	xil_printf("gray value = %d \n \r", gray) ;
	xil_printf("end of grayscale \n \r") ;

}

int main()
{
	//set up platform
	init_platform() ;
	init_hls_grayscale() ;

	int red = 0 ;
	int green = 0 ;
	int blue = 0 ;

	for(;;)
	{
		//enter input
		xil_printf("input value of R, G, B \n \r") ;
		scanf("%d", &red) ;
		xil_printf("%d ", red) ;
		scanf("%d", &green) ;
		xil_printf("%d ", green) ;
		scanf("%d", &blue) ;
		xil_printf("%d \n \r", blue) ;
		// start IP

		grayscale(red, green, blue) ;
	}
	cleanup_platform() ;
	return main() ;
}
