linux自带的UVC驱动，位于![[Pasted image 20240703104310.png]]，![[Pasted image 20240703104331.png]]
## USB枚举：
![[Pasted image 20240715093609.png]]
每个逻辑上的功能，都有自己的“接口”，对于USB摄像头，有两个接口，分别为<span style="color:#ffc000">控制接口</span>和<span style="color:#ffc000">流接口</span>，IT为input terminal。CT为control terminal。输入终端的数据传给PU，PU的数据传给OT，最终用来控制video streaming。控制的内容包括但是不限于选择分辨率等等。
uvc驱动程序要做的就是解析video function描述符，并构造出一堆结构体，来管理框图中的每一个模块：
* entity：解析、构造entity，放入uvc_device结构体中entities指向的链表。
* streaming：解析format/frame，构造uvc_streaming、放入uvc_device中streams指向的链表
* 对每个OT，找到完整的chain，注册video_dev



USB摄像头插入后，函数调用流程：
probe函数被调用后，会创建一个uvc_devices，这就是用来控制摄像头的主结构体.
完成entity的扫描和添加后，将会进行以下工作：
![[Pasted image 20240724221857.png]]
其中，扫描设备将会扫描到完整的信号链路（从输入终端一直到流接口），当扫描到链路后，会把chain存入uvc_devices，然后进行注册，注册的过程会查找是否有TT_STREAMING类型的输出端口，如果有，则会进行video_device的注册并生成/dev/videoX。
## USB摄像头控制过程：
