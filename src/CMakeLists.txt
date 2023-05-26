
# find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
# qt_standard_project_setup()

#----------------------------------------------------------------------------------------------------------------------
# Header files
#----------------------------------------------------------------------------------------------------------------------

set(HEADERS
    gamepads/enumerator.h
    gamepads/gamepadhandle.h
    gamepads/gamepadmanager.h
    gamepads/handleaxisupdate.h
    gamepads/handlebatteryupdate.h
    gamepads/handlebuttonupdate.h
    gamepads/handlesensorupdate.h
    gamepads/handletouchpadupdate.h
    server/activeclients.h
    server/clientendpoint.h
    server/clientendpointcounter.h
    server/common.h
    server/communication.h
    server/deserialiser.h
    server/serialiser.h
    shared/gamepaddata.h
    )

#----------------------------------------------------------------------------------------------------------------------
# Source files
#----------------------------------------------------------------------------------------------------------------------

set(SOURCES
    gamepads/enumerator.cpp
    gamepads/gamepadhandle.cpp
    gamepads/gamepadmanager.cpp
    gamepads/handleaxisupdate.cpp
    gamepads/handlebatteryupdate.cpp
    gamepads/handlebuttonupdate.cpp
    gamepads/handlesensorupdate.cpp
    gamepads/handletouchpadupdate.cpp
    server/activeclients.cpp
    server/clientendpoint.cpp
    server/clientendpointcounter.cpp
    server/common.cpp
    server/communication.cpp
    server/deserialiser.cpp
    server/serialiser.cpp
    )

#----------------------------------------------------------------------------------------------------------------------
# Target config
#----------------------------------------------------------------------------------------------------------------------

add_executable(${PROJECT_NAME} main.cpp ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static ${Boost_LIBRARIES})
