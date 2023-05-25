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

    SDL_Gamepad* getHandle() const;
    std::uint8_t getIndex() const;
    bool         hasSensorSupport() const;

private:
    SDL_Gamepad* m_handle;
    std::uint8_t m_index;
    std::string  m_name;
    bool         m_sensor_supported{false};
};
}  // namespace gamepads
