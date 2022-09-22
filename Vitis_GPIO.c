
#include "xparameters.h" // 包含 GPIO 基地址，device ID。
#include "xgpio.h" // 檔案名稱根據 IP 名稱不同改變，此範例調用 GPIO 範例，因此叫 xgpio.h。
#include "xil_printf.h" // 使用 xil_printf("")，比較不占用內存，結尾必須加上 \r 使打印回到第一列。

#include "xil_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <assert.h>

int main()
{
	// 以下指令皆可以在 xgpio.h 找到。

	//set gpio input/output
	XGpio input ;
	XGpio output ;

	//initialize axi gpio (input/output, device ID)
	XGpio_Initialize(&input, XPAR_AXI_GPIO_0_DEVICE_ID) ;
	XGpio_Initialize(&output, XPAR_AXI_GPIO_0_DEVICE_ID) ;
	//make sure the device ID matches the instance (input/output)

	//Data Direction Reg (input/output, channel, input 1/output 0)
	XGpio_SetDataDirection(&output, 1, 0) ;
	XGpio_SetDataDirection(&input, 2, 1) ;

	//main code

	/***** set input vector *****/
	unsigned char *data ;
	data = (unsigned char*)malloc(27&sizeof(unsigned char)) ;
	*data = 0 ;
	/***** set output vector *****/
	int *gray ;
	gray = (int*)malloc(9*sizeof(int)) ;
	*gray = 0 ;

	//set input register value
	xil_printf("input data : \n \r") ;
	data[0] = 80 ; data[1] = 120 ; data[2] = 160 ; data[3] = 250 ; data[4] = 45 ; data[5] = 200 ; data[6] = 160 ; data[7] = 100 ; data[8] = 200 ;
	data[9] = 20 ; data[10] = 40 ; data[11] = 80 ; data[12] = 90 ; data[13] = 180 ; data[14] = 250 ; data[15] = 10 ; data[16] = 100 ; data[17] = 200 ;
	data[18] = 40 ; data[19] = 250 ; data[20] = 0 ; data[21] = 0 ; data[22] = 0 ; data[23] = 100 ; data[24] = 40 ; data[25] = 45 ; data[26] = 40 ;
	//print input value
	int j = 0 ;
	for(j=0; j<3; ++j)
	{
		xil_printf("%3d %3d %3d, %3d %3d %3d, %3d %3d %3d, ", data[9*j+0], data[9*j+1], data[9*j+2], data[9*j+3], data[9*j+4], data[9*j+5], data[9*j+6], data[9*j+7], data[9*j+8]) ;
		xil_printf("\n \r") ;
	}
	/***** main code *****/

	xil_printf("start processing ... \n \r") ;
	int *data_in ;
	data_in = (int*)malloc(sizeof(int)) ;
	*data_in = 0 ;
	int q = 0 ;
	int q2 = 0 ;
	for(q=0; q<27; q=q+3)
	{
		data_in[q2] = data[q] + data[q+1]*256 + data[q+2]*65536 ;
		++q2 ;
	}

	for(;;)
	{
		// data is type "int" (32 bits)
		int cnt = 0 ;
		int k = 0 ;
		for(cnt=0; cnt<27; cnt=cnt+3)
		{
			//data = (input, channel)
			XGpio_DiscreteWrite(&output, 1, data_in[k]) ;
			//(output, channel, data)
			gray[k] = XGpio_DiscreteRead(&input, 2) ;

			xil_printf("R, G, B = %d, %d, %d \n \r", data[cnt], data[cnt+1], data[cnt+2]) ;
			xil_printf("gray value = %d\n \r", gray[k]) ;
			++k ;
		}
		xil_printf("end grayscale processing ... \n \r") ;
		break ;
	}
	xil_printf("gray data register : \n \r") ;
	xil_printf("%3d, %3d, %3d \n \r", gray[0], gray[1], gray[2]) ;
	xil_printf("%3d, %3d, %3d \n \r", gray[3], gray[4], gray[5]) ;
	xil_printf("%3d, %3d, %3d \n \r", gray[6], gray[7], gray[8]) ;
	return 0 ;
}
