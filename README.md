# Slash

Slash 是基于 Unreal Engine 5.4 的 C++ 项目。这个仓库包含项目源码、配置和 Visual Studio Tools 插件源码。

本仓库不包含 `Content/` 和 `Assets/`。现有项目中的地图、蓝图和其他资源需要从有权访问的项目副本还原到原路径。`Config/DefaultEngine.ini` 中启用的默认地图是 `/Game/Maps/SlashOpenMaps`；没有这些资源时，克隆后无法直接运行当前关卡。

在 Windows 上准备 Unreal Engine 5.4 和 Visual Studio C++ 工具链。资源还原后，右键点击 `Slash.uproject` 并生成 Visual Studio 项目文件，再使用 `Development Editor | Win64` 配置编译 `SlashEditor`，然后打开 `Slash.uproject`。
