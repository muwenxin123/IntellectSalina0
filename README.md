
# 智慧巡检与管控系统

基于 Qt 的智能视频巡检系统，支持多路视频实时检测、目标跟踪和事件记录。

![Version](https://img.shields.io/badge/version-1.0-blue)
![Platform](https://img.shields.io/badge/platform-Windows-brightgreen)
![Qt](https://img.shields.io/badge/Qt-5.11.2-green)

---

## 功能特性

- 🎥 **5路视频同时播放** - 支持多路 RTSP 流并发处理
- 🔍 **实时目标检测** - 支持 YOLO 模型动态切换
- 🖱️ **检测框点击交互** - 点击任意检测框查看详细信息
- 📊 **实时统计卡片** - 各类别目标数量统计显示
- 📸 **事件记录与回放** - 自动保存检测事件，支持回放查看
- 🗺️ **2D地图视图** - 无人机位置实时显示

---

## 系统架构

LY_AgriVideoPlayer (主控制器)
├── CameraWidget (视频窗口 ×5)
├── AIWebSocketClient (WebSocket通信)
├── DataResult (检测结果展示)
├── EventRecord (事件记录)
├── StatisticsCard (实时统计)
└── UAVAgriVideoDialogList (视频布局)


## 🚀 快速开始

### 环境要求
- Visual Studio 2015 或更高版本
- Qt 5.11.2
- Windows 7/10/11

### 编译运行
```bash
# 1. 克隆项目
git clone https://github.com/muwenxin123/IntellectSalina0.git

# 2. 用 Visual Studio 打开解决方案
IntellectSalina0.sln

# 3. 编译运行（F5）
