#pragma once

#include <gsl/span>

#include <atomic>

class SimplePanner
{
  public:
    void set_current(float);
    float get_current() const;
    void set_desired(float);
    float get_desired() const;
    void set_desired_by_manual(float);
    float get_desired_by_manual() const;
    void set_desired_by_pan_adjuster(float);
    float get_desired_by_pan_adjuster() const;
    void set_adjust(bool);
    bool get_adjust() const;

    void process(gsl::span<int16_t> samples, int channels, int leftChannelNr, int rightChannelNr);

  private:
    std::atomic_bool m_adjust = false;

    // Property Privates
    std::atomic<float> m_current{0.0f};                   // decibels
    std::atomic<float> m_desired = 0.0f;                  // decibels
    std::atomic<float> m_desired_by_manual = 0.0f;        // decibels
    std::atomic<float> m_desired_by_pan_adjuster = 0.0f;  // decibels
};
