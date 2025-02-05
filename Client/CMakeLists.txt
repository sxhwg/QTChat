#CMake最低版本
cmake_minimum_required(VERSION 3.16)

#项目信息:名称 版本 语言
project(QTChat VERSION 0.1 LANGUAGES CXX)

#设置自动处理ui moc qrc
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#设置项目使用的C++版本，要求编译器必须支持
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#查找QT库的指定模块
find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

# 创建一个接口库来封装Qt依赖
add_library(QtModules INTERFACE)
target_link_libraries(QtModules INTERFACE
    Qt5::Widgets
    Qt5::Network
)

#添加子目录
add_subdirectory(common)
add_subdirectory(customize)
add_subdirectory(chat)
add_subdirectory(login)

#主目录源文件
set(MAIN_SOURCES main.cpp)

#生成可执行文件
add_executable(QTChat ${MAIN_SOURCES} res.qrc)

#链接接口库
target_link_libraries(QTChat PRIVATE
    QtModules
    CommonInterface
    CustomizeInterface
    LoginInterface
    ChatInterface
)
#链接对象库
target_link_libraries(QTChat PRIVATE
    Common
    Customize
    Login
    Chat
)

#设置为Windows上的GUI应用程序
#设置可执行文件输出目录
set_target_properties(QTChat PROPERTIES
    WIN32_EXECUTABLE TRUE
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

#构建完成后复制配置文件到可执行文件输出目录
add_custom_command(TARGET QTChat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/config.ini"
    "${CMAKE_BINARY_DIR}/bin"
)
