# QTChat概述

## 一、项目简介

全栈即时通讯项目，客户端基于QT，服务端基于C++。  
参考的恋恋风辰老师的开源项目：[项目链接](https://gitee.com/secondtonone1/llfcchat)  
目前在该项目的基础上，将客户端的qmake构建改为CMake构建，原本的100+源文件被划分到多个子目录中，实现模块化管理；将服务端的MSBuild构建改为CMake构建，搭配vcpkg实现良好的项目依赖管理，并从Windows平台移植到Linux平台。

## 二、环境需求

客户端(Windows)：QT Creator  
服务端(Linux)：Visual Studio Code, CMake, Git, vcpkg, mysql-connector-c++, Node.js

实际上客户端和服务端都可以在Windows和Linux上构建，只需要配置相应的环境和修改部分构建相关文件。

## 三、环境搭建

### 客户端

QT Creator的环境搭建比较简单，略。

### 服务端

主要介绍vcpkg的环境搭建及第三方库安装方式，其他环境的搭建也比较简单。  
vcpkg搭配CMake在能够实现较好的包管理功能，但网上资料不够详细，甚至问AI都会踩很多坑，所以分享下我的解决方案。  
本项目中，服务端除了mysql-connector-c++这个库vcpkg安装出错，必须手动安装外，其他第三方库均采用vcpkg安装。

1. 克隆vcpkg仓库。  
`git clone https://github.com/microsoft/vcpkg.git`

2. 切换到vcpkg目录，执行Linux对应的脚本文件。  
`./bootstrap-vcpkg.sh`

3. 将vcpkg命令永久配置到环境变量，我使用的ubuntu-22.04.5，采用以下命令：  
打开文件 `nano ~/.bashrc`  
在文件末尾添加 `export PATH="$PATH:/vcpkg文件夹"`  
关闭文件后，执行 `source ~/.bashrc`

4. 在vcpkg文件夹中创建vcpkg_installed文件夹，这个文件夹接下来会用到。

5. 使用vcpkg的清单模式。  
切换到项目目录，执行命令 `vcpkg new --application`  
会生成两个vcpkg的配置文件：vcpkg.json和vcpkg-configuration.json  
然后复制本项目对应文件的内容，也可以直接替换文件  

6. 创建两个CMake的配置文件：CMakePresets.json和CMakeUserPresets.json。  
然后复制本项目对应文件的内容，也可以直接替换文件。  
修改CMakeUserPresets.json中所有VCPKG_ROOT对应的值，改为你的vcpkg文件夹路径。  

    ```json
    "environment": {
                "VCPKG_ROOT": "vcpkg文件夹路径"
            }
    ```

7. 如果使用经典模式，不方便管理第三方库的版本，项目依赖也不清晰，直接使用清单模式可以避免上述问题，但每个项目构建目录中都会有一个vcpkg_installed文件夹用于存放第三方库，而本项目有多个服务器项目，且需要的第三方库较大，非常浪费空间。  
为了解决上述问题，需要在CMakeUserPresets.json配置缓存变量，值为第4步中指定的vcpkg_installed文件的路径。  
前面已经复制了该文件，并修改过VCPKG_ROOT的值，这里不需要修改，但是vcpkg_installed文件夹如果没有指定到vcpkg文件夹，则需要修改。

    ```json
    "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
        "VCPKG_HOST_TRIPLET": "x64-linux",
        "VCPKG_TARGET_TRIPLET": "x64-linux",
        "VCPKG_INSTALLED_DIR": "$env{VCPKG_ROOT}/vcpkg_installed"
      }
    ```

8. 写好CMakeLists.txt文件后，使用CMake构建时就会自动先使用vcpkg安装第三方库。

9. 额外说下，如果是在Windows上，CMakeUserPresets.json的缓存变量部分需要修改为以下这样：

    ```json
    "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
        "VCPKG_HOST_TRIPLET": "x64-windows-static-md",
        "VCPKG_TARGET_TRIPLET": "x64-windows-static-md",
        "VCPKG_INSTALLED_DIR": "$env{VCPKG_ROOT}/vcpkg_installed"
      }
    ```

10. vcpkg还有些使用细节上面没有提到，比如清单模式下怎么控制版本和仅安装部分boost库，以及大量本项目没有用到的功能，这些可以参考官方文档：[文档链接](https://learn.microsoft.com/zh-cn/vcpkg/)
