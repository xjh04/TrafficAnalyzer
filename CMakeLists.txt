cmake_minimum_required(VERSION 3.16)
project(NetworkTrafficAnalyzer VERSION 2.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt5组件
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

# 自动处理MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 源文件
set(SOURCES
    main.cpp
    MainWindow.cpp
    LoginWidget.cpp
    RegisterWidget.cpp
    TrafficAnalyzerWidget.cpp
    SettingsWidget.cpp
)

# 头文件
set(HEADERS
    MainWindow.h
    LoginWidget.h
    RegisterWidget.h
    TrafficAnalyzerWidget.h
    SettingsWidget.h
)

# 创建可执行文件
add_executable(NetworkTrafficAnalyzer
    ${SOURCES}
    ${HEADERS}
)

# 链接Qt库
target_link_libraries(NetworkTrafficAnalyzer
    Qt5::Core
    Qt5::Widgets
)

# 设置编译器特定选项
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(NetworkTrafficAnalyzer PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

if(MSVC)
    target_compile_options(NetworkTrafficAnalyzer PRIVATE
        /W4
        /utf-8
    )
    set_property(TARGET NetworkTrafficAnalyzer PROPERTY
        WIN32_EXECUTABLE TRUE
    )
endif()

# 安装规则
install(TARGETS NetworkTrafficAnalyzer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
