cmake_minimum_required(VERSION 3.16)

project(Algorithms VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Charts Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts Widgets)

set(PROJECT_SOURCES
        main.cpp
        widgets/main_window.cpp
        widgets/main_window.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Algorithms
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        widgets/algorithm_visualization_window.h  widgets/algorithm_visualization_window.cpp
        icons.qrc
        core/algorithm.cpp core/algorithm.h core/algorithm_visualizer.cpp core/algorithm_visualizer.h core/widgets/data_structure_widget.cpp core/widgets/data_structure_widget.h
        graphs/edge_list.cpp graphs/edge_list.h graphs/graph.cpp graphs/graph.h graphs/graph_algorithms.cpp graphs/graph_algorithms.h graphs/graph_algorithm_visualizers.cpp graphs/graph_algorithm_visualizers.h
        graphs/widgets/graph_widget.h graphs/widgets/graph_widget.cpp
        graphs/graph_builders.cpp graphs/graph_builders.h
        graphs/graph_visual_builders.h graphs/graph_visual_builders.cpp

        graphs/widgets/random_graph_properties_dialog.h graphs/widgets/random_graph_properties_dialog.cpp
        widgets/algorithm_benchmark_window.h widgets/algorithm_benchmark_window.cpp
        core/data_structure_builder.h core/data_structure_builder.cpp
        core/data_structure.h core/data_structure.cpp
        core/property_editor_factory.cpp core/property_editor_factory.h
        core/data_structure_visual_builder.h core/data_structure_visual_builder.cpp
        core/widgets/properties_dialog.h core/widgets/properties_dialog.cpp
        graphs/widgets/add_edge_dialog.h graphs/widgets/add_edge_dialog.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Algorithms APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Algorithms SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Algorithms
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Algorithms PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Algorithms PRIVATE Qt${QT_VERSION_MAJOR}::Charts)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Algorithms)
endif()
set_target_properties(Algorithms PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Algorithms
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Algorithms)
endif()
