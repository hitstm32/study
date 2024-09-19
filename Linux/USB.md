 https://www.usbzh.com/nav/

USB设备被发现的过程是通过USB D+或者D-上的电平跳变，USB host端D+D-有15K下拉电阻，USB device内部D+或者D-有1.5k上拉电阻，当USB设备插入，D+或者D-会被拉高，由此host端实现了设备的检测。D+拉高为全速设备（12Mbps），全拉高为低速设备，高速设备的硬件和全速设备一样，但是软件上加以区分，识别到全速设备后先复位，然后通过协议询问是否支持高速，如果支持，则转换到高速模式（480M）
## USB状态切换图
![[Pasted image 20240617163851.png]]
![[Pasted image 20240617165927.png]]
## USB传输逻辑：
![[Pasted image 20240617171256.png]]
IDLE->SOP->同步脉冲串->数据负载->EOP->IDLE
**传输使用反相不归零码(NRZI)+位填充**
## 软件层面数据传输：
### 事务：
![[Pasted image 20240617171525.png]]
如果要访问某个设备，肯定要确定设备的地址和端点。所有数据传输都是host发起，还需要确定数据传输方向。
![[Pasted image 20240617171756.png]]
PID中含有方向、包类型。
![[Pasted image 20240617172119.png]]
PID域有8位组成，实际上有效位数为4，分为4大类包
![[Pasted image 20240617172324.png]]
这里的输入和输出都是相对host。
地址有7位，端点4位
![[Pasted image 20240617172625.png]]
![[Pasted image 20240617172629.png]]

**如果想发数据，就要先发个令牌包(token phase)，然后再发数据包（data phase），数据包里才包含数据本身。收到数据后，dev可以回复握手(ACK)包(hand phase)，这三种包组成了一种<span style="color:#c00000">事务</span>，事务是一次传输的完整流程**
**如果想读数据，先发起读令牌包，然后设备端发送数据包，最后host发送握手包完成传输**
事务由包组成，包由域组成，域由位组成。

### 传输：
![[Pasted image 20240617174558.png]]
* 批量传输：可靠，非实时
* 中断传输：可靠，实时
* 实时传输：不可靠，实时
所有传输都是由host发起，即使dev有数据，也不能主动发给host，这就导致所谓的“中断传输”并不是真正的中断，只是周期性的发起传输，尽量做到实时。
![[Pasted image 20240617182844.png]]
一个bulk， int， iso分别由1个事务实现。一个control传输由多个事务实现。
#### 批量传输（事务）bulk
![[Pasted image 20240617183216.png]]
如果要进行大量传输，直接简单重复以上过程即可，只不过发data包时要不断切换PID中的DATA0和DATA1。
#### 中断传输
![[Pasted image 20240617184704.png]]
跟批量传输区别不大，少了ping和一个握手pid。
#### iso（实时传输）
![[Pasted image 20240617185122.png]]
不用发握手包了，更快。
#### 控制传输（Transfer）
![[Pasted image 20240618114112.png]]
![[Pasted image 20240617185333.png]]
控制传输由多个事务组成，首先发起一个setup事务，然后再发起data事务，然后状态事务。
* 建立事务：由三个包组成：
	* 设置令牌包：通知设备要开始一个控制传输，无方向。
	* 数据包：数据包里包含收发双方约定好的格式化数据，例如收还是发之类的
	* 握手包：响应
* 数据阶段（由一个或者多个批量(bulk)事务构成）：
	* 令牌包：包含地址，EP等
	* 数据包data0
	* 数据包data1
	* ......
	* 最后一个数据包
	* 握手包
* 状态阶段
	* 令牌包
	* 数据包（无数据，有检验码）
		* ![[Pasted image 20240618113712.png]]控制写状态
		* 
	* 握手包

实际传输过程：
![[Pasted image 20240618161645.png]]
## USB描述符
设备/配置/接口/端点
在setup事务的数据包中，表示了要访问什么，dev? interface? endpoint?
对于一种usb设备，可以有多种配置。例如一个usb网卡，第一次连接时可以是一个u盘，u盘里有驱动，可以用于安装驱动，后面再连接时变成usb网卡。
usb耳机有多种功能，有声音和按键，两种功能同时起效，这实际上是两个“接口(interface)”
**每个设备可以有多种配置，每个配置可以有多个接口,每个接口可以有不同的逻辑功能，比如xilinx下载器还带个uart口，就是这个原理**
**每个设备都会有端点0，这个端点是双向的，用来配置设备**
![[Pasted image 20240618195059.png]]
每个设备都会有一个设备描述符，每种配置也有自己的配置描述符，配置描述符下各个接口有自己的接口描述符，端点有自己的端点描述符。
描述符有自己的格式，例如设备描述符有18字节，通过设备描述符可以区分出不同设备。
一个USB设备，拥有：
* 一个设<span style="color:#c00000">备描述符</span>：传输设备ID，有多少配置描述符，端点0一次能传多少字节数据。
* 多个<span style="color:#c00000">配置描述符</span>：表示有多少接口，供电方式，最大电流
* 一个配置描述符下多个<span style="color:#c00000">接口描述符</span>：哪类接口、有几个setting、几个端点
* 一个接口描述符下有多个<span style="color:#c00000">端点描述符</span>：端点号、方向、类型（批量/中断/同步）

