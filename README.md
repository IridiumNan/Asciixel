# Asciixel

使用 C++ 将图片、视频转换为 ASCII 字符画或字符动画的小工具。

**当前状态：设计阶段，尚无可运行程序。** 本文描述首版目标，不代表功能已经实现。

## 首版目标

- C++17 + CMake；以 Windows 为首个验证平台，核心算法保持平台无关。
- FFmpeg 解码本地 PNG、JPEG 和构建所支持的 SDR 视频格式；首版不播放音频。
- 图片输出单色终端字符画或 UTF-8 TXT；视频在支持 ANSI 的终端中播放单色字符动画。
- 用户指定等宽字体文件；像素字号默认 16，列数支持显式指定或终端自动适配，TXT 默认 120 列；按字符单元比例计算行数。
- 通过 FreeType 标定 ASCII 32～126 的实际 alpha 覆盖率，包含空格，缺字明确报告。
- 对图像分块求面积加权平均亮度，再按实际字形密度匹配，默认铺满字符密度范围。
- 支持黑底白字、白底黑字和保真亮度模式；视频采用时间戳调度与可关闭的字符迟滞。
- 同一字体配置在进程内只标定一次；错误可读，退出时恢复终端状态。

终端的字体与栅格化不由程序控制。指定字体用于标定及比例计算，用户需在终端设置相应字体；终端效果属于近似结果。

## 首版命令行约定

以下命令是待实现的接口示例，目前还不能运行。`fonts/mono.ttf` 是用户提供字体的示例路径。

```text
asciixel photo.png --font fonts/mono.ttf
asciixel photo.png --font fonts/mono.ttf --format txt --output art.txt --columns 120
asciixel photo.png --font fonts/mono.ttf --format txt --theme light --mapping faithful
asciixel clip.mp4 --font fonts/mono.ttf --columns auto --hysteresis 0
```

| 参数 | 默认值 | 说明 |
| --- | --- | --- |
| `<input>` | 必填 | 单个本地 PNG/JPEG 或支持的 SDR 视频文件 |
| `--font <path>` | 必填 | 等宽字体，face 0 |
| `--font-size <N>` | 16 | 像素字号，整数 1～256 |
| `--format terminal\|txt` | terminal | 终端预览或静态文本；视频不能输出 TXT |
| `--output <path\|->` | txt 为 `-` | 仅 TXT 使用；`-` 表示 stdout；拒绝覆盖现存文件 |
| `--columns <N\|auto>` | terminal 为 auto，txt 为 120 | 整数 1～4096；TXT 不支持 auto |
| `--theme dark\|light` | dark | 黑底白字或白底黑字，同时决定透明像素合成背景 |
| `--mapping stretch\|faithful` | stretch | 密度铺满或亮度保真，可与任意主题组合 |
| `--hysteresis <h>` | 视频为 0.005 | 有限数值 0～1，0 关闭；静态图片不接受显式设置 |
| `--help` | — | 单独使用，显示帮助 |

终端模式要求 stdout 为交互终端且支持 ANSI 和尺寸查询，不因重定向自动切换格式。终端预留最后一行和一列，自动模式选择能容纳的最大网格；显式尺寸放不下则报错。所有模式网格上限为 1,048,576 个字符。

TXT 固定为 UTF-8 无 BOM，每行 LF（包括最后一行），保留行尾空格，无 ANSI；文件不保存主题颜色，需在匹配的背景下查看。日志和错误只写 stderr。退出码为 0 成功、2 参数/配置错误、1 运行错误、130 用户中断。完整冲突规则、布局和背景契约见 [设计文档](docs/design.md)。

## 验收目标

| 场景 | 预期 |
| --- | --- |
| 黑白渐变 | 输出字符覆盖率随目标覆盖率单调变化；反色方向正确 |
| 空格、句点、密集字符 | 使用完整单元格归一化，空格密度为零；排序来自实际字体 |
| 已知比例的圆形 | 网格比例按单元格宽高修正，行数与设计公式一致 |
| 非整数采样边界 | 使用覆盖面积权重，常量图像缩放后亮度保持不变 |
| 重复运行同一输入 | 固定配置下字符选择可重复，不依赖容器迭代顺序 |
| 变帧率视频、末尾延迟帧 | 按显示时间戳播放，EOF 排空解码器；不积累固定 sleep 误差 |
| 帧延迟与场景切换 | 预览允许跳过过期显示；迟滞不阻止显著变化 |
| 无效媒体、缺字、终端过小、用户中断 | 明确报错或安全退出，恢复光标及颜色 |
| 参数冲突、TXT 自动列数、现存输出文件 | 明确拒绝，不静默降级或覆盖 |
| 透明图像与主题切换 | 合成背景、映射极性与输出主题一致，完全透明块为空格 |
| TXT 字节格式 | 无 BOM/CR/ANSI，每行保留 C 个字符并以 LF 结束 |

