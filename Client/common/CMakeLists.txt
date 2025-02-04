#common子目录
file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS "*.h" "*.cpp" "*.ui")

add_library(Common OBJECT ${COMMON_SOURCES})
target_include_directories(Common PRIVATE include)
target_link_libraries(Common PRIVATE QtModules)

add_library(CommonInterface INTERFACE)
target_include_directories(CommonInterface INTERFACE include)
