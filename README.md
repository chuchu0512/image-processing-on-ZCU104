# image processing ZCU104
* Using Xilinx Design Tools such as Vivado、Vitis and Vitis HLS to do image processing design on Linux or Windows and processing on ZCU104.  

* We use Vivado to create main image processing function (IP) by Verilog, and connect ZYNQMP SOC and IP by AXI-lite bridge, here we use the simple way called AXI-GPIO.  

* If the function is too complicated to code by Verilog, we can use Vitis HLS transform the code from C to Verilog, by this way, we can create the AXI-lite IP without AXI-GPIO.  

* Next, we use Vitis write C code to control the AXI-GPIO or AXI-lite IP by Xilinx functions.  

* Last but not least, we need to read the image (bmp fromat) data from SD card and store the image data, so we also need to do these on Vitis.  

* Also, we boot FPGA by bin file, so we don't need petalinux.

# NOTICE !!
	This  will be completed in October
**此教學會在十月底前更新完成**
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
bmp strcture  
* **header : 14 byte**  
	|start address|name|size(byte)|content|  
	|:-----:|:-----:|:-----:|:-----:|  
	|0x0000|ID|2|bmp file ID|  
	|0x0002|file size|4|total file size|  
	|0x0004|reserved|4|reserved|  
	|0x000A|data offset|4|data offset|  
* **info header : 40 byte**  
	|start address|name|size(byte)|content|  
	|:-----:|:-----:|:-----:|:-----:|  
	|0x000E|header size|4|info header size|  
	|0x0012|width|4|bmp width(pixel)|  
	|0x0016|height|4|bmp height(pixel)|  
	|0x001A|planes|2|bmp plane counts|
	|0x001X|bits per pixel|2|pixel size|
	|0x001E|compression|4|compression method|
	|0x0022|data size|4|bmp data size|
	|0x0026|h resolution|4|horizontal resolution|
	|0x002A|v resolution|4|vertical resolution|
	|0x002E|used colors|4|palette colors used|
	|0x0032|important colors|4|important color count|  
* **data : width * height * 3 byte**
	|address|color|size(byte)|  
	|:-----:|:-----:|:-----:|  
	|offset + 0x0|BLUE|1|  
	|offset + 0x1|GREEN|1|
	|offset + 0x2|RED|1|  
	|offset + 0x3|BLUE|1|
	|offset + 0x4|GREEN|1|
	|offset + 0x5|RED|1|
	|...|...|...|
* **important infomation**
	* ID : `ascii code "BM"`
	* data offset : `to read and write data from offset address`
	* header size : `usually 40 byte`
	* width、height : `1 row of image is width * 3 byte`
	* bits per pixel : `usually 24 bits`
	* data size : `usually width * height * 3 byte`  

FatFs  
* we can use [FatFs website](http://elm-chan.org/fsw/ff/00index_e.html) to check usage of FatFs functions  

## 4.2 using PS-DDR4 to store data
# 5 full design example using Xillinx ZCU104 board
