## 中文版，请往下拖 
## Decoding Random Bluetooth Address of IOS 
To protect the private, Bluetooth 4.0 Low Energy usually to do advertising with a random private address. <br>
<br>
The private address may be of either of the following two sub-types: <br>
• Non-resolvable private address<br>
• Resolvable private address<br>
<br>
A resolvable private address may be resolved if the corresponding device’s IRK is available using this procedure. If a resolvable private address is resolved, the device can associate this address with the peer device. <br>

When IOS is enabling with continuity service, it will always sending BLE advertising to let other IOS device to know. Of course, the Bluetooth MAC Address is random and changed every 15 minutes. <br>

But this random address is a Resolvable private address. If the IRK of this IOS device is known, the random address can be determined if it belongs to this device. <br>

## Get IRK of your iPhone
Use an ESP32 board and download with “get_irk” project. It will start a BLE service. <br>
Pay attention on following settings.<br>
![](https://github.com/fryefryefrye/Bluetooth-keyless-system/raw/master/img/arduino_setting.jpg) <br>
Use your iPhone install with “LightBlue” APP, find the “ESP_BLE_SECURITY” service, and connect it, the IRK will be print out. <br>
![](https://github.com/fryefryefrye/Bluetooth-keyless-system/raw/master/img/get_irk.jpg) <br>

## Monitor BLE advertising with nRF24L01
With the help of the following project, we can Monitor BLE advertising with very cheap nRF24L01 module<br>
https://github.com/Pranavgulati/RF24BLE<br>
https://github.com/nRF24/RF24<br>
Install above two library into Arduino.<br>
Download “nrf_ble_add” project into a Arduino nano board, you can Monitor BLE advertising and determine if the MAC address in the air is belong to your iPhone.<br><br>
These is only one key function you need to call to check MAC address. Everything needed for this function is in key.h file.You can use it in your other project.<br>
BOOLEAN btm_ble_addr_resolvable(BD_ADDR rpa, esp_bt_octet16_t irk)<br><br>

You can build the Arduino based BLE Monitor Just connect nRF24L01 module with Arduino follow the table.<br>

![](https://github.com/fryefryefrye/Bluetooth-keyless-system/raw/master/img/uno_nrf.jpg) <br>

![](https://github.com/fryefryefrye/Bluetooth-keyless-system/raw/master/img/ResolveAddress.jpg) <br>

## Reference
Most of the code is copied and pasted from the following project.Great Thanks to this great company and their amazing ESP32 module with source code.<br>
https://github.com/espressif/esp-idf <br>

# 蓝牙无线接近开关

## 原理
IOS的连续服务功能，会用蓝牙4.0的低功耗不断的发送“广告”包。
但是这个广告包用的是一个随机蓝牙地址。
只不过这个随机地址是可解析的，前提是你要有相应的Key。
在蓝牙规范里，这个key就是 IRK。

## 获得你的iPhone 的 IRK
用Arduino环境，把get_irk项目烧进一块ESP32板子，这块板子就会打开一个蓝牙服务。
然后在iPhone上安装LightBlue软件，找到“ESP_BLE_SECURITY”服务，点击连接，就会有提示是否配对。确定后手机端连接成功，ESP32端就会打印出IRK。

## 低成本监听蓝牙广播包
用Arduino环境，把nrf_ble_add项目烧进一块nano板子，连接上一块nRF24L01，即可监听广播包，提取其中的随机MAC地址，然后与之前获得的IRK进行比较，就能判断是不是自己的手机。


