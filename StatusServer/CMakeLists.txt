cmake_minimum_required(VERSION 3.19)

project(StatusServer)

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
    StatusServer.cpp
    ConfigMgr.h
    ConfigMgr.cpp
    Const.h
    RedisMgr.h
    RedisMgr.cpp
    Singleton.h
    StatusServiceImpl.cpp
    StatusServiceImpl.h
    message.grpc.pb.h
    message.grpc.pb.cc
    message.pb.h
    message.pb.cc
)

add_executable(StatusServer ${PROJECT_SOURCES})

if(WIN32)
    target_compile_definitions(StatusServer PRIVATE _WIN32_WINNT=0x0600)
endif()

target_link_libraries(StatusServer PRIVATE
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

set_target_properties(StatusServer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_command(TARGET StatusServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/config.ini"
    "${CMAKE_BINARY_DIR}/bin"
)
