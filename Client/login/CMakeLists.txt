#login子目录
file(GLOB_RECURSE LOGIN_SOURCES CONFIGURE_DEPENDS "*.h" "*.cpp" "*.ui")

add_library(Login OBJECT ${LOGIN_SOURCES})
target_include_directories(Login PRIVATE include)
target_link_libraries(Login PRIVATE QtModules CommonInterface)

add_library(LoginInterface INTERFACE)
target_include_directories(LoginInterface INTERFACE include)
