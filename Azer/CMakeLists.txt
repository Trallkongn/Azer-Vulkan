project(Azer LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    "src/Azer/Core/EntryPoint.h"
    "src/Azer/Core/Application.h"
    "src/Azer/Core/Application.cpp"
    "src/azpch.h"
    "src/azpch.cpp"
    "src/Azer/Core/Window.h" 
    "src/Azer/Events/Event.h" 
    "src/Azer/Core/Core.h" 
    "src/Azer/Events/ApplicationEvent.h" 
    "src/Azer/Events/KeyEvent.h" 
    "src/Azer/Events/MouseEvent.h"
    "src/Azer/Core/Log.h" 
    "src/Azer/Core/Log.cpp"
    "src/Platform/Windows/WindowsWindow.h"
    "src/Platform/Windows/WindowsWindow.cpp"
)

add_subdirectory(vendor/glfw)
find_package(Vulkan REQUIRED)

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/azpch.h>"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    AZ_PLATFORM_WINDOWS
    AZ_ENABLE_ASSERTS
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan spdlog::spdlog)