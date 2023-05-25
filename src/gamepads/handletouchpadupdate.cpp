// class header include
#include "handleaxisupdate.h"

// system includes

// local includes

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
std::optional<std::uint8_t> handleTouchpadUpdate(const SDL_GamepadTouchpadEvent& event, GamepadManager&)
{
    // TODO
    BOOST_LOG_TRIVIAL(trace) << "touchpad (" << event.touchpad << ") x:" << event.x << " y:" << event.y
                             << " pr:" << event.pressure << " fr:" << event.finger << " received for gamepad "
                             << event.which;
    return std::nullopt;
}
}  // namespace gamepads
