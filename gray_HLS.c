#include <stdio.h>
/*----- R, G, B is 8 bits length -----*/
int grayscale(unsigned char R, unsigned char G, unsigned char B, float GRAY)
{
	/*----- set input and return AXI port -----*/
	#pragma HLS INTERFACE s_axilite port=R bundle=BUS_AXI
	#pragma HLS INTERFACE s_axilite port=G bundle=BUS_AXI
	#pragma HLS INTERFACE s_axilite port=B bundle=BUS_AXI
	#pragma HLS INTERFACE s_axilite port=GRAY bundle=BUS_AXI

	GRAY = (int)(0.299*R + 0.587*G + 0.114*B) ;

	return 0 ;
}
