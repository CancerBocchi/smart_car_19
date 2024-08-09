import pyb
import sensor, image, time, math
import os, tf
from machine import UART


sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_brightness(600)
sensor.skip_frames(time = 20)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()

net_path = "classify_model.tflite"  # 定义模型的路径
net_abc_path = "abc123_model.tflite"
#labels = [line.rstrip() for line in open("/sd/labels.txt")] # 加载标签
net = tf.load(net_path, load_to_fb=True) # 加载模型
net_abc = tf.load(net_abc_path, load_to_fb=True)

uart = UART(2, baudrate=115200)
uart_array = [0X0A, 0, 0X0D]
classify_result = [
            [0x61,0],   #
            [0X62,0],   #
            [0X63,0],   #
            [0X64,0],   #
            [0X65,0],   #
            [0X66,0],   #
            [0X67,0],   #
            [0X68,0],   #
            [0X69,0],   #
            [0X6A,0],   #
            [0X6B,0],   #
            [0X6C,0],   #
            [0X6D,0],   #
            [0X6E,0],   #
            [0X6F,0]    #
            ]

classify_result_abc = [
            [0x31,0],   #
            [0X32,0],   #
            [0X33,0],   #
            [0X61,0],   #
            [0X62,0],   #
            [0X63,0],   #
            [0X64,0],   #
            [0X65,0],   #
            [0X66,0],   #
            [0X67,0],   #
            [0X68,0],   #
            [0X69,0],   #
            [0X6A,0],   #
            [0X6B,0],   #
            [0X6C,0],   #
            [0X6D,0],   #
            [0X6E,0],   #
            [0X6F,0]    #
            ]
def classify_reset():
    global classify_result, classify_result_abc
    classify_result = [
            [0x61,0],   #
            [0X62,0],   #
            [0X63,0],   #
            [0X64,0],   #
            [0X65,0],   #
            [0X66,0],   #
            [0X67,0],   #
            [0X68,0],   #
            [0X69,0],   #
            [0X6A,0],   #
            [0X6B,0],   #
            [0X6C,0],   #
            [0X6D,0],   #
            [0X6E,0],   #
            [0X6F,0]    #
            ]
    classify_result_abc = [
            [0x31,0],   #
            [0X32,0],   #
            [0X33,0],   #
            [0X61,0],   #
            [0X62,0],   #
            [0X63,0],   #
            [0X64,0],   #
            [0X65,0],   #
            [0X66,0],   #
            [0X67,0],   #
            [0X68,0],   #
            [0X69,0],   #
            [0X6A,0],   #
            [0X6B,0],   #
            [0X6C,0],   #
            [0X6D,0],   #
            [0X6E,0],   #
            [0X6F,0]    #
            ]
count = 0
mode = 0

st_x,st_y = 98,71
ed_x,ed_y = 220,177

