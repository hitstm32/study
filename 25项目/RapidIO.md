RIO分三层：逻辑层，传输层，物理层
![[Pasted image 20240312141000.png]]
![[Pasted image 20240312141015.png]]
RIO以包为基础。由包头，载荷数据和16bitsCRC校验组成，载荷数据长度不超过256字节，包的格式如下：
![[Pasted image 20240312141109.png]]
RIO逻辑层支持的事务主要有直接IO事务和消息传递事务，直接IO的前提是，主设备知道从设备的存储器映射，能够直接读写从设备存储器。
![[Pasted image 20240312141610.png]]
NREAD读操作，不需要响应，包格式类型为2。
读返回的13类数据包格式如下：
![[Pasted image 20240312141756.png]]
RIO也支持消息传递：
![[Pasted image 20240312141921.png]]
这两种的区别是DOORBELL只支持16bit消息传递，Message Operation则支持4096字节。

XIlinx的SRIO IP核提供了三种接口，分别为IOPort，MessagePort和User-Defined Port
![[Pasted image 20240312143441.png]]
![[Pasted image 20240312143546.png]]
数据传输时序：
![[Pasted image 20240312145043.png]]