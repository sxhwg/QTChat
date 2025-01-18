cmake_minimum_required(VERSION 3.19)

project(GateServer)

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
    GateServer.cpp
    Const.h
    CServer.h
    CServer.cpp
    Singleton.h
    AsioIOServicePool.h
    AsioIOServicePool.cpp
    HttpConnection.h
    HttpConnection.cpp
    LogicSystem.h
    LogicSystem.cpp
    message.pb.h
    message.pb.cc
    message.grpc.pb.h
    message.grpc.pb.cc
    VerifyGrpcClient.h
    VerifyGrpcClient.cpp
    StatusGrpcClient.h
    StatusGrpcClient.cpp
    ConfigMgr.h
    ConfigMgr.cpp
    RedisMgr.h
    RedisMgr.cpp
    MysqlMgr.h
    MysqlMgr.cpp
    MysqlDao.h
    MysqlDao.cpp
)

add_executable(GateServer ${PROJECT_SOURCES})

if(WIN32)
    target_compile_definitions(GateServer PRIVATE _WIN32_WINNT=0x0600)
endif()

target_link_libraries(GateServer PRIVATE
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

set_target_properties(GateServer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_command(TARGET GateServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/config.ini"
    "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET GateServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "/home/xhwg/c++lib/mysql-connector-c++-8.3.0/lib64/libmysqlcppconn8.so.2.8.3.0"
    "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET GateServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "/home/xhwg/c++lib/mysql-connector-c++-8.3.0/lib64/libmysqlcppconn.so.9.8.3.0"
    "${CMAKE_BINARY_DIR}/bin"
)