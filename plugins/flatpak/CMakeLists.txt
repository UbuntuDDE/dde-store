add_library(flatpakplugin MODULE flatpakplugin.cpp)
target_link_libraries(flatpakplugin libddestore Qt5::Core Qt5::Network ${DtkWidget_LIBRARIES} ${DtkCore_LIBRARIES} PkgConfig::Flatpak)
get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
    OUTPUT_VARIABLE QT_INSTALL_PLUGINS OUTPUT_STRIP_TRAILING_WHITESPACE
)
install(TARGETS flatpakplugin DESTINATION ${QT_INSTALL_PLUGINS}/dde-store)