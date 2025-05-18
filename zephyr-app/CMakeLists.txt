cmake_minimum_required(VERSION 3.20.0)

set(BOARD native_sim)
#set(CONF_FILE prj.conf)

find_package(Zephyr)

project(ng28)



target_sources(app PRIVATE 
                           # Drivers and such
                           src/display-sim.cpp

                           # "OS" logic
                           src/main.cpp
                           src/viewmanager.cpp

                           # Graphical views
                           src/view.cpp
                           src/view-home.cpp
                       )
