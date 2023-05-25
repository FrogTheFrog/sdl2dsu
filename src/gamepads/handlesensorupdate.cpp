// class header include
#include "handlesensorupdate.h"

// system includes
#include <cmath>

// local includes

//--------------------------------------------------------------------------------------------------

namespace gamepads
{
namespace
{
bool absoluteToleranceCompare(float x, float y)
{
    return std::fabs(x - y) <= std::numeric_limits<float>::epsilon();
}

//--------------------------------------------------------------------------------------------------

bool tryModifyState(std::uint64_t& from, std::uint64_t to)
{
    if (from == to)
    {
        return false;
    }

    from = to;
    return true;
}

//--------------------------------------------------------------------------------------------------

bool tryModifyState(float& from, float to)
{
    if (absoluteToleranceCompare(from, to))
    {
        return false;
    }

    from = to;
    return true;
}
}  // namespace

//--------------------------------------------------------------------------------------------------

std::optional<std::uint8_t> handleSensorUpdate(const SDL_GamepadSensorEvent& event, GamepadManager& manager)
{
    switch (event.sensor)
    {
        case SDL_SensorType::SDL_SENSOR_ACCEL:
        case SDL_SensorType::SDL_SENSOR_ACCEL_L:
        case SDL_SensorType::SDL_SENSOR_ACCEL_R:
            return manager.tryUpdateData(
                event.which,
                [&event](auto& data)
                {
                    bool updated{false};
                    updated = tryModifyState(data.m_sensor.m_ts, event.sensor_timestamp);
                    updated = tryModifyState(data.m_sensor.m_accel.m_x, event.data[0]) || updated;
                    updated = tryModifyState(data.m_sensor.m_accel.m_y, event.data[1]) || updated;
                    updated = tryModifyState(data.m_sensor.m_accel.m_z, event.data[2]) || updated;
                    return updated;
                });

        case SDL_SensorType::SDL_SENSOR_GYRO:
        case SDL_SensorType::SDL_SENSOR_GYRO_L:
        case SDL_SensorType::SDL_SENSOR_GYRO_R:
            return manager.tryUpdateData(
                event.which,
                [&event](auto& data)
                {
                    bool updated{false};
                    // TODO: check if TS changes
                    updated = tryModifyState(data.m_sensor.m_ts, event.sensor_timestamp);
                    updated = tryModifyState(data.m_sensor.m_gyro.m_pitch, event.data[0]) || updated;
                    updated = tryModifyState(data.m_sensor.m_gyro.m_yaw, event.data[1]) || updated;
                    updated = tryModifyState(data.m_sensor.m_gyro.m_roll, event.data[2]) || updated;
                    return updated;
                });

        default:
            BOOST_LOG_TRIVIAL(trace) << "sensor (" << event.sensor << ") value change [" << event.data[0] << ", "
                                     << event.data[1] << ", " << event.data[2] << "] with TS " << event.sensor_timestamp
                                     << " received for gamepad " << event.which;
            return std::nullopt;
    }
}
}  // namespace gamepads
