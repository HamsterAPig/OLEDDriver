project(OLEDDrive
        LANGUAGES C)
file(GLOB_RECURSE SOURCES "*.c")

add_compile_definitions(__USING_SSD1306)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
        PRIVATE
        ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})