while(True):
    img = sensor.snapshot()
    uart_num = uart.any()
    if(uart_num):
        uart_str = uart.read(uart_num) # 读取串口数据
        #uart.write("I'll handle these.")
        if(uart_str[0] == 0X41):    # *卡片分类，不用count
            mode = 6
        if(uart_str[0] == 0X43):    # *卡片分类
            mode = 1
            print("I'm classifying pictures")
        if(uart_str[0] == 0X42):    # 卡片分类_use_find_rect
            mode = 3
            print("I'm classifying pictures")
        if(uart_str[0] == 0X4D):    # *数字字母分类_no_count
            mode = 5
        if(uart_str[0] == 0X4E):    # *数字字母分类_count_mode
            mode = 4
            print("I'm classifying numbers and alphabets")
        if(uart_str[0] == 0X4F):    # 数字字母分类_use_find_rect
            mode = 2
            print("I'm classifying numbers and alphabets")
        if(uart_str[0] == 0X53):    # RESET
            mode = 0
            classify_reset()
            print("Return to RESET.")
    if mode is 0:
        uart_array[1] = 0
        uart.write(bytearray(uart_array))
        #print(uart_array)
    if mode is 1:
        frame = img.copy(roi=(st_x,st_y,ed_x-st_x,ed_y-st_y))
        #img1 = img.copy(frame)
        count += 1
        img.draw_rectangle((st_x,st_y,ed_x-st_x,ed_y-st_y), color = (255, 0, 0))
        #img1 = img.copy(r.rect())
        for obj in tf.classify(net , frame, scale=1, offset=0):
            for i in range(len(classify_result)):
                classify_result[i][1] += obj.output()[i]
            #print(classify_result)
        if count >= 2:
            count = 0
            classify_result.sort(key=lambda x: x[1], reverse=True)
            uart_array[1] = classify_result[0][0]
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            print(classify_result[0][0],classify_result[0][1])
            classify_reset()
        uart.write(bytearray(uart_array))
    if mode is 2:
        for r in img.find_rects(threshold = 5,quality = 80100):
            img.draw_rectangle(r.rect(), color = (255, 0, 0))
            img1 = img.copy(r.rect())
            for obj in tf.classify(net_abc , img1,scale=1,offset=0):
                #print(obj.output())
                for i in range(len(classify_result_abc)):
                    classify_result_abc[i][1] = obj.output()[i]
            classify_result_abc.sort(key=lambda x: x[1], reverse=True)
            uart_array[1] = classify_result_abc[0][0]
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            print(classify_result_abc[0][0],classify_result_abc[0][1])
            classify_reset()
        uart.write(bytearray(uart_array))
    if mode is 3:
        for r in img.find_rects(threshold = 10050):
            img.draw_rectangle(r.rect(), color = (255, 0, 0))
            img1 = img.copy(r.rect())
            for obj in tf.classify(net, img1, scale=1,offset=0):
                for i in range(len(classify_result)):
                    classify_result[i][1] = obj.output()[i]
            classify_result.sort(key=lambda x: x[1], reverse=True)
            uart_array[1] = classify_result[0][0]
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            print(classify_result[0][0],classify_result[0][1])
            classify_reset()
        uart.write(bytearray(uart_array))
    if mode is 4:           # 数字字母分类，用count
        frame = img.copy(roi=(98,71,220-98,177-71))
        #img1 = img.copy(frame)
        count += 1
        img.draw_rectangle((st_x,st_y,ed_x-st_x,ed_y-st_y), color = (255, 0, 0))
        #img1 = img.copy(r.rect())
        for obj in tf.classify(net_abc , frame, scale=1, offset=0):
            for i in range(len(classify_result_abc)):
                classify_result_abc[i][1] += obj.output()[i]
            #print(obj.output())
        if count >= 2:
            count = 0
            classify_result_abc.sort(key=lambda x: x[1], reverse=True)
            uart_array[1] = classify_result_abc[0][0]
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            uart.write(bytearray(uart_array))
            print(classify_result_abc[0][0],classify_result_abc[0][1])
            classify_reset()
        uart.write(bytearray(uart_array))
    if mode is 5:       # 数字字母分类，"0X4D"
        classify_reset()
        frame = img.copy(roi=(st_x,st_y,ed_x-st_x,ed_y-st_y))
        #img1 = img.copy(frame)
        #count += 1
        img.draw_rectangle((st_x,st_y,ed_x-st_x,ed_y-st_y), color = (255, 0, 0))
        #img1 = img.copy(r.rect())
        for obj in tf.classify(net_abc , frame, scale=1, offset=0):
            for i in range(len(classify_result_abc)):
                classify_result_abc[i][1] = obj.output()[i]
            #print(obj.output())
        classify_result_abc.sort(key=lambda x: x[1], reverse=True)
        uart_array[1] = classify_result_abc[0][0]
        uart.write(bytearray(uart_array))
        uart.write(bytearray(uart_array))
        uart.write(bytearray(uart_array))
        uart.write(bytearray(uart_array))
        uart.write(bytearray(uart_array))
        uart.write(bytearray(uart_array))
        print(classify_result_abc[0][0],classify_result_abc[0][1])
        #uart.write(bytearray(uart_array))
    if mode is 6:       # pic分类，不用count‘0X41’
        classify_reset()
        frame = img.copy(roi=(st_x,st_y,ed_x-st_x,ed_y-st_y))
        #img1 = img.copy(frame)
        #count += 1
        img.draw_rectangle((st_x,st_y,ed_x-st_x,ed_y-st_y), color = (255, 0, 0))
        #img1 = img.copy(r.rect())
        for obj in tf.classify(net , frame, scale=1, offset=0):
            for i in range(len(classify_result)):
                classify_result[i][1] = obj.output()[i]
            #print(obj.output())
        classify_result.sort(key=lambda x: x[1], reverse=True)
        uart_array[1] = classify_result[0][0]
        uart.write(bytearray(uart_array))
        uart.write(bytearray(uart_array))
        uart.write(bytearray(uart_array))
        uart.write(bytearray(uart_array))
        uart.write(bytearray(uart_array))
        uart.write(bytearray(uart_array))
        print(classify_result[0][0],classify_result[0][1])
        #uart.write(bytearray(uart_array))






