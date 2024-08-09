# 十九届智能车视觉组

战邮MNJUPTGA队 软件部分

本工程基于的开源工程文档：

- Rt-thread：[RT-Thread 官方文档](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/README)


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

##  代码结构说明

为了方便编写逻辑，使用了rt_thread操作系统，并且使用信号量来作为逻辑的触发，更加方便的编写了最终的逻辑

```c
--【车代码架构】
--【任务层】
--【硬件算法层】【外部通信层】
--【硬件驱动层】
```

![Snipaste_2024-07-20_18-49-59](C:\Users\CANCER_CMP\Desktop\smart_car_19\img\Snipaste_2024-07-20_18-49-59.png)

- 硬件驱动层
  - step_motor 文件：机械臂以及转盘
  - buzzer 文件：蜂鸣器
  - motor 文件：电机速度闭环驱动
  - encoder 文件：编码器驱动
  - Attitude_algorithm 文件：航向角姿态解算与IMU初始化
- 硬件算法层
  - my_vision 文件：视觉算法
  - camera 文件：摄像头预处理算法以及总纂风运行逻辑整合
  - car 文件：车辆运动解算，角度闭环，定距移动，车辆定角度旋转等API
- 外部通信层
  - art 文件：用于与art通信，切换art模式，接收art信息等
  - MCXvision 文件：用于与MCX通信，切换MCX模式以及接收MCX数据
- 任务逻辑层：
  - side_catch 文件：边线处理逻辑
  - locate_picture 文件：定位抓取/防止文件
  - final 文件：终点分类逻辑文件
  - trace_line 文件：巡线任务文件
  - cross/circule_handle 文件：十字和圆环处理逻辑

## 视觉算法部分

视觉算法部分，图像预处理部分使用的大津法+八邻域，借鉴的车u的算法，而元素判断部分使用的则是自己的方法

### 元素判断部分思路

元素判断我使用的是分段的思路，将扫出来的边线先分为缺线段和不缺线段，然后将不缺线段分为单调的弯道，不单调的弧，直线三个种类，并且根据分段来初步判断元素，若初步判断元素可能成立，再进行特征点的解析，用于防止误识别（某些情况确实还是会，但是是小概率事件）

#### 角点检测：

我们角检测的思路分为三类

- 直线+缺陷，直线段上不可能有角点但是直线接了缺陷，那么直线的最后一个点就是角点

  ![直线接缺线](C:\Users\CANCER_CMP\Desktop\smart_car_19\img\直线接缺线.png)

- 但是有时候并不是这样突变，或者说变化很快但是还没变到缺线，比如下图，此时我们角点检测的思路为计算某点和某点前5个坐标的点和后5个坐标的点三个点的cos角度值，判断其是否为小于一个阈值，小于则为角点

  ![没有缺线的角点](C:\Users\CANCER_CMP\Desktop\smart_car_19\img\没有缺线的角点.png)

​									![变化较慢的角点](C:\Users\CANCER_CMP\Desktop\smart_car_19\img\变化较慢的角点.png)		

- 同时我们角点检测只对可能是十字或者圆环的状态进行检测，这样大大降低了误识别的概率

#### 十字：

- 十字分为两个状态，一个是进入状态，一个是十字中间状态，19届我们处理十字任务时需要能够使得车停在对的位置，比如十字中心处

- 十字进入状态的情况非常多，大致分为正入十字和斜入十字两种

  <div>			<!--块级封装-->
      <center>	<!--将图片和文字居中-->
      <img src="C:\Users\CANCER_CMP\Desktop\smart_car_19\img\十字情况1.png"
           alt="无法显示图片时显示的文字"
           style="zoom:50%"/>
      <br>		<!--换行-->
      正入十字	<!--标题-->
      </center>
  </div>

  



<div>			<!--块级封装-->
    <center>	<!--将图片和文字居中-->
    <img src="C:\Users\CANCER_CMP\Desktop\smart_car_19\img\十字情况3.png"
         alt="无法显示图片时显示的文字"
         style="zoom:50%"/>
    <br>		<!--换行-->
    斜入十字1<!--标题-->
    </center>
</div>



<div>			<!--块级封装-->
    <center>	<!--将图片和文字居中-->
    <img src="C:\Users\CANCER_CMP\Desktop\smart_car_19\img\十字情况2.png"
         alt="无法显示图片时显示的文字"
         style="zoom:50%"/>
    <br>		<!--换行-->
    斜入十字2<!--标题-->
    </center>
</div>

正入十字不需要补线，但是斜入十字不补线必跑飞；正入十字就是：双边都是 不缺陷-缺陷-不缺陷 ，斜入十字1：一边圆弧，另一边缺线-不缺线，斜入十字2：一边 缺线-不缺线-缺线，另一边 缺线-不缺线，补线策略其实也非常明显

- 两边有线，中间缺线只需要找到两个边的角点，连接一条线即可
- 两边远处有线，近处缺线，只需要将远处的线延长至近处即可，做一条切线
- 全部都不缺陷，但是又明显的角点，只需要在角点处做一条切线延伸到远处即可
- 另外正入十字情况不需要补线，斜入十字补线即可

## 开发日志

### 4/21

目前进度：已经将之前的小车运动相关库移植完毕，目前正在准备调试视觉部分——————

### 4/24

发现编码器和电机不匹配，重新匹配了编码器和电机，并且准备调整角度闭环，总钻风视觉巡线部分完成，准备编写巡线代码

### 6/12

继续编写圆环逻辑，暂定圆环解决方案如下

- 定留在圆环中间，转向圆环，前进一个固定的距离抓取图片，然后转向180度，分类最不好分类的图片
- 转向至初始前进方向，然后横向移动回圆环中心，给Vx一个值，开始巡线
- 检测到航向角与前进方向的航向角相差60的倍数的时候，进行前进识别放置图片
- 当放置完成后，将航向角拉回前进方向吗，然后形式固定距离，离开圆环

### 7/20

省赛结束了，最后才发现大类标号错了，可惜了，华东第七，悬了，不过确实我们只认真调试了两个月，不像别的佬调了大半年，就这样吧，自己的问题认命吧

