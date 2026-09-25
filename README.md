# 单片机定制项目集

STM32F103 系列课程设计 / 定制项目,共 3 套已完成系统。均为 Keil MDK(ARM Compiler)工程,源码结构统一为 `Start`(启动与外设库) / `Library`(标准外设库) / `User`(应用层) / `Hardware`(驱动) / `System`(系统配置)。

| 目录 | 项目 | 说明 |
| --- | --- | --- |
| `03-five-sensor-presence-detection` | 五传感器 + 人员(人体存在)检测系统 | 含 HLK-LD2402 毫米波雷达人体存在检测、TVOC-301 气体传感器等 |
| `04-five-sensor-water-pump` | 五传感器 + 抽水泵系统 | 传感器采集 + 水泵联动控制 |
| `06-corridor-management` | 楼道管理系统 | OLED 显示、照明与安全策略控制,附 PC 端测试用例 |

## 使用说明

1. 用 Keil MDK 打开各目录下的 `Project.uvprojx`。
2. 器件为 STM32F103C8(中等容量),需安装 STM32F1 Series Device Pack。
3. 仓库不含编译产物(`Objects/`、`Listings/`、`*.axf`、`*.map` 等),首次打开后需自行 Build。
4. 各工程内 `Hardware/` 与 `User/` 下的驱动/应用代码即项目实际逻辑,引脚分配可参考工程内 `引脚接线图.txt` 与各目录 README。

## 未纳入仓库的内容

- 编译输出:`Objects/`、`Listings/`、`DebugConfig/`、`*.axf`、`*.map`、`*.crf`、`*.hex` 等
- IDE 界面状态文件 `*.uvguix.*`(`Project.uvprojx`、`Project.uvoptx` 已保留,可直接打开工程)
- 厂商模块资料包:`LD2402人体存在单模块资料`、`TVOC-301二氧化碳传感器模块资料`,约 139 MB 的压缩包与数据手册
- PC 端测试可执行文件 `tests/*.exe`(`.c` 测试源码已保留)
