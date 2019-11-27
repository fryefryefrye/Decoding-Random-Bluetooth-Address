# 蓝牙无线接近开关

## 原理
IOS的连续服务功能，会用蓝牙4.0的低功耗不断的发送“广告”包。
但是这个广告包用的是一个随机蓝牙地址。
只不过这个随机地址是可解析的，前提是你要有相应的Key。
在蓝牙规范里，这个key就是 IRK。

## Get IRK of you iPhone
用Arduino环境，把get_irk项目烧进一块ESP32板子，这块板子就会打开一个蓝牙服务。
然后在iPhone上安装LightBlue软件，找到“ESP_BLE_SECURITY”服务，点击连接，就会有提示是否配对。确定后手机端连接成功，ESP32端就会打印出IRK。
![](https://github.com/fryefryefrye/Bluetooth-keyless-system/raw/master/img/get_irk.jpg) <br>

  
## 低成本监听蓝牙广播包
用Arduino环境，把nrf_ble_add项目烧进一块nano板子，连接上一块nRF24L01，即可监听广播包，提取其中的随机MAC地址，然后与之前获得的IRK进行比较，就能判断是不是自己的手机。
You can build the Arduino based Controller whatever you like. Just connect nRF24L01 module with Arduino follow the table.<br>

| PIN  | NRF24L01|Arduino UNO/NANO|Mega1280/2560|
| ---------- | -----------| -----------| -----------|
|  1  |  GND  |  GND  |  GND  |
|  2  |  VCC  |  3.3V  |  3.3V  |
|  3  |  CE   |  digIO 7  |  digIO 7  |
|  4  |  CSN  |  digIO 8  |  digIO 8  |
|  5  |  SCK   |  digIO 13  |  digIO 52  |
|  6  |  MOSI  |  digIO 11  |  digIO 51  |
|  7  |  MISO  | digIO 12  |  digIO 50  |
|  8  |  IRQ  |  -  |-|
<br> 

![](https://github.com/fryefryefrye/Open-Source-RKS/raw/master/img/uno_nrf.jpg)


