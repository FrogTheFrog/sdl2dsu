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

set(RESOURCES)
if(WIN32)
    list(APPEND RESOURCES "../resources/windows.rc")
endif()

add_executable(${PROJECT_NAME} main.cpp ${HEADERS} ${SOURCES} ${RESOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static ${Boost_LIBRARIES})

#----------------------------------------------------------------------------------------------------------------------
# Install config
#----------------------------------------------------------------------------------------------------------------------

if(UNIX)
    install(
        TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
    )

    install(
        FILES ../resources/${PROJECT_NAME}.desktop
        DESTINATION share/applications/
    )

    foreach(size 16;32;64;128;256)
        install(
            FILES ../resources/${PROJECT_NAME}-${size}.png
            DESTINATION share/icons/hicolor/${size}x${size}/apps/
            RENAME ${PROJECT_NAME}.png
        )
    endforeach()    
elseif(NOT WIN32)
    message(FATAL_ERROR "OS is not supported!")
endif()
