cmake_minimum_required(VERSION 3.16)
project(MathTrainer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

set(SOURCES
    src/main.cpp

    src/data/datamodels.h
    src/data/sessionrepository.h
    src/data/sessionrepository.cpp
    src/data/settingsmanager.h
    src/data/settingsmanager.cpp

    src/logic/examplegenerator.h
    src/logic/examplegenerator.cpp
    src/logic/answerchecker.h
    src/logic/answerchecker.cpp
    src/logic/trainingsession.h
    src/logic/trainingsession.cpp
    src/logic/statscalculator.h
    src/logic/statscalculator.cpp

    src/ui/mainwindow.h
    src/ui/mainwindow.cpp
    src/ui/mainmenuwidget.h
    src/ui/mainmenuwidget.cpp
    src/ui/trainingwidget.h
    src/ui/trainingwidget.cpp
    src/ui/resultswidget.h
    src/ui/resultswidget.cpp
    src/ui/historywidget.h
    src/ui/historywidget.cpp
)

add_executable(MathTrainer ${SOURCES})

target_link_libraries(MathTrainer PRIVATE Qt6::Widgets Qt6::Core)

target_include_directories(MathTrainer PRIVATE src)
