cmake_minimum_required(VERSION 3.16)

project(QuickTodo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force MSVC to treat source/execution charset as UTF-8 to avoid mojibake in build pipeline.
if (MSVC)
    add_compile_options(/utf-8)
    add_compile_options(/EHsc)
endif ()

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Sql)

# third party libraries via Vcpkg
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(${PROJECT_NAME}
        main.cpp
        src/model/AppModel.cpp
        src/model/task/Task.cpp
        src/model/repository/TaskRepository.cpp
        src/viewmodel/MainViewModel.cpp
        src/viewmodel/TaskListViewModel.cpp
        src/viewmodel/TaskActionViewModel.cpp
        src/viewmodel/GlobalStateViewModel.cpp
        src/app/AppContainer.cpp
        src/infrastructure/database/DatabaseManager.cpp
        src/infrastructure/logging/Logging.cpp
)

# Keep the QML entry list centralized so module packaging and deployment stay in sync.
set(QUICKTODO_QML_FILES
        src/view/Main.qml
        src/view/pages/TaskPage.qml
        src/view/components/TaskList.qml
        src/view/components/TaskItemDelegate.qml
        src/view/components/GlobalErrorDialog.qml
)

# Register the QML module explicitly so Qt can bundle these files into the target resources.
qt_add_qml_module(${PROJECT_NAME}
        URI quick_todo
        QML_FILES
        ${QUICKTODO_QML_FILES}
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(${PROJECT_NAME} PROPERTIES
        #    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt6::Quick
        Qt6::QuickControls2
        Qt6::Sql
        fmt::fmt-header-only
        spdlog::spdlog_header_only
)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (WIN32)
    # Deploy Qt runtime + QML plugins (e.g. QtQuick.Controls) next to exe for IDE run.
    get_target_property(_qt_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qt_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if (WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --qmldir "${CMAKE_CURRENT_SOURCE_DIR}"
                "$<TARGET_FILE:${PROJECT_NAME}>"
                COMMENT "Running windeployqt for ${PROJECT_NAME}"
                VERBATIM
        )
    else ()
        message(WARNING "windeployqt not found; Qt runtime/QML plugins may be missing at runtime.")
    endif ()
endif ()

install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
