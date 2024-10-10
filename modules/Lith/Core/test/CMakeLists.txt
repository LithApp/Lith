
add_executable(ProtocolTest ProtocolTest.cpp)
add_test(ProtocolTest ProtocolTest)
target_link_libraries(ProtocolTest PRIVATE Qt6::Gui Qt6::Qml Qt6::Test LithCore)
target_include_directories(ProtocolTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_executable(ColorTest ColorTest.cpp)
add_test(ColorTest ColorTest)
target_link_libraries(ColorTest PRIVATE Qt6::Gui Qt6::Qml Qt6::Test LithCore)
target_include_directories(ColorTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_executable(FormattedStringTest FormattedStringTest.cpp)
add_test(FormattedStringTest FormattedStringTest)
target_link_libraries(FormattedStringTest PRIVATE Qt6::Gui Qt6::Qml Qt6::Test LithCore)
target_include_directories(FormattedStringTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
