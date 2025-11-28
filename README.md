# 智慧停车管理系统

[![Qt](https://img.shields.io/badge/%23-6.0-41CD52?logo=qt&logoColor=white&logoSize=auto)](https://www.qt.io/)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)
[![OpenSSL](https://img.shields.io/badge/%23-3.6.0-721412?&logo=openssl&logoSize=auto)](https://www.openssl.org/)
[![OpenCV](https://img.shields.io/badge/OpenCV-4.1.2-5C3EE8?logo=opencv&logoColor=white)](https://opencv.org/)
[![SQLite](https://img.shields.io/badge/SQLite%203-003B57?logo=sqlite&logoColor=white)](https://sqlite.org/)
[![HyperLPR](https://img.shields.io/badge/HyperLPR-385D16?&logo=github)](https://github.com/szad670401/HyperLPR)

> 基于 Qt C++ 与计算机视觉技术的现代化智能停车场管理解决方案。集成车牌识别、自动计费、以及支付宝沙箱支付功能。

---

## 项目简介

本项目是一个模拟真实场景的智慧停车场管理系统。它利用摄像头实时捕获车辆画面，通过深度学习模型识别车牌，自动记录车辆出入库时间，并基于停留时长计算费用。

系统不仅实现了本地的数据管理，还通过集成 OpenSSL 和 HTTP 协议，手写实现了支付宝当面付的完整支付流程，是一个集成了 **端侧视觉识别** 与 **云端支付交互** 的综合性工程实践。

## 核心特性

### 1. 智能视觉识别
- **车牌识别 (LPR):** 集成 **HyperLPR** 高性能中文车牌识别框架。
- **抗干扰机制:** 实现了基于队列的 **多帧加权投票机制**，在连续多帧识别结果一致时才确认为有效车牌，有效防止识别抖动和误报。
- **二维码扫描:** 使用 **OpenCV** `QRCodeDetector` 识别支付二维码，支持扫码支付流程。

### 2. 支付系统集成
- **支付宝沙箱接入:** 对接支付宝开放平台 API。
- **原生加密实现:** 不依赖支付宝SDK ~~（C++其实也没有）~~ ，直接使用 **OpenSSL** 实现了 `RSA2 (SHA256WithRSA)` 签名算法和 URL 编码逻辑，手动构建符合支付宝规范的 HTTP POST 请求。
- **异步网络处理:** 使用 `QNetworkAccessManager` 处理高并发网络请求，界面不卡顿。

### 3. 数据与业务逻辑
- **出入库逻辑:** 自动判断车辆是进场还是离场，并在 SQLite 数据库中维护 `carPresent` (在场车辆) 和 `carLog` (历史记录)。
- **动态计费:** 支持自定义配置计费规则（如：每秒单价、计费单位），实时计算停车费。
- **数据可视化:** 提供数据总览面板，可查看在停车辆、常客统计及营收报表。

### 4. 配置管理
- 提供图形化设置界面，可实时修改支付网关参数、AppID 以及费率设置。

## 技术栈 | Tech Stack

| 模块 | 技术/库 | 说明 |
| :--- | :--- | :--- |
| **开发语言** | C++ 17 | 核心逻辑实现 |
| **GUI 框架** | Qt 6 (Widgets) | 界面与信号槽机制 |
| **视觉处理** | OpenCV 4.1.2 | 图像预处理、二维码识别 |
| **深度学习** | HyperLPR 3 + MNN | 边缘侧车牌检测与识别 |
| **数据库** | SQLite 3 | 轻量级本地数据存储 |
| **网络与加密** | OpenSSL 1.1 | 实现 RSA 签名与 SHA256 哈希 |
| **构建工具** | QMake | 项目构建与依赖管理 |

## 关键代码解析

### 多帧加权投票防抖
为了避免摄像头画面抖动导致的车牌识别跳变，系统采用了一个大小为 8 的队列进行投票：
```cpp
// mainwindow.cpp
if(plateVoteFlag > 0)
{
    // 累加置信度
    plateVote[idx].second += livePlate.second; 
    // 当某个车牌的累积置信度超过阈值（如 2.5/5.0）时，锁定该车牌
    if(plateVote[idx].second > 2.5) plateVoteFlag = 1; 
}
```

### 手写支付宝 RSA 签名
未使用现成支付宝 SDK，而是通过 OpenSSL 底层库实现RSA签名：
```cpp
// paymentservice.cpp
RSA* rsa = PEM_read_RSAPrivateKey(fp, nullptr, nullptr, nullptr);
SHA256_Update(&sha256_ctx, signStrUtf8.constData(), signStrUtf8.length());
RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, signBuf, &sig_len, rsa);
```

## 运行程序
### 快速试用
1. 在 [Release](https://github.com/TSJim0711/ParkingManageSystem/releases) 下载可执行档案 `ParkingManageSys.exe`。
2. 建立一个`rsa.pem`文件，并把一个支付宝沙箱私钥写进去。并将其放在 `ParkingManageSys.exe` 同级文件夹内。
3. 运行 `ParkingManageSys.exe`。

### 编译运行
1. 安装 **Qt 6.x** (包含 MSVC 编译器组件)。
2. 确保项目目录中包含以下依赖文件夹（需自行编译或下载对应的预编译库）：
   - `hyperlpr3/`
   - `opencv2/`
   - `openssl/`
3. 将 `r2_mobile` (HyperLPR模型) 和 `rsa.pem` (支付宝私钥) 放置在构建输出目录（Build Directory）下。
4. 使用 Qt Creator 打开 `ParkingManageSys.pro`。
5. 配置构建套件（建议使用 MSVC 64bit，因为链接了 `.lib` 文件）。
6. 点击构建并运行。
