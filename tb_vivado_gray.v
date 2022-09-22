`timescale 1ns/1ps

module tb_gray() ;
  reg [23:0] DATA ;
  wire [7:0] GRAY ;
  
  gray gray_m0 (.data(DATA), .gray(GRAY)) ;
  
  initial begin
    DATA[23:0] <= {{8'hff}, {8'h25}, {8'hcd}} ;
    #10
    DATA[23:0] <= {{8'h65}, {8'hd4}, {8'h91}} ;
    #10
    $finish ;
  end ;
endmodule
