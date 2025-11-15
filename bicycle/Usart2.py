import cv2
import torch
import numpy as np
from pathlib import Path
from models.common import DetectMultiBackend
from utils.general import check_img_size, non_max_suppression, scale_boxes
from utils.plots import Annotator, colors
from utils.torch_utils import select_device
import serial
import time

def mid(follow, mask):
    halfWidth = follow.shape[1] // 2
    half = halfWidth
    mid = 0  # 初始化 mid
    for y in range(follow.shape[0] - 90, 90, -1):
        left = np.argmax(mask[y][0:half] == 255) if np.any(mask[y][0:half] == 255) else max(0, half - halfWidth)
        right = np.argmax(mask[y][half:follow.shape[1]] == 255) + half - 1 if np.any(
            mask[y][half:follow.shape[1]] == 255) else min(follow.shape[1], half + halfWidth)

        mid = (left + right) // 2
        follow[y, int(mid)] = 255

    error = follow.shape[1] // 2 - int(mid)
    return follow, error

def main():
    # 选择设备
    device = select_device('')

    # 模型路径
    model_path = Path('./best.pt')  # 确保路径正确
    model = DetectMultiBackend(model_path, device=device, dnn=False)

    stride, names, pt = model.stride, model.names, model.pt
    imgsz = check_img_size((224, 224), s=stride)[0]  # 尝试更小的图像大小

    # 使用 OpenCV 初始化摄像头
    cap = cv2.VideoCapture(0)  # 打开第一个摄像头
    cap.set(cv2.CAP_PROP_FPS, 15)  # 设置每秒处理15帧

    if not cap.isOpened():
        print("Error: Cannot open webcam")
        exit()

    frame_skip = 2  # 每处理一帧后跳过的帧数
    frame_count = 0

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error: Cannot read frame")
            break

        frame = cv2.resize(frame, (640, 480))

        # HSV阈值分割
        img_hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        mask = cv2.inRange(img_hsv, np.array([0, 0, 200]), np.array([180, 40, 255]))
        follow = mask.copy()
        follow, error = mid(follow, mask)
        error = ((error * 110) / 20)

        # 确保 error 在有效范围内
        error = int(error)
        print(f"error: {error}")
        if error < -128 or error > 127:
            data_to_send = error.to_bytes(2, byteorder='little', signed=True)  # 使用2字节
        else:
            data_to_send = error.to_bytes(1, byteorder='little', signed=True)  # 使用1字节

        ser.write(data_to_send)
        ser.flush()  # 使用flush()而不是flushInput()
        frame_count += 1
        if frame_count % (frame_skip + 1) != 0:
            continue

        img = np.ascontiguousarray(cv2.resize(frame, (imgsz, imgsz))[:, :, ::-1].transpose(2, 0, 1))

        img = torch.from_numpy(img).to(device).float() / 255.0  # 使用 FP32 并归一化
        if img.ndimension() == 3:
            img = img.unsqueeze(0)

        with torch.no_grad():  # 禁用梯度计算，加快推理速度
            pred = model(img, augment=False, visualize=False)
            pred = non_max_suppression(pred, 0.25, 0.45, None, False, max_det=1000)

        for det in pred:  # detections per image
            annotator = Annotator(frame, line_width=2, example=str(names))

            if len(det):
                det[:, :4] = scale_boxes(img.shape[2:], det[:, :4], frame.shape).round()
                for *xyxy, conf, cls in reversed(det):
                    c = int(cls)
                    label = f'{names[c]}'
                    ser.write(label.encode('utf-8'))  # 将label编码为字节流
                    ser.flush()  # 使用flush()而不是flushInput()
                    annotator.box_label(xyxy, label, color=colors(c, True))

        time.sleep(0.1)  # 可以根据需要调整这个时间
        count = ser.inWaiting()
        if count != 0:
            recv = ser.read(count).decode('utf-8')  # 解码为字符串
            print(recv)

        # 显示结果帧
        cv2.imshow('Real-time Detection, YOLOv5', frame)
        if cv2.waitKey(1) == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    ser = None
    try:
        # 打开串口
        ser = serial.Serial('/dev/ttyS0', 115200)
        if not ser.isOpen():
            ser.open()  # 打开串口
        ser.write(b"Raspberry pi is ready\n")

        main()  # 调用主函数
    except KeyboardInterrupt:
        print("程序被中断")
    except Exception as e:
        print(f"发生错误: {e}")
    finally:
        if ser is not None:
            ser.close()
