#chat子目录
file(GLOB_RECURSE CHAT_SOURCES CONFIGURE_DEPENDS "*.h" "*.cpp" "*.ui")

add_library(Chat OBJECT ${CHAT_SOURCES})
target_include_directories(Chat PRIVATE include)
target_link_libraries(Chat PRIVATE QtModules CommonInterface LoginInterface)

add_library(ChatInterface INTERFACE)
target_include_directories(ChatInterface INTERFACE include)
