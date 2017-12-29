DON'T use libusb shipped with mingw64!!!

example
```
Info: fail to load "usb.ids", using embbed db
> Summary
>> bus 1, root
>> bus 1, ports 1
>> bus 1, ports 3
>> bus 2, root
>> bus 2, ports 1
>> bus 2, ports 1.2
>> bus 3, root
>> bus 3, ports 1
>> bus 3, ports 1.2
>> bus 3, ports 1.3
>> bus 3, ports 1.6

> Detials
>> bus 1, root
>> VID_1033&PID_0194 (Host Controler)
  Vendor       : Nucam Corp.
  Device       : (null)
  Address      : 1
  Class        : interface

>> bus 1, ports 1
>> VID_0BDA&PID_0411 (USB3.0)
  Vendor       : Realtek Semiconductor Corp.
  Device       : (null)
  Address      : 3
  Class        : hub
  Interface0
     IN,  0x81, 2 bytes

>> bus 1, ports 3
>> VID_0BDA&PID_5411 (USB2.0)
  Vendor       : Realtek Semiconductor Corp.
  Device       : (null)
  Address      : 2
  Class        : hub
  Interface0
     IN,  0x81, 1 bytes
  Interface1
     IN,  0x81, 1 bytes

>> bus 2, root
>> VID_8086&PID_1C26 (Host Controler)
  Vendor       : Intel Corp.
  Device       : (null)
  Address      : 1
  Class        : interface

>> bus 2, ports 1
>> VID_8087&PID_0024 (USB2.0)
  Vendor       : Intel Corp.
  Device       : Integrated Rate Matching Hub
  Address      : 2
  Class        : hub
  Interface0
     IN,  0x81, 2 bytes

>> bus 2, ports 1.2
>> VID_0781&PID_5581 (USB3.0(on 2.0 port))
  Vendor       : SanDisk Corp.
  Device       : Ultra
  Address      : 3
  Class        : interface
  Interface0
     IN,  0x81, 512 bytes
     OUT, 0x02, 512 bytes

>> bus 3, root
>> VID_8086&PID_1C2D (Host Controler)
  Vendor       : Intel Corp.
  Device       : (null)
  Address      : 1
  Class        : interface

>> bus 3, ports 1
>> VID_8087&PID_0024 (USB2.0)
  Vendor       : Intel Corp.
  Device       : Integrated Rate Matching Hub
  Address      : 2
  Class        : hub
  Interface0
     IN,  0x81, 1 bytes

>> bus 3, ports 1.2
>> VID_046D&PID_C52F (USB2.0)
  Vendor       : Logitech, Inc.
  Device       : Unifying Receiver
  Address      : 6
  Class        : interface
  Config: 1
  Manufacturer(1): "Logitech"
  Product(2): "USB Receiver"
  Interface0
     IN,  0x81, 8 bytes
  Interface0
     IN,  0x82, 20 bytes

>> bus 3, ports 1.3
>> VID_147E&PID_2016 (bcdUSB = 0x0101)
  Vendor       : Upek
  Device       : Biometric Touchchip/Touchstrip Fingerprint Sensor
  Address      : 3
  Class        : interface
  Config: 1
  Manufacturer(1): ""
  Product(2): ""

>> bus 3, ports 1.6
>> VID_04F2&PID_B221 (USB2.0)
  Vendor       : Chicony Electronics Co., Ltd
  Device       : integrated camera
  Address      : 4
  Class        : etc.
  Interface0
     IN,  0x81, 16 bytes
  Interface0
  Interface1
     IN,  0x82, 960 bytes
  Interface2
     IN,  0x82, 1024 bytes
  Interface3
     IN,  0x82, 2908 bytes
  Interface4
     IN,  0x82, 3072 bytes
  Interface5
     IN,  0x82, 4956 bytes
  Interface6
     IN,  0x82, 5056 bytes
  Interface7
     IN,  0x82, 5116 bytes
```

