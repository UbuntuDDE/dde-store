cmake_minimum_required(VERSION 3.0)

project(dde-store)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CCX_STANDARD 11)
set(CMAKE_CCX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 REQUIRED Core Widgets Network DBus Gui LinguistTools)
find_package(Dtk REQUIRED Widget Gui)
find_package(PackageKitQt5 REQUIRED)
find_package(AppStreamQt REQUIRED)
find_package(PkgConfig REQUIRED)
include(FindPkgConfig)
include(GNUInstallDirs)

include_directories(
    ${PackageKitQt5_INCLUDE_DIR}
    ${AppStreamQt_INCLUDE_DIRS}
)

file(GLOB TS_FILES ./translations/*.ts)
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

file(GLOB SRC
    main.cpp
    mainwindow.cpp
    mainwindow.h
    pages/homepage.cpp
    pages/homepage.h
    pages/itempage.cpp
    pages/itempage.h
    pages/categorypage.cpp
    pages/categorypage.h
    pages/updatespage.cpp
    pages/updatespage.h
    backend/sourcemanager.cpp
    backend/sourcemanager.h
    backend/sources/packagekit/packagekitsource.cpp
    backend/sources/packagekit/packagekitsource.h
    backend/ratingshelper.cpp
    backend/ratingshelper.h
    backend/settings.cpp
    backend/settings.h
    widgets/gallery.cpp
    widgets/gallery.h
    widgets/stars.cpp
    widgets/stars.h
    widgets/list.cpp
    widgets/list.h
    resources/resources.qrc
)

add_executable(${PROJECT_NAME}
    ${SRC}
    ${TS_FILES}
    ${QM_FILES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${PackageKitQt5_INCLUDE_DIR}
    ${AppStreamQt_INCLUDE_DIRS}
    ${SNAPD_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    ${DtkWidget_LIBRARIES}
    ${DtkCore_LIBRARIES}
    ${DtkGui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${PackageKitQt5_LIBRARIES}
    ${SNAPD_LIBRARIES}
    AppStreamQt
)

add_subdirectory(backend/sources)

set(CMAKE_INSTALL_PREFIX /usr)

file(GLOB libfiles
    pages/itempage.cpp
    pages/categorypage.cpp
    pages/updatespage.cpp
    plugins/snap/snapinterface.h
)

add_library(libddestore ${libfiles})

target_link_libraries(libddestore PUBLIC
    ${DtkWidget_LIBRARIES}
    ${DtkCore_LIBRARIES}
    ${DtkGui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${PackageKitQt5_LIBRARIES}
    ${SNAPD_LIBRARIES}
    AppStreamQt
)

target_include_directories(libddestore PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS libddestore DESTINATION ${CMAKE_INSTALL_LIBDIR}/libddestore)

install(TARGETS dde-store DESTINATION bin)
install(FILES ${QM_FILES} DESTINATION share/dde-store/translations)
install(FILES resources/dde-store.desktop DESTINATION share/applications)