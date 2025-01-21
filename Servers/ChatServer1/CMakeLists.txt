cmake_minimum_required(VERSION 3.19)

project(ChatServer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(mysql-concpp_DIR /home/xhwg/c++lib/mysql-connector-c++-8.3.0)

find_package(mysql-concpp CONFIG REQUIRED)
find_package(boost_beast CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(boost_uuid CONFIG REQUIRED)
find_package(boost_property_tree CONFIG REQUIRED)
find_package(boost_filesystem REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(hiredis CONFIG REQUIRED)

set(PROJECT_SOURCES
    ChatServer.cpp
    AsioIOServicePool.h
    AsioIOServicePool.cpp
    ChatGrpcClient.h
    ChatGrpcClient.cpp
    ChatServiceImpl.h
    ChatServiceImpl.cpp
    ConfigMgr.h
    ConfigMgr.cpp
    Const.h
    CServer.h
    CServer.cpp
    CSession.h
    CSession.cpp
    Data.h
    LogicSystem.h
    LogicSystem.cpp
    message.grpc.pb.h
    message.grpc.pb.cc
    message.pb.h
    message.pb.cc
    MsgNode.h
    MsgNode.cpp
    MysqlDao.h
    MysqlDao.cpp
    MysqlMgr.h
    MysqlMgr.cpp
    RedisMgr.h
    RedisMgr.cpp
    Singleton.h
    UserMgr.h
    UserMgr.cpp
)

add_executable(ChatServer ${PROJECT_SOURCES})

if(WIN32)
    target_compile_definitions(ChatServer PRIVATE _WIN32_WINNT=0x0600)
endif()

target_link_libraries(ChatServer PRIVATE
    Boost::beast
    Boost::asio
    Boost::uuid
    Boost::property_tree
    Boost::filesystem
    JsonCpp::JsonCpp
    gRPC::gpr
    gRPC::grpc
    gRPC::grpc++
    gRPC::grpc++_alts
    hiredis::hiredis
    mysql::concpp-jdbc
)

set_target_properties(ChatServer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_command(TARGET ChatServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/config.ini"
    "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET ChatServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "/home/xhwg/c++lib/mysql-connector-c++-8.3.0/lib64/libmysqlcppconn8.so.2.8.3.0"
    "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET ChatServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "/home/xhwg/c++lib/mysql-connector-c++-8.3.0/lib64/libmysqlcppconn.so.9.8.3.0"
    "${CMAKE_BINARY_DIR}/bin"
)