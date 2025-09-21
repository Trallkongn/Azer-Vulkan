project(SandBox LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    "src/SandBox.cpp"
    "src/NewLayer.h" 
    "src/NewLayer.cpp"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    AZ_PLATFORM_WINDOWS
)

target_link_libraries(${PROJECT_NAME} PRIVATE Azer spdlog::spdlog glm::glm)