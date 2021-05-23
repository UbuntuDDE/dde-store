add_library(flatpakplugin MODULE flatpaksource.cpp ../../sourcemanager.h)
target_link_libraries(flatpakplugin Qt5::Core Qt5::Widgets PkgConfig::Flatpak)
get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
    OUTPUT_VARIABLE QT_INSTALL_PLUGINS OUTPUT_STRIP_TRAILING_WHITESPACE
)
install(TARGETS flatpakplugin DESTINATION ${QT_INSTALL_PLUGINS}/dde-store)