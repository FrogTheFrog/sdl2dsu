// class header include
#include "handlesensorupdate.h"

// system includes
#include <boost/log/trivial.hpp>
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

//--------------------------------------------------------------------------------------------------

std::uint64_t timestampToDsuTimestamp(std::uint64_t ts)
{
    // SDL provides TS in nanoseconds, we need microseconds
    return ts / 1000;
}

//--------------------------------------------------------------------------------------------------

float accelToDsuAccel(float value)
{
    // SDL standardizes the accel value, but DSU does not like the gravity multiplier
    return value / SDL_STANDARD_GRAVITY;
}

//--------------------------------------------------------------------------------------------------

float gyroToDsuGyro(float value)
{
    // SDL also standardizes the gyro value, but DSU needs deg/s
    return static_cast<float>(value * 180.0f / M_PI);
}
}  // namespace

//--------------------------------------------------------------------------------------------------

bool handleSensorUpdate(const SDL_GamepadSensorEvent& event, shared::GamepadData& data)
{
    BOOST_LOG_TRIVIAL(trace) << "sensor (" << event.sensor << ") value change [" << event.data[0] << ", "
                             << event.data[1] << ", " << event.data[2] << "] with TS " << event.sensor_timestamp
                             << " received for gamepad " << event.which;

    bool updated{false};
    switch (event.sensor)
    {
        case SDL_SensorType::SDL_SENSOR_ACCEL:
        case SDL_SensorType::SDL_SENSOR_ACCEL_L:
        case SDL_SensorType::SDL_SENSOR_ACCEL_R:
        {
            updated = tryModifyState(data.m_sensor.m_ts, timestampToDsuTimestamp(event.sensor_timestamp));
            updated = tryModifyState(data.m_sensor.m_accel.m_x, accelToDsuAccel(-event.data[0])) || updated;
            updated = tryModifyState(data.m_sensor.m_accel.m_y, accelToDsuAccel(-event.data[1])) || updated;
            updated = tryModifyState(data.m_sensor.m_accel.m_z, accelToDsuAccel(-event.data[2])) || updated;
            break;
        }

        case SDL_SensorType::SDL_SENSOR_GYRO:
        case SDL_SensorType::SDL_SENSOR_GYRO_L:
        case SDL_SensorType::SDL_SENSOR_GYRO_R:
        {
            updated = tryModifyState(data.m_sensor.m_ts, timestampToDsuTimestamp(event.sensor_timestamp));
            updated = tryModifyState(data.m_sensor.m_gyro.m_pitch, gyroToDsuGyro(event.data[0])) || updated;
            updated = tryModifyState(data.m_sensor.m_gyro.m_yaw, gyroToDsuGyro(-event.data[1])) || updated;
            updated = tryModifyState(data.m_sensor.m_gyro.m_roll, gyroToDsuGyro(-event.data[2])) || updated;
            break;
        }

        default:
            break;
    }

    return updated;
}
}  // namespace gamepads
