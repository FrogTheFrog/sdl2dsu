// class header include
#include "gamepadhandle.h"

// system includes
#include <boost/log/trivial.hpp>

// local includes

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
GamepadHandle::GamepadHandle(std::uint32_t id, std::uint8_t index)
    : m_handle{SDL_OpenGamepad(id)}
    , m_index{index}
    , m_name{SDL_GetGamepadInstanceName(id)}
{
    if (m_handle)
    {
        BOOST_LOG_TRIVIAL(info) << "Watching gamepad (id " << id << "): " << m_name;
        m_sensor_supported = (SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_ACCEL)
                              || SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_ACCEL_L)
                              || SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_ACCEL_R))
                             && (SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_GYRO)
                                 || SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_GYRO_L)
                                 || SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_GYRO_R));
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "Failed to start watching gamepad: " << m_name << ". Error: " << SDL_GetError();
    }
}

//--------------------------------------------------------------------------------------------------

GamepadHandle::~GamepadHandle()
{
    SDL_CloseGamepad(m_handle);
    if (m_handle)
    {
        BOOST_LOG_TRIVIAL(info) << "Stopped watching gamepad: " << m_name;
    }
}

//--------------------------------------------------------------------------------------------------

SDL_Gamepad* GamepadHandle::getHandle() const
{
    return m_handle;
}

//--------------------------------------------------------------------------------------------------

std::uint8_t GamepadHandle::getIndex() const
{
    return m_index;
}

//--------------------------------------------------------------------------------------------------

bool GamepadHandle::hasSensorSupport() const
{
    return m_sensor_supported;
}
}  // namespace gamepads
