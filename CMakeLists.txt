# Project Needs a name ofcourse
project(remote_temp)

# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

# We add our source code here
set(remote_temp_SRCS remote_temp.cpp)

# Now make sure all files get to the right place
kde4_add_plugin(remote_temp ${remote_temp_SRCS})
target_link_libraries(remote_temp
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS remote_temp
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES remote_temp.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
