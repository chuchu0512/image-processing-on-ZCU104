# image processing on ZCU104
* Using Xilinx Design Tools such as Vivado、Vitis and Vitis HLS to do image processing design on Linux or Windows and processing on ZCU104.  

* We use Vivado to create main image processing function (IP) by Verilog, and connect ZYNQMP SOC and IP by AXI-lite bridge, here we use the simple way called AXI-GPIO.  

* If the function is too complicated to code by Verilog, we can use Vitis HLS transform the code from C to Verilog, by this way, we can create the AXI-lite IP without AXI-GPIO.  

* Next, we use Vitis write C code to control the AXI-GPIO or AXI-lite IP by Xilinx functions.  

* Last but not least, we need to read the image (bmp fromat) data from SD card and store the image data, so we also need to do these on Vitis.  

* Also, we boot FPGA by bin file, so we don't need petalinux.

# NOTICE !!
	This  will be completed in October
**此教學會在十月底前更新完成**
- [x] chapter 1  
- [x] chapter 2  
- [ ] chapter 3  
- [ ] chapter 4  
- [ ] chapter 5
# contents
> 1. install Xilinx Design Tools on Linux or Windows  
> 2. Vivado part  
	2.1. create own verilog IP  
	2.2. connect SOC and IP using AXI-GPIO  
	2.3. control the AXI-GPIO on Vitis  
	2.4. launch the program to FPGA  
> 3. Vitis HLS part  
	3.1. create own C code AXI-IP  
	3.2. connect SOC and AXI-IP  
	3.3. control the IP on Vitis
> 4. Vitis part  
	4.1. read bmp file data from SD card  
	4.2. using PS-DDR4 to store data
> 5. full design example using Xilinx ZCU104 board
# 1 install Xilinx Design Tools on Linux or Windows
* Go to [Xilinx website](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools.html) download the installer on Linux or Windows and start install.  
	* Notice on Linux : you should install `libncurses5-dev` and `libncurses5` before install Xilinx Design Tools.  
* Download [2022 patch](https://support.xilinx.com/s/article/76960?language=en_US) into Xilinx path, follow `README` file to install.  
* Open Vivado, enter `Tools/Setting/Text Editor`, find `Syntax Checking`, change `Sigasi` to `Vivado`. You should restart Vivado before next project.  
# 2 Vivado part
## 2.1 create own verilog IP
* Create new Project
* Choose RTL Project
	- [x] Do not specify sources at this time  
	- [ ] Project is an extensible Vitis platform  
* find ZCU104 board and choose  
* Copy `gray.v` and `tb_gray.v` into project  
* Set tb file to top run debug and set function file to top run synthesis check errors  
* Enter `Tools/Create and Package New IP`, choose Package your current project and export to the path you want to store  
## 2.2 connect SOC and IP using AXI-GPIO
* Create new Project  
* Enter `Tools/Settings/IP/Repository`, add the IP you create in 2.1  
* click `Create Block Design` on the left  
* Add `Zynq UltraScale+ MPSoC` and `click Run Block Automation`  
* Dobble click enter the IP. set `PS-PL Configuration/PS-PL Interface/Master Interface`use only `AXI HPM0 LPD`  
	- [x] Enable DDR Controller
	- [x] SD 1
	- [x] UART 0 and UART 1  
* Add AXI-GPIO, set GPIO channel 1 `All Inputs` and channel 2 `All outputs` 32bits  
* Add own IP, connect GPIO port to data, GPIO2 port to gray  
![](https://i.imgur.com/KvxdGvf.png)  
* Run connection Automation and choose All Automation and you will get the block diagram 
![](https://i.imgur.com/Nec4hjA.png)  
* Press `F6` to valid design, next `Create HDL Wrapper`, after that `Generate Bitstream`(it takes time)  
	* if GPIO has different width with IP, it will get warring, you can ignore it  
* Enter `File/Export/Export Hardware`, choose `include bitstream`, and you will get the `xsa` file  
## 2.3 control the AXI-GPIO on Vitis
* create workplace path, copy the xsa file into the path  
* Open Vitis and `Create Platform Project` in the workplace path. Named platform project name same as xsa file name, choose your xsa file as `Hardware Specification` and default other selection  
* Enter `psu_cortex53_0/standalone on psu_cortex53_0/Board Support Package(BSP)`, find `axi_gpio_0` click `Import Example` and create xgpio_example  
* Follow the `Vitis_GPIO.c` code to use functions  
* Build the `project` to check error.
## 2.4 launch the program to FPGA
* There is 2 ways to launch FPGA  
	First way :  
	* We connect USB to local PC and FPGA, and open Serial port moniter(ilke `PuTTY` or `Vitis Serial`)  
	* Turn `SW6[4:1] as [ON ON ON ON]` and turn power on  
	* right click project and find `Run As/Launch Hardware`  

	Second way :
	* build the `system` and you can find the `bin file` in `Debug/sd_card`  
	* `COPY` the bin file into SD card  
	* Also connect USB to local PC and FPGA, and open Serial port moniter  
	* Turn `SW6[4:1] as [OFF OFF OFF ON]` and turn power on  
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
