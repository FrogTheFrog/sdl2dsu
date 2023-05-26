// class header include
#include "handleaxisupdate.h"

// system includes
#include <limits>

// local includes

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
namespace
{
bool tryModifyState(std::uint16_t& from, float to)
{
    const auto to_with_range_converted{static_cast<std::uint16_t>(to * std::numeric_limits<std::uint16_t>::max())};
    if (from == to_with_range_converted)
    {
        return false;
    }

    from = to_with_range_converted;
    return true;
}

//--------------------------------------------------------------------------------------------------

bool tryModifyState(bool& from, float to)
{
    const bool is_touching{to > 0};
    if (from == is_touching)
    {
        return false;
    }

    from = is_touching;
    return true;
}
}  // namespace

//--------------------------------------------------------------------------------------------------

std::optional<std::uint8_t> handleTouchpadUpdate(const SDL_GamepadTouchpadEvent& event, GamepadManager& manager)
{
    if (event.touchpad == 0 && (event.finger == 0 || event.finger == 1))
    {
        return manager.tryUpdateData(event.which,
                                     [&event](auto& data)
                                     {
                                         shared::details::Touch& touch{event.finger == 0
                                                                           ? data.m_touchpad.m_first_touch
                                                                           : data.m_touchpad.m_second_touch};

                                         bool updated{tryModifyState(touch.m_touched, event.pressure)};
                                         if (updated && touch.m_touched)
                                         {
                                             // Increment the touch "id" (counter) once the finger is touching.
                                             // Overflows are ok.
                                             touch.m_id++;
                                         }
                                         updated = tryModifyState(touch.m_x, event.x) || updated;
                                         updated = tryModifyState(touch.m_y, event.y) || updated;
                                         return updated;
                                     });
    }

    BOOST_LOG_TRIVIAL(trace) << "touchpad (" << event.touchpad << ") x:" << event.x << " y:" << event.y
                             << " pr:" << event.pressure << " fr:" << event.finger << " received for gamepad "
                             << event.which;
    return std::nullopt;
}
}  // namespace gamepads
