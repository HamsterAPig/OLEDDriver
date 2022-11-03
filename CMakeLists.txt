project(OLEDDrive
        LANGUAGES C)
file(GLOB_RECURSE SOURCES "*.c")

# 启用DMA支持
add_compile_definitions(__ENABLE_DMA_OLED)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
        PRIVATE
        ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})