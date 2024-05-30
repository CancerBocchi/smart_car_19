# 十九届智能车视觉组

你邮智能车小队

本工程基于的开源工程文档：

- Rt-thread：[RT-Thread 官方文档](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/README)

- 


## 引脚规划

预留出 2个SPI，2个I2C，4个Encoder，3个UART，1个CSI

- Encoder:

  | Encoder1 | Encoder2 | Encoder3 | Encoder4 |
  | :------: | :------: | :------: | :------: |
  |    C0    |    C4    |    C7    |   C10    |

  

- CSI接口：

  | Definition. | Pin. |
  | :---------: | :--: |
  |   CSI_D9    | B24  |
  |   CSI_D8    | B25  |
  |   CSI_D7    | B26  |
  |   CSI_D6    | B27  |
  |   CSI_D5    | B28  |
  |   CSI_D4    | B29  |
  |   CSI_D3    | B30  |
  |   CSI_D2    | B31  |
  | CSI_PIXCLK  | B20  |
  |  CSI_VSYNC  | B22  |

- I2C：I2C1，3可用

  |      |        MPU        |     reserved.     |
  | :--: | :---------------: | :---------------: |
  | Per. |       I2C1        |       I2C3        |
  | Pin. | B16(SCL),B17(SDA) | B22(SDA),B23(SCL) |

- SPI：SPI3，4可用

  |            |  SPI3  | SPI4 |
  | :--------: | :----: | :--: |
  |    CS0     |   B3   | C18  |
  |    CS1     | B4(no) | C19  |
  |    CS2     | B5(no) | C20  |
  |    CS3     | B6(no) | C27  |
  |    SCK     |   B0   | C23  |
  | SOUT(MOSI) |   B1   | C22  |
  | SIN(MISO)  |   B2   | C21  |


​	这里使用SPI3作为与屏幕通信的接口 屏幕其他接口：C24 C25 C26

- UART：

  CSI预留一个串口位置，LPUART4

  |      | debug            | openart1          |       CSI       |   data_output   |
  | ---- | ---------------- | ----------------- | :-------------: | :-------------: |
  | Per. | LPUART1          | LPUART5           |     LPUART4     |     LPUART2     |
  | Pin. | B12(TX)  B13(RX) | C28(RX)   C29(TX) | C16(TX) C17(RX) | B18(TX) B19(RX) |

- 电机控制接口：

  |       | 直流电机 | 舵机 |
  | :---: | :------: | :--: |
  | 电机1 |  D0,D1   | B10  |
  | 电机2 |  D2,D3   | B11  |
  | 电机3 | D14,D15  |  C6  |
  | 电机4 | D16,D17  |  C9  |
  | 电机5 |    -     | D12  |
  | 电机6 |    -     | D13  |
  | 电机7 |    -     |  -   |
  | 电机8 |    -     |  -   |

- 电磁铁 C30 C31 C27 C12 C13 C15 C16

## 开发日志

### 4/21

目前进度：已经将之前的小车运动相关库移植完毕，目前正在准备调试视觉部分

### 4/24

发现编码器和电机不匹配，重新匹配了编码器和电机，并且准备调整角度闭环，总钻风视觉巡线部分完成，准备编写巡线代码