以上是未来实现的验收清单，本次尚未运行功能测试。

## 首版之外

彩色输出、空间形状匹配、PNG/MP4 导出、音频同步、GUI、GPU、HDR 色调映射、非等宽字体、网络媒体和磁盘字体缓存均留待后续。

## 项目目录结构

以下为规划结构；目前只有 README 和设计文档，代码目录及构建文件尚未创建。

```text
Asciixel/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── design.md
│   └── design-review.md
├── include/asciixel/
│   ├── model/                      # 普通数据结构，不暴露第三方类型
│   │   ├── image.hpp
│   │   ├── font_profile.hpp
│   │   ├── ascii_frame.hpp
│   │   └── error.hpp
│   ├── core/                       # 可独立测试的核心算法
│   │   ├── grid_layout.hpp
│   │   ├── glyph_analyzer.hpp
│   │   ├── block_sampler.hpp
│   │   ├── tone_mapper.hpp
│   │   ├── glyph_matcher.hpp
│   │   └── temporal_stabilizer.hpp
│   ├── ports/                      # 外部能力接口
│   │   ├── media_source.hpp
│   │   ├── font_rasterizer.hpp
│   │   ├── frame_sink.hpp
│   │   └── clock.hpp
│   └── app/                        # 转换任务与播放流程
│       ├── config.hpp
│       ├── font_profile_builder.hpp
│       ├── frame_converter.hpp
│       ├── playback_scheduler.hpp
│       └── conversion_session.hpp
├── src/
│   ├── core/                       # core 头文件对应的实现
│   ├── app/                        # 编排、缓存与播放状态
│   ├── adapters/
│   │   ├── ffmpeg/
│   │   │   ├── media_source.cpp
│   │   │   ├── decoder.cpp
│   │   │   ├── frame_normalizer.cpp
│   │   │   └── ffmpeg_raii.hpp
│   │   ├── freetype/
│   │   │   ├── font_rasterizer.cpp
│   │   │   └── freetype_raii.hpp
│   │   ├── terminal/
│   │   │   ├── terminal_sink.cpp
│   │   │   └── terminal_session.cpp
│   │   ├── text/
│   │   │   └── text_sink.cpp
│   │   └── system/
│   │       └── steady_clock.cpp
│   └── cli/
│       ├── main.cpp
│       └── arguments.cpp
└── tests/
    ├── core/
    ├── app/
    ├── integration/
    └── fixtures/
```

### 分层职责

| 层 | 职责 | 依赖约束 |
| --- | --- | --- |
| Model | 图像、字形、字体配置、字符帧与错误 | 普通 C++ 类型，无第三方库类型 |
| Core | 网格、覆盖率、分块采样、色调映射、匹配与迟滞 | 依赖 Model；不读文件、不打印、不等待 |
| Ports | 媒体源、字体栅格化、输出和时钟接口 | 使用 Model 传递数据 |
| Adapters | 实现 Ports，封装外部库及平台资源 | FFmpeg、FreeType 和终端细节留在适配器内 |
| Application | 流程编排、字体缓存、调度、取消与错误传播 | 依赖 Core、Ports，不直接调用外部库 |
| CLI | 参数解析、适配器创建与注入、退出码 | 组装 Application 和 Adapters，不实现图像算法 |

FFmpeg 适配器内部保留解码与归一化两个职责，对应用输出统一的 `LinearImage`。字体适配器输出统一布局的字形掩模，由核心算法计算覆盖率。基础匹配无状态，视频历史仅由 `TemporalStabilizer` 保存；调度器控制时间，输出器只负责绘制。

### 构建目标

| CMake Target | 内容 | 依赖 |
| --- | --- | --- |
| `asciixel_core` | Model、Core | C++ 标准库 |
| `asciixel_app` | Ports、Application | `asciixel_core` |
| `asciixel_adapters` | 首版外部适配器 | Core、Ports、FFmpeg、FreeType、平台 API |
| `asciixel` | CLI 可执行程序 | App、Adapters |

算法测试只链接核心库；应用测试使用假时钟和内存媒体源；真实解码与终端恢复放入集成测试。接口头文件与适配器工厂声明可在实现时按需要补充，不要求每个小结构单独建文件。

## 设计文档

- [核心设计与接口约束](docs/design.md)
- [设计审查与修订记录](docs/design-review.md)

实现顺序：字体标定和纯映射算法 → 图片/TXT → 终端 → 视频时间轴与迟滞。依赖版本和构建命令将在实际接入并验证后补充。
