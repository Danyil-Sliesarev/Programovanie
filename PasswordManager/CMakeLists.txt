cmake_minimum_required(VERSION 3.16)
project(PasswordManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network Concurrent)

qt_add_executable(PasswordManager
    src/main.cpp
    src/mainwindow.h src/mainwindow.cpp
    src/accountentry.h
    src/databasemanager.h src/databasemanager.cpp
    src/accountrepository.h src/accountrepository.cpp
    src/accounttablemodel.h src/accounttablemodel.cpp
    src/accountfilterproxymodel.h src/accountfilterproxymodel.cpp
    src/passwordleakchecker.h src/passwordleakchecker.cpp
    src/batchchecker.h src/batchchecker.cpp
)

target_link_libraries(PasswordManager PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    Qt6::Concurrent
)
