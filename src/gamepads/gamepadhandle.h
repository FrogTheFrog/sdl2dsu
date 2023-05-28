#pragma once

// system includes
#include <boost/move/core.hpp>
#include <string>

// local includes
#include "SDL.h"
#include "shared/gamepaddata.h"

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
class GamepadHandle final
{
    BOOST_MOVABLE_BUT_NOT_COPYABLE(GamepadHandle)

public:
    explicit GamepadHandle(std::uint32_t id, std::uint8_t index);
    ~GamepadHandle();

    SDL_Gamepad*       getHandle() const;
    std::uint8_t       getIndex() const;
    const std::string& getName() const;
    void               tryChangeSensorState(const std::optional<bool>& enable);

private:
    bool hasSensorSupport() const;
    bool refreshSensorStatus();

    SDL_Gamepad*   m_handle;
    std::uint8_t   m_index;
    std::string    m_name;
    SDL_SensorType m_accel{SDL_SensorType::SDL_SENSOR_INVALID};
    SDL_SensorType m_gyro{SDL_SensorType::SDL_SENSOR_INVALID};
};
}  // namespace gamepads
