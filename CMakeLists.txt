cmake_minimum_required(VERSION 3.16)

project(underlying-test-platform VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force MSVC to treat source/execution charset as UTF-8 to avoid mojibake in build pipeline.
if(MSVC)
    add_compile_options(/utf-8)
endif()

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
        src/app/AppContainer.cpp
        src/infrastructure/database/DatabaseManager.cpp
        src/infrastructure/logging/Logging.cpp
)

qt_add_qml_module(${PROJECT_NAME}
        URI underlying_test_platform
        QML_FILES
        Main.qml
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

install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
