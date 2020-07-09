#include "simplepanner.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

constexpr const float kPi = 3.14159265358979323846f;
constexpr const float kPanFadeTime = 400.F;  // Rate to fade at (dB per second)
constexpr const float kApaAttackRate = 20.0f;
constexpr const float kApaDecayRate = 1.0f;

constexpr const std::int32_t kSampleRate = 48000;

//! Sets the current pan (-1-+1) either set by user interaction or Pan adjustment
/*!
  Intended for internal use
  \param value the current pan (-1-+1)
*/
void SimplePanner::set_current(float value)
{
    m_current.store(value);
}

//! Gets the current pan (-1-+1) either set by user interaction or Pan adjustment
/*!
  \return the current pan (-1-+1)
*/
float SimplePanner::get_current() const
{
    return m_current.load();
}

//! Sets the desired pan (-1-+1) either set by user interaction or Pan adjustment
/*!
  Intended for internal use
  \param value the desired pan (-1-+1)
*/
void SimplePanner::set_desired(float value)
{
    m_desired.store(value);
}

//! Gets the desired pan (-1-+1) either set by user interaction or Pan adjustment
/*!
  \return the desired pan (-1-+1)
*/
float SimplePanner::get_desired() const
{
    return m_desired.load();
}

//! Sets the desired pan (-1-+1) of a user interaction
/*!
  Intended for internal use
  \param value the desired Pan of a user interaction (dB)
*/
void SimplePanner::set_desired_by_manual(float value)
{
    m_desired_by_manual.store(value);
    set_desired(value);
}

//! Gets the desired pan (-1-+1) set by the last user interaction
/*!
  \return the desired pan (-1-+1) of the last user interaction
*/
float SimplePanner::get_desired_by_manual() const
{
    return m_desired_by_manual.load();
}

//! Sets the desired pan (-1-+1) of an automatic Pan adjustment
/*!
  Intended for internal use
  \param value the desired Pan of an automatic Pan adjustment (dB)
*/
void SimplePanner::set_desired_by_pan_adjuster(float value)
{
    m_desired_by_pan_adjuster.store(value);
    set_desired(value);
}

//! Gets the desired pan (-1-+1) set by the last automatic Pan adjustment
/*!
  \return the desired pan (-1-+1) of the automatic Pan adjustment
*/
float SimplePanner::get_desired_by_pan_adjuster() const
{
    return m_desired_by_pan_adjuster.load();
}

bool SimplePanner::get_adjust() const
{
    return m_adjust.load();
}

void SimplePanner::set_adjust(bool value)
{
    m_adjust.store(value);
}


// process

// void SimplePanner::process(int nSamples,
//                      float *pleft,
//                      float *pright,
//                      float balance // -1...+1
//                      )
//{
//    float p, gl, gr;

//    p=M_PI*(balance+1)/4;
//    gl=::cos(p);
//    gr=::sin(p);

//    while( nSamples-- )
//    {
//       *pleft =*pleft++ *gl;
//       *pright=*pright++*gr;
//    }
//}

namespace
{
void do_process(gsl::span<float> left,
                gsl::span<float> right,
                float balance  // -1...+1
)
{
    balance = balance < -1 ? -1 : balance;
    balance = balance > 1 ? 1 : balance;

    const float p = kPi * (balance + 1) / 4;
    const auto gl = ::cos(p);
    const auto gr = ::sin(p);

    for (auto &sample : left)
        sample *= gl;
    for (auto &sample : right)
        sample *= gr;
}
}  // namespace

void SimplePanner::process(gsl::span<int16_t> samples, int channels, int leftChannelNr, int rightChannelNr)
{
    const auto frame_count = samples.size() / channels;
    // Determine Pan for current buffer
    auto current = get_current();
    auto desired = get_desired();
    auto desired_by_manual = get_desired_by_manual();
    auto desired_by_pan_adjuster = get_desired_by_pan_adjuster();

    if (desired != desired_by_manual)
    {
        if (get_adjust())
        {
            // is attacking
            float fade_step_down = (kApaAttackRate / kSampleRate) * frame_count;
            float fade_step_up = (kApaDecayRate / kSampleRate) * frame_count;
            if (current < desired_by_pan_adjuster - fade_step_up)
            {
                current += fade_step_up;
            }
            else if (current > desired_by_pan_adjuster + fade_step_down)
            {
                current -= fade_step_down;
            }
            else
            {
                current = desired_by_pan_adjuster;
            }
        }
        else
        {
            // is releasing
            float fade_step_gainAdjust = (kApaDecayRate / kSampleRate) * frame_count;
            if (current < desired_by_manual - fade_step_gainAdjust)
            {
                current += fade_step_gainAdjust;
            }
            else if (current > desired_by_manual + fade_step_gainAdjust)
            {
                current -= fade_step_gainAdjust;
            }
            else
            {
                current = desired_by_manual;
            }
        }
    }
    else if (current != desired_by_manual)
    {
        float fade_step = (kPanFadeTime / kSampleRate) * frame_count;
        if (current < desired_by_manual - fade_step)
        {
            current += fade_step;
        }
        else if (current > desired_by_manual + fade_step)
        {
            current -= fade_step;
        }
        else
        {
            current = desired_by_manual;
        }
    }
    set_current(current);

    // End Determine Pan for current buffer

    // Extract from interleaved and convert to QList<float>
    std::vector<float> float_data_left;
    std::vector<float> float_data_right;
    for (auto i = decltype(frame_count){0}; i < frame_count; ++i)
    {
        float_data_left.push_back(samples[leftChannelNr + (i * channels)]);
        float_data_right.push_back(samples[rightChannelNr + (i * channels)]);
    }

    // don't need to adjust the range to -1 - +1 do we?

    do_process(float_data_left, float_data_right, current);

    // put it back into interleaved and Clip
    for (auto i = decltype(frame_count){0}; i < frame_count; ++i)
    {
        auto left = float_data_left.at(i);
        samples[leftChannelNr + (i * channels)] = static_cast<int16_t>(
        std::clamp<int32_t>(static_cast<int32_t>(floor(.5 + left)), std::numeric_limits<int16_t>::min(),
                            std::numeric_limits<int16_t>::max()));

        float right = float_data_right.at(i);
        samples[rightChannelNr + (i * channels)] = static_cast<int16_t>(
        std::clamp<int32_t>(static_cast<int32_t>(floor(.5 + right)), std::numeric_limits<int16_t>::min(),
                            std::numeric_limits<int16_t>::max()));
    }
}
