cmake_minimum_required(VERSION 3.20)
project(price_watcher CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Drogon CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_path(TGBOT_INCLUDE_DIR NAMES tgbot/tgbot.h)
find_library(TGBOT_LIBRARY NAMES TgBot tgbot tgbot-cpp)
find_path(GUMBO_INCLUDE_DIR NAMES gumbo.h)
find_library(GUMBO_LIBRARY NAMES gumbo)
aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(services SERVICES_SRC)

add_executable(${PROJECT_NAME} main.cc)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${GUMBO_INCLUDE_DIR}
    ${TGBOT_INCLUDE_DIR}
)
target_sources(${PROJECT_NAME} PRIVATE
    ${CTL_SRC}
    ${FILTER_SRC}
    ${PLUGIN_SRC}
    ${MODEL_SRC}
    ${SERVICES_SRC}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Drogon::Drogon
    nlohmann_json::nlohmann_json
    ${TGBOT_LIBRARY}
    ${GUMBO_LIBRARY}
)
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Drogon::Drogon>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()