`lsusb -v`：
```bash
Bus 003 Device 032: ID 2bdf:0101  
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               2.00
  bDeviceClass          239 Miscellaneous Device
  bDeviceSubClass         2 ?
  bDeviceProtocol         1 Interface Association
  bMaxPacketSize0        64
  idVendor           0x2bdf 
  idProduct          0x0101 
  bcdDevice            4.09
  iManufacturer           1 HIK
  iProduct                2 HikCamera
  iSerial                 3 F01218820
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength          433
    bNumInterfaces          2
    bConfigurationValue     1
    iConfiguration          4 Config 1
    bmAttributes         0xc0
      Self Powered
    MaxPower                2mA
    Interface Association:
      bLength                 8
      bDescriptorType        11
      bFirstInterface         0
      bInterfaceCount         2
      bFunctionClass         14 Video
      bFunctionSubClass       3 Video Interface Collection
      bFunctionProtocol       0 
      iFunction               5 UVC Camera
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        0
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass        14 Video
      bInterfaceSubClass      1 Video Control
      bInterfaceProtocol      0 
      iInterface              5 UVC Camera
      VideoControl Interface Descriptor:
        bLength                13
        bDescriptorType        36
        bDescriptorSubtype      1 (HEADER)
        bcdUVC               1.10
        wTotalLength           81
        dwClockFrequency       48.000000MHz
        bInCollection           1
        baInterfaceNr( 0)       1
      VideoControl Interface Descriptor:
        bLength                18
        bDescriptorType        36
        bDescriptorSubtype      2 (INPUT_TERMINAL)
        bTerminalID             2
        wTerminalType      0x0201 Camera Sensor
        bAssocTerminal          0
        iTerminal               0 
        wObjectiveFocalLengthMin      0
        wObjectiveFocalLengthMax      0
        wOcularFocalLength            0
        bControlSize                  3
        bmControls           0x00000000
      VideoControl Interface Descriptor:
        bLength                12
        bDescriptorType        36
        bDescriptorSubtype      5 (PROCESSING_UNIT)
        bUnitID                 5
        bSourceID               1
        wMaxMultiplier      16384
        bControlSize            2
        bmControls     0x00000000
        iProcessing             0 
        bmVideoStandards     0x 9
          None
          SECAM - 625/50
      VideoControl Interface Descriptor:
        bLength                29
        bDescriptorType        36
        bDescriptorSubtype      6 (EXTENSION_UNIT)
        bUnitID                10
        guidExtensionCode         {41769ea2-04de-e347-8b2b-f4341aff003b}
        bNumControl            15
        bNrPins                 1
        baSourceID( 0)          2
        bControlSize            4
        bmControls( 0)       0xff
        bmControls( 1)       0x03
        bmControls( 2)       0x00
        bmControls( 3)       0x00
        iExtension              0 
      VideoControl Interface Descriptor:
        bLength                 9
        bDescriptorType        36
        bDescriptorSubtype      3 (OUTPUT_TERMINAL)
        bTerminalID             3
        wTerminalType      0x0101 USB Streaming
        bAssocTerminal          0
        bSourceID               2
        iTerminal               0 
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x83  EP 3 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0010  1x 16 bytes
        bInterval               8
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        1
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass        14 Video
      bInterfaceSubClass      2 Video Streaming
      bInterfaceProtocol      0 
      iInterface              6 Video Streaming
      VideoStreaming Interface Descriptor:
        bLength                            16
        bDescriptorType                    36
        bDescriptorSubtype                  1 (INPUT_HEADER)
        bNumFormats                         3
        wTotalLength                      298
        bEndPointAddress                  129
        bmInfo                              0
        bTerminalLink                       3
        bStillCaptureMethod                 0
        bTriggerSupport                     0
        bTriggerUsage                       0
        bControlSize                        1
        bmaControls( 0)                    27
        bmaControls( 1)                    27
        bmaControls( 2)                    27
      VideoStreaming Interface Descriptor:
        bLength                            27
        bDescriptorType                    36
        bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
        bFormatIndex                        1
        bNumFrameDescriptors                3
        guidFormat                            {59555932-0000-1000-8000-00aa00389b71}
        bBitsPerPixel                      16
        bDefaultFrameIndex                  1
        bAspectRatioX                       0
        bAspectRatioY                       0
        bmInterlaceFlags                 0x00
          Interlaced stream or variable: No
          Fields per frame: 2 fields
          Field 1 first: No
          Field pattern: Field 1 only
          bCopyProtect                      0
      VideoStreaming Interface Descriptor:
        bLength                            30
        bDescriptorType                    36
        bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
        bFrameIndex                         1
        bmCapabilities                   0x00
          Still image unsupported
        wWidth                            120
        wHeight                           160
        dwMinBitRate                  7680000
        dwMaxBitRate                  7680000
        dwMaxVideoFrameBufferSize       38400
        dwDefaultFrameInterval         400000
        bFrameIntervalType                  1
        dwFrameInterval( 0)            400000
      VideoStreaming Interface Descriptor:
        bLength                            30
        bDescriptorType                    36
        bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
        bFrameIndex                         2
        bmCapabilities                   0x00
          Still image unsupported
        wWidth                            240
        wHeight                           320
        dwMinBitRate                 36864000
        dwMaxBitRate                 36864000
        dwMaxVideoFrameBufferSize      153600
        dwDefaultFrameInterval         333333
        bFrameIntervalType                  1
        dwFrameInterval( 0)            333333
      VideoStreaming Interface Descriptor:
        bLength                            30
        bDescriptorType                    36
        bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
        bFrameIndex                         3
        bmCapabilities                   0x00
          Still image unsupported
        wWidth                            640
        wHeight                           360
        dwMinBitRate                110592000
        dwMaxBitRate                110592000
        dwMaxVideoFrameBufferSize      460800
        dwDefaultFrameInterval         333333
        bFrameIntervalType                  1
        dwFrameInterval( 0)            333333
      VideoStreaming Interface Descriptor:
        bLength                            11
        bDescriptorType                    36
        bDescriptorSubtype                  6 (FORMAT_MJPEG)
        bFormatIndex                        2
        bNumFrameDescriptors                3
        bFlags                              0
          Fixed-size samples: No
        bDefaultFrameIndex                  1
        bAspectRatioX                       0
        bAspectRatioY                       0
        bmInterlaceFlags                 0x00
          Interlaced stream or variable: No
          Fields per frame: 1 fields
          Field 1 first: No
          Field pattern: Field 1 only
          bCopyProtect                      0
      VideoStreaming Interface Descriptor:
        bLength                            30
        bDescriptorType                    36
        bDescriptorSubtype                  7 (FRAME_MJPEG)
        bFrameIndex                         1
        bmCapabilities                   0x00
          Still image unsupported
        wWidth                            120
        wHeight                           160
        dwMinBitRate                  7680000
        dwMaxBitRate                  7680000
        dwMaxVideoFrameBufferSize       38400
        dwDefaultFrameInterval         400000
        bFrameIntervalType                  1
        dwFrameInterval( 0)            400000
      VideoStreaming Interface Descriptor:
        bLength                            30
        bDescriptorType                    36
        bDescriptorSubtype                  7 (FRAME_MJPEG)
        bFrameIndex                         2
        bmCapabilities                   0x00
          Still image unsupported
        wWidth                            240
        wHeight                           320
        dwMinBitRate                 10240000
        dwMaxBitRate                 10240000
        dwMaxVideoFrameBufferSize      153600
        dwDefaultFrameInterval         333333
        bFrameIntervalType                  1
        dwFrameInterval( 0)            333333
      VideoStreaming Interface Descriptor:
        bLength                            30
        bDescriptorType                    36
        bDescriptorSubtype                  7 (FRAME_MJPEG)
        bFrameIndex                         3
        bmCapabilities                   0x00
          Still image unsupported
        wWidth                            640
        wHeight                           360
        dwMinBitRate                 10240000
        dwMaxBitRate                 10240000
        dwMaxVideoFrameBufferSize      460800
        dwDefaultFrameInterval         333333
        bFrameIntervalType                  1
        dwFrameInterval( 0)            333333
      VideoStreaming Interface Descriptor:
        bLength                            28
        bDescriptorType                    36
        bDescriptorSubtype                 16 (FORMAT_FRAME_BASED)
        bFormatIndex                        3
        bNumFrameDescriptors                1
        guidFormat                            {48323634-0000-1000-8000-00aa00389b71}
        bBitsPerPixel                      16
        bDefaultFrameIndex                  1
        bAspectRatioX                       0
        bAspectRatioY                       0
        bmInterlaceFlags                 0x00
          Interlaced stream or variable: No
          Fields per frame: 2 fields
          Field 1 first: No
          Field pattern: Field 1 only
          bCopyProtect                      0
          bVariableSize                     1
      VideoStreaming Interface Descriptor:
        bLength                            30
        bDescriptorType                    36
        bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
        bFrameIndex                         1
        bmCapabilities                   0x00
          Still image unsupported
        wWidth                            240
        wHeight                           320
        dwMinBitRate                  8192000
        dwMaxBitRate                  8192000
        dwDefaultFrameInterval         333333
        bFrameIntervalType                  1
        dwBytesPerLine                      0
        dwFrameInterval( 0)            333333
      VideoStreaming Interface Descriptor:
        bLength                             6
        bDescriptorType                    36
        bDescriptorSubtype                 13 (COLORFORMAT)
        bColorPrimaries                     1 (BT.709,sRGB)
        bTransferCharacteristics            1 (BT.709)
        bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x81  EP 1 IN
        bmAttributes            2
          Transfer Type            Bulk
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0200  1x 512 bytes
        bInterval               0
Device Qualifier (for other device speed):
  bLength                10
  bDescriptorType         6
  bcdUSB               2.00
  bDeviceClass          239 Miscellaneous Device
  bDeviceSubClass         2 ?
  bDeviceProtocol         1 Interface Association
  bMaxPacketSize0        64
  bNumConfigurations      1
Device Status:     0x0001
  Self Powered

```
以上是海康威视热成像的lsusb信息，可见，其有一个配置描述符，两个接口描述符，每个接口下有两个端点，加上每个设备都有的0号端点，共占<span style="color:#c00000">三个</span>端点。
## USB设备枚举过程
![[Pasted image 20240618204748.png]]
USB枚举过程中，通过控制传输（control transfer）来实现设备枚举，控制传输分为三个阶段。建立阶段、数据阶段、状态阶段。host发起的控制传输中，在建立阶段的数据(data0，8bytes)中对USB设备描述符进行请求，请求有固定格式。
![[Pasted image 20240618205316.png]]
1字节：要干啥，比如读描述符或者设置描述符之类：
![[Pasted image 20240618211521.png]]
 ![[Pasted image 20240618212231.png]]
 可见过程为：
 * 获得设备描述符
 * 分配地址后，设置地址，使用新地址访问设备
 * 再次获得设备描述符
 * 获得配置描述符
 * 设置配置，使设备工作于某个配置下
