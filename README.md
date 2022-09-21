# image processing ZCU104
Using Xilinx Design Tools such as Vivado、Vitis and Vitis HLS to do image processing design on Linux or Windows and processing on ZCU104.  

We use Vivado to create main image processing function (IP) by Verilog, and connect ZYNQMP SOC and IP by AXI-lite bridge, here we use the simple way called AXI-GPIO.  

If the function is too complicated to code by Verilog, we can use Vitis HLS transform the code from C to Verilog, by this way, we can create the AXI-lite IP without AXI-GPIO.  

Next, we use Vitis write C code to control the AXI-GPIO or AXI-lite IP by Xilinx functions.  

Last but not least, we need to read the image (bmp fromat) data from SD card and store the image data, so we also need to do these on Vitis.  

Also, we boot FPGA by bin file, so we don't need petalinux.

# NOTICE !!
	This  will be completed in September
**此教學會在九月底前更新完成**
- [ ] chapter 1  
- [ ] chapter 2  
- [ ] chapter 3  
- [ ] chapter 4  
- [ ] chapter 5
# contents
> 1. install Xilinx Design Tools on Linux or Windows
> 2. Vivado part  
	2.1. create own verilog IP  
	2.2. connect SOC and IP using AXI-GPIO  
	2.3. control the AXI-GPIO on Vitis
> 3. Vitis HLS part  
	3.1. create own C code AXI-IP  
	3.2. connect SOC and AXI-IP  
	3.3. control the IP on Vitis
> 4. Vitis part  
	4.1. read bmp file data from SD card  
	4.2. using PS-DDR4 to store data
> 5. full design example using Xilinx ZCU104 board
# 1 install Xilinx Design Tools on Linux or Windows
# 2 Vivado part
## 2.1 create own verilog IP
## 2.2 connect SOC and IP using AXI-GPIO
## 2.3 control the AXI-GPIO on Vitis
# 3 Vitis HLS part
## 3.1 create own C code AXI-IP
## 3.2 connect the SOC and AXI-IP
## 3.3 control the IP on Vitis
# 4 Vitis part
## 4.1 read bmp file data from SD card
## 4.2 using PS-DDR4 to store data
# 5 full design example using Xillinx ZCU104 board
