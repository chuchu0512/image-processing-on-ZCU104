`timescale 1ns / 1ps

module gray(
input [23:0] data,
output [7:0] gray
    );
    reg [7:0] gray ;
    reg [15:0] r ;
    reg [15:0] g ;
    reg [15:0] b ;
    
    always@(data) begin
        b[15:0] = {8'b0, data[23:16]} ;
        g[15:0] = {8'b0, data[15:8]} ;
        r[15:0] = {8'b0, data[7:0]} ;
        
        gray <= ((77*r + 150*g + 29*b) >> 8) ;    
        end
endmodule