## libusb使用
libusb用来实现用户态的驱动程序，降低使用usb的复杂度。
libusb的接口分为同步和异步
* 同步：阻塞，启动传输和等待结果在一个函数里
* 异步：非阻塞，启动传输和等待结果分开
### 读取鼠标数据
#### HID协议
从接口描述符中，确定它是usb鼠标，就可以读取鼠标数据。
```bash
Bus 003 Device 004: ID 0e0f:0003 VMware, Inc. Virtual Mouse
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               1.10
  bDeviceClass            0 (Defined at Interface level)
  bDeviceSubClass         0 
  bDeviceProtocol         0 
  bMaxPacketSize0         8
  idVendor           0x0e0f VMware, Inc.
  idProduct          0x0003 Virtual Mouse
  bcdDevice            1.03
  iManufacturer           1 VMware
  iProduct                2 VMware Virtual USB Mouse
  iSerial                 0 
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength           34
    bNumInterfaces          1
    bConfigurationValue     1
    iConfiguration          1 VMware
    bmAttributes         0xc0
      Self Powered
    MaxPower                0mA
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        0
      bAlternateSetting       0
      bNumEndpoints           1
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      1 Boot Interface Subclass
      bInterfaceProtocol      2 Mouse
      iInterface              1 VMware
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.10
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 Report
          wDescriptorLength      46
         Report Descriptors: 
           ** UNAVAILABLE **
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x81  EP 1 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0008  1x 8 bytes
        bInterval               1
Device Status:     0x0001
  Self Powered
```
可见，从接口描述符中，可以看到bInterfaceClass为HID，且bInterfaceProtocol为鼠标。Boot Interface Subclass的意思是，在boot阶段(win的bios阶段)就可以识别出鼠标。
* 端点0：控制，双向传输
* 端点x：中断传输，输入（相对host）
发起一个中断传输，即可读取数据。
![[Pasted image 20240620074646.png]]
# USB设备驱动模型

分配urb，填充urb