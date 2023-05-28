// class header include
#include "gamepadhandle.h"

// system includes
#include <boost/log/trivial.hpp>

// local includes

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
namespace
{
std::string getGUIDString(std::uint32_t id)
{
    const auto           guid{SDL_GetGamepadInstanceGUID(id)};
    std::array<char, 33> buffer = {};

    if (SDL_GetJoystickGUIDString(guid, buffer.data(), buffer.size()) < 0)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to get GUID string: " << SDL_GetError();
        return {};
    }

    return std::string(buffer.data());
}
}  // namespace

//--------------------------------------------------------------------------------------------------

GamepadHandle::GamepadHandle(std::uint32_t id, std::uint8_t index)
    : m_handle{SDL_OpenGamepad(id)}
    , m_index{index}
    , m_name{SDL_GetGamepadInstanceName(id)}
{
    if (m_handle)
    {
        BOOST_LOG_TRIVIAL(info) << "Watching gamepad GUID: " << getGUIDString(id) << ", name: " << m_name;
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

const std::string& GamepadHandle::getName() const
{
    return m_name;
}

//--------------------------------------------------------------------------------------------------

void GamepadHandle::tryChangeSensorState(const std::optional<bool>& enable)
{
    if (!hasSensorSupport() && !refreshSensorStatus())
    {
        return;
    }

    const auto to_sdl_bool = [](bool value) { return value ? SDL_TRUE : SDL_FALSE; };

    const bool accel_enabled{SDL_GamepadSensorEnabled(m_handle, m_accel) == SDL_TRUE};
    const bool gyro_enabled{SDL_GamepadSensorEnabled(m_handle, m_gyro) == SDL_TRUE};

    const bool current_state{accel_enabled && gyro_enabled};
    const bool new_state{enable ? *enable : !current_state /* toggle */};

    if (new_state != current_state)
    {
        BOOST_LOG_TRIVIAL(info) << "Changing accel and gyro state to " << (new_state ? "ENABLED" : "DISABLED")
                                << " for " << m_name;
        if (SDL_SetGamepadSensorEnabled(m_handle, m_accel, to_sdl_bool(new_state)) < 0)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to change accel state: " << SDL_GetError();
        }
        if (SDL_SetGamepadSensorEnabled(m_handle, m_gyro, to_sdl_bool(new_state)) < 0)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to change gyro state: " << SDL_GetError();
        }
    }
}

//--------------------------------------------------------------------------------------------------

bool GamepadHandle::hasSensorSupport() const
{
    return m_accel != SDL_SensorType::SDL_SENSOR_INVALID && m_gyro != SDL_SensorType::SDL_SENSOR_INVALID;
}

//--------------------------------------------------------------------------------------------------

bool GamepadHandle::refreshSensorStatus()
{
    if (SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_ACCEL))
    {
        m_accel = SDL_SensorType::SDL_SENSOR_ACCEL;
    }
    else if (SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_ACCEL_L))
    {
        m_accel = SDL_SensorType::SDL_SENSOR_ACCEL_L;
    }
    else if (SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_ACCEL_R))
    {
        m_accel = SDL_SensorType::SDL_SENSOR_ACCEL_R;
    }

    if (SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_GYRO))
    {
        m_gyro = SDL_SensorType::SDL_SENSOR_GYRO;
    }
    else if (SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_GYRO_L))
    {
        m_gyro = SDL_SensorType::SDL_SENSOR_GYRO_L;
    }
    else if (SDL_GamepadHasSensor(m_handle, SDL_SensorType::SDL_SENSOR_GYRO_R))
    {
        m_gyro = SDL_SensorType::SDL_SENSOR_GYRO_R;
    }

    return hasSensorSupport();
}
}  // namespace gamepads
