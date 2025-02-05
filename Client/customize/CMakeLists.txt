#customize子目录
file(GLOB_RECURSE CUSTOMIZE_SOURCES CONFIGURE_DEPENDS "*.h" "*.cpp" "*.ui")

add_library(Customize OBJECT ${CUSTOMIZE_SOURCES})
target_include_directories(Customize PRIVATE include)
target_link_libraries(Customize PRIVATE QtModules CommonInterface)

add_library(CustomizeInterface INTERFACE)
target_include_directories(CustomizeInterface INTERFACE include)
