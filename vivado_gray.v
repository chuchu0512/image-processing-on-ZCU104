'timescale 1ns/1ps

module gray(
  input [23:0] data, // input R, G, B each 8bits.
  output [7:0] gray // output Grayscale 8 bits.
) ;
  
  reg [7:0] gray ;
  reg [15:0] r ;
  reg [15:0] g ;
  reg [15:0] b ;
  
  always@(data) begin
    b[15:0] = {8'b0, data[23:16]} ;
    g[15:0] = {8'b0, data[15:8]} ;
    r[15:0] = {8'b0, data[7:0]} ;
    // bmp data start from blue to red
    gray <= ((77*r + 150*g + 29*b) >> 8) ;
  end
endmodule
