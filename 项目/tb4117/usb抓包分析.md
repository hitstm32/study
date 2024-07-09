 启动
```bash
# 加载内核模块 usbmon
sudo modprobe usbmon

# 添加用户组usbmon
sudo addgroup usbmon

# 添加当前用户到组
sudo gpasswd -a $USER usbmon

# 写入规则文件
sudo vi /etc/udev/rules.d/99-usbmon.rules
SUBSYSTEM=="usbmon", GROUP="usbmon", MODE="640"
```
查看USB地址：
```bash
lsusb

# 返回信息（003 总线地址 010 设备ID）
Bus 003 Device 030: ID 2bdf:0101 
# 所以，可以得到摄像头地址是3:30:x，摄像头包含多个端点，可以以实现控制、数据流传输等功能
```
过滤
````bash
(usb.src == "3.9.1" and usb.dst =="host") || (usb.src == "host" && usb.dst == "3.9.2")
````


## 端点：
> CONTROL
>控制端点被用来允许对 USB 设备的不同部分存取. 通常用作配置设备, 获取关于设备的信息, 发送命令到设备, 或者获取关于设备的状态报告. 这些端点在尺寸上常常较小. 每个 USB 设备有一个控制端点称为"端点 0", 被 USB 核用来在插入时配置设备. 这些传送由 USB 协议保证来总有足够的带宽使它到达设备.
>INTERRUPT
>[中断](https://so.csdn.net/so/search?q=%E4%B8%AD%E6%96%AD&spm=1001.2101.3001.7020)端点传送小量的数据, 以固定的速率在每次 USB 主请求设备数据时. 这些端点对 USB 键盘和鼠标来说是主要的传送方法. 它们还用来传送数据到 USB 设备来控制设备, 但通常不用来传送大量数据. 这些传送由 USB 协议保证来总有足够的带宽使它到达设备.
>BULK
>块端点传送大量的数据. 这些端点常常比中断端点大(它们一次可持有更多的字符). 它们是普遍的, 对于需要传送不能有任何数据丢失的数据. 这些传送不被 USB 协议保证来一直使它在特定时间范围内完成. 如果总线上没有足够的空间来发送整个 BULK 报文, 它被分为多次传送到或者从设备. 这些端点普遍在打印机, 存储器, 和网络设备上.
>ISOCHRONOUS
>同步端点也传送大量数据, 但是这个数据常常不被保证它完成. 这些端点用在可以处理数据丢失的设备中, 并且更多依赖于保持持续的数据流. 实时数据收集, 例如音频和视频设备, 一直都使用这些端点.


# 实测数据：

## F01218820（自用）：
`Bus 003 Device 009: ID 2bdf:0101`
* USB_Init()
	* 啥都没传，应该是初始化了本机的usb
* CUSTOM_GET_DEVICE_COUNT()
	* 获取一堆字符串，都是些HIK，HikCamera之类的
	* 获取和设置了一些东西，获取到的数据和设置的数据一模一样
	* ![[Pasted image 20240621210513.png]]
	* ![[Pasted image 20240621210541.png]]
	* ![[Pasted image 20240621210551.png]]
	* ![[Pasted image 20240621210603.png]]
	* ![[Pasted image 20240621210625.png]]
	* ![[Pasted image 20240621210633.png]]
* CUSTOM_ENUM_DEVICE()
	* 没收发任何东西，SDK返回设备VID和PID
* CUSTOM_LOGIN_DEVICE()
	* 又是一堆字符串获取，和CUSTOM_GET_DEVICE_COUNT一样
	* ![[Pasted image 20240621211319.png]]
	* ![[Pasted image 20240621211326.png]]
	* ![[Pasted image 20240621211335.png]]
	* ![[Pasted image 20240621211344.png]]
	* ![[Pasted image 20240621211352.png]]
* THERMAL_GET_IMAGE_ENHANCEMENT() //设置伪彩之类的
	* ![[Pasted image 20240621212640.png]]
	* ![[Pasted image 20240621212720.png]]
	* ![[Pasted image 20240621212728.png]]
	* ![[Pasted image 20240621212736.png]]
	* ![[Pasted image 20240621212753.png]]
	* ![[Pasted image 20240621212802.png]]
	* ![[Pasted image 20240621212812.png]]
	* ![[Pasted image 20240621212823.png]]
	* ![[Pasted image 20240621212834.png]]
	* 数据完全依据![[Pasted image 20240621213151.png]]
* THERMAL_SET_IMAGE_ENHANCEMENT() //设置伪彩等
	* ![[Pasted image 20240621213241.png]]
	* ![[Pasted image 20240621213250.png]]
	* ![[Pasted image 20240621213256.png]]
	* ![[Pasted image 20240621213304.png]]
	* 直接把部分结构体数据发出去了，没啥东西。
* THERMAL_GET_IMAGE_VIDEO_ADJUST()  //调方向
	* ![[Pasted image 20240621224819.png]]
	* ![[Pasted image 20240621224829.png]]
	* ![[Pasted image 20240621224838.png]]
	* ![[Pasted image 20240621224846.png]]
	* ![[Pasted image 20240621224855.png]]
	* ![[Pasted image 20240621224903.png]]
	* ![[Pasted image 20240621224911.png]]
	* ![[Pasted image 20240621224920.png]]
	* ![[Pasted image 20240621224926.png]]
	* ![[Pasted image 20240621224945.png]]
* THEhexRMAL_SET_IMAGE_VIDEO_ADJUST()
	* ![[Pasted image 20240621225027.png]]
	* ![[Pasted image 20240621225036.png]]
	* ![[Pasted image 20240621225047.png]]
	* ![[Pasted image 20240621225057.png]]
* 