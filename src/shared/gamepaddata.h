#pragma once

// system includes
#include <array>
#include <cstdint>
#include <optional>

// local includes

//--------------------------------------------------------------------------------------------------

namespace shared
{
namespace details
{
enum class BatteryLevel : std::uint8_t
{
    Unknown = 0x00,
    Empty   = 0x01,
    Low     = 0x02,
    Medium  = 0x03,
    Full    = 0x05,
    Wired   = 0xEE
};

//--------------------------------------------------------------------------------------------------

struct ABXY
{
    bool m_a{false};
    bool m_b{false};
    bool m_x{false};
    bool m_y{false};
};

//--------------------------------------------------------------------------------------------------

struct Dpad
{
    bool m_up{false};
    bool m_down{false};
    bool m_left{false};
    bool m_right{false};
};

//--------------------------------------------------------------------------------------------------

struct Special
{
    bool m_back{false};
    bool m_guide{false};
    bool m_start{false};
};

//--------------------------------------------------------------------------------------------------

struct Shoulder
{
    bool m_left{false};
    bool m_right{false};
};

//--------------------------------------------------------------------------------------------------

struct Trigger
{
    std::uint8_t m_left{0};
    std::uint8_t m_right{0};
};

//--------------------------------------------------------------------------------------------------

struct Stick
{
    bool         m_pressed{false};
    std::uint8_t m_x{0};
    std::uint8_t m_y{0};
};

//--------------------------------------------------------------------------------------------------

struct Touchpad
{
    bool m_pressed{false};
};

//--------------------------------------------------------------------------------------------------

struct Accel
{
    float m_x{0.f};
    float m_y{0.f};
    float m_z{0.f};
};

//--------------------------------------------------------------------------------------------------

struct Gyro
{
    float m_pitch{0.f};
    float m_yaw{0.f};
    float m_roll{0.f};
};

//--------------------------------------------------------------------------------------------------

struct Sensor
{
    bool          m_supported{false};
    Accel         m_accel{};
    Gyro          m_gyro{};
    std::uint64_t m_ts{0};
};
}  // namespace details

//--------------------------------------------------------------------------------------------------

struct GamepadData
{
    details::BatteryLevel m_battery{details::BatteryLevel::Unknown};
    details::ABXY         m_abxy;
    details::Dpad         m_dpad;
    details::Special      m_special;
    details::Shoulder     m_shoulder;
    details::Trigger      m_trigger;
    details::Stick        m_left_stick;
    details::Stick        m_right_stick;
    details::Touchpad     m_touchpad;
    details::Sensor       m_sensor;
};

//--------------------------------------------------------------------------------------------------

using GamepadDataContainer = std::array<std::optional<GamepadData>, 4>;
}  // namespace shared
