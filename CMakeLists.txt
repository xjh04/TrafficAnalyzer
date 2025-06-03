cmake_minimum_required(VERSION 3.29)
project(QT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置 Qt5 路径
if(WIN32)
    # 你的 Qt5 安装路径
    set(CMAKE_PREFIX_PATH "D:/QT/install/5.14.2/mingw73_64")
elseif(APPLE)
    # macOS 路径示例
#    set(CMAKE_PREFIX_PATH "/usr/local/opt/qt5" "/Applications/Qt/5.15.2/clang_64")
else()
    # Linux 路径示例
#    set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt5" "/opt/Qt/5.15.2/gcc_64")
endif()

# 查找 Qt5 包
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

# 检查是否找到 Qt5
if(NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 not found. Please install Qt5 or set CMAKE_PREFIX_PATH correctly.")
endif()

# 显示找到的 Qt5 版本
message(STATUS "Found Qt5 version: ${Qt5_VERSION}")

add_executable(QT main.cpp
        LoginWidget.cpp
        MainWindow.cpp
        RegisterWidget.cpp
        TrafficAnalyzerWidget.cpp)

target_link_libraries(QT
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

# Windows 特定的 DLL 复制逻辑
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()

    # 获取 Qt5 安装路径
    get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
    set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

    if(EXISTS ${QT5_WINDEPLOYQT_EXECUTABLE})
        # 使用 windeployqt 自动部署 Qt 依赖
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}>
                COMMENT "Deploying Qt libraries")
    else()
        # 手动复制 DLL 的备用方案
        set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
            if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
                set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
            endif ()
        endif ()

        # 复制平台插件
        if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        endif ()

        # 复制核心 DLL
        foreach (QT_LIB Core Gui Widgets)
            if(EXISTS "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll")
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy
                        "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                        "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
            endif()
        endforeach (QT_LIB)
    endif()
endif ()