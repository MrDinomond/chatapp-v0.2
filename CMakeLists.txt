cmake_minimum_required(VERSION 3.16)
project(ChatApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt/6.9.0")

find_package(Qt6 COMPONENTS Core Widgets Sql REQUIRED)

# Добавляем исходные файлы
set(SOURCES
    src/main.cpp
    src/core/User.cpp
    src/core/Message.cpp
    src/core/Chat.cpp
    src/utils/DatabaseManager.cpp
    src/ui/MainWindow.cpp
    src/ui/LoginWidget.cpp
    src/ui/RegistrationWidget.cpp
    src/ui/ChatWidget.cpp
    src/ui/MainMenuWidget.cpp
)

# Добавляем заголовочные файлы
set(HEADERS
    include/core/User.h
    include/core/Message.h
    include/core/Chat.h
    include/utils/DatabaseManager.h
    include/ui/MainWindow.h
    include/ui/LoginWidget.h
    include/ui/RegistrationWidget.h
    include/ui/ChatWidget.h
    include/ui/MainMenuWidget.h
)

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Подключаем Qt модули
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)

# Устанавливаем пути для заголовочных файлов
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Устанавливаем выходную директорию
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
) 