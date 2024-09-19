http是超文本传输协议的缩写，主要用于网页的传输，也经常应用在网络API的开发。
* HTTP是无连接的，每次只处理一个请求，服务器处理完请求，并收到客户应答后，就断开连接。
* 媒体独立，可传输任意信息。
* 无状态，服务器不记忆之前请求的信息。
流程：
* 请求
	* 请求行：地址和动词（get、post）
		* GET /hello.txt HTTP/1.1
	* 请求头（用来描述发送者的信息）
		* 反爬一般就是通过请求头实现，通过判定请求头的内容，来确定是否响应
		* 里面有一些参数，例如可以接受什么语言，用的什么浏览器等，cookie等。
	* 请求参数（例如要搜索的关键词）
		* get 请求的请求参数直接放在地址参数里，post请求的参数放在请求体里，网页上看不见，只能抓包才能看见。
* 响应
	* 响应代码：200表示成功、404表示不存在等
	* 响应头：描述内容的一些信息
		* 网页格式，编码方式，网页过期时间等
		* 返回的cookie，需要保存下来到本地，以后请求的时候发送coookie
		* content-type：响应内容是什么格式，例如HTML，纯文本，图片，XML等
	* 响应内容：HTML、json、图片等
一个完整的请求：
```json
{
  "args": {

  },
  "headers": {
    "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7",
    "Accept-Encoding": "gzip, deflate, br, zstd",
    "Accept-Language": "zh-CN,zh;q=0.9,en;q=0.8",
    "Host": "httpbin.org",
    "Priority": "u=0, i",
    "Sec-Ch-Ua": "\"Chromium\";v=\"128\", \"Not;A=Brand\";v=\"24\", \"Google Chrome\";v=\"128\"",
    "Sec-Ch-Ua-Mobile": "?0",
    "Sec-Ch-Ua-Platform": "\"Windows\"",
    "Sec-Fetch-Dest": "document",
    "Sec-Fetch-Mode": "navigate",
    "Sec-Fetch-Site": "none",
    "Sec-Fetch-User": "?1",
    "Upgrade-Insecure-Requests": "1",
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Safari/537.36",
    "X-Amzn-Trace-Id": "Root=1-66eb7d94-690d603b49de24a850407f75"
  },
  "origin": "124.115.222.147",
  "url": "https://httpbin.org/get"
}
```

# DIGEST认证
海康威视的摄像头认证通过DIGEST协议进行。digest鉴权是一种基于摘要算法的身份验证机制，用于网络通信中验证用户身份。每次发送请求中使用加密的密码摘要来验证用户身份，从而提高安全性。
## 流程:
* 客户端请求：
	* 客户端向服务器发送请求
	* 请求包含客户端认证信息，如用户名密码
* 服务器响应：
	* 服务器收到请求后，生成随机的“challenge”字符串，发给客户端
	* 服务器还会生成一个随机的“nonce（number used once）”值，包含在响应中
* 客户端响应：
	* 客户端收到服务器响应后，根据challenge字符串和密码生成一个“响应摘要（response digest）”
	* 响应摘要的生成过程包括对用户名、密码、challege和nonce值进行算法运算，一般通过MD5或者SHA-1等加密算法