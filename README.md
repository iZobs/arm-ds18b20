说明：
    (1) 这是一个为TQ2440写的DS18B20 温度传感器驱动.DQ接GPF4脚，用的是miscdriver驱动。
    (2)app文件夹存放测试应用程序
    (3)module文件夹存放驱动代码
    (4)document文件夹存放相关的datasheet等文档
        
使用:
    (1) ~/ cp DS18B20.ko  /dev
        ~/ isnmod DS18B20.ko (将会生成 /dev/DS18B20 设备)
        
    (2) ~/ cp ds18b20  /bin
        ~/ ./ds18b20

