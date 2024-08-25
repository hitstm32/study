作用：
* 引脚枚举与命名
* 引脚复用
* 引桥配置如上下拉、驱动强度

# 应用
在设备树中，pinctrl子系统分为pinctrl_server和pinctrl_client。对于client，写法统一。对于server，由于不同芯片的server是BSP工程师自己实现的，所以设备树写法也不统一。

举一下f1c200s里的例子：
```d
pio: pinctrl@1c20800 {
	compatible = "allwinner,suniv-f1c100s-pinctrl";
	reg = <0x01c20800 0x400>;
	interrupts = <38>, <39>, <40>;
	clocks = <&ccu 37>, <&osc24M>, <&osc32k>;
	clock-names = "apb", "hosc", "losc";
	gpio-controller;
	interrupt-controller;
	#interrupt-cells = <3>;
	#gpio-cells = <3>;
	uart0_pe_pins: uart0-pe-pins {
		pins = "PE0", "PE1";
		function = "uart0";
	};
	uart1_pe_pins: uart1-pe-pins {
		pins = "PD3", "PD4";
		function = "uart1";
	};
	mmc0_pins: mmc0-pins {
        pins = "PF0", "PF1", "PF2", "PF3", "PF4", "PF5";
        function = "mmc0";
    };
	spi1_pins: spi1-pins{
        pins = "PE7","PE8","PE9";
        function = "spi1";
    };
};
```

```d
&uart1 {
	pinctrl-names = "default";
	pinctrl-0 = <&uart1_pe_pins>;
	status = "okay";
};
```

```d
&pio {
	encoder_pins: encoder_pins{
	    pins = "PE11","PE5","PE4";
	    function = "gpio_in";
	};
};
rotary_encoder:rotary_encoder {
	status = "okay";
	compatible = "allwinner,ljz_rotary_encoder";
	pinctrl-names = "default";
	pinctrl-0 = <&encoder_pins>;
	EC-A = <&pio 4 11 0>;
	EC-B = <&pio 4 5 0>;
	EC-key = <&pio 4 4 0>;
};
```

# 内部实现：
pinctrl_server会被解析成一个结构体pinctrl_dev，供给右边client对应的程序调用。不需要自己去实现pinctrl_dev，只需要提供并注册一个pinctrl_desc，就可以生成pinctrl_dev。

从pinctrl_f1c200s.c的probe文件一步步来分析：
pinctrl_server在设备树中是个设备节点，会被转化为platform_dev，当与platform_driver匹配时，便会调用probe。

<span style="color:#ffc000">分析desc结构体是如何实现子系统三个功能</span>。
使用pinctrlops获得引脚组信息。
pmxops把引脚配置成某功能
