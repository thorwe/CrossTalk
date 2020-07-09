#pragma once

#include "definitions_positionalaudio.h"
#include "linked_mem.h"

#include "teamspeak/public_definitions.h"

#include <gsl/span>

#include <array>
#include <mutex>
#include <optional>
#include <shared_mutex>

namespace thorwe
{

// the values except dw_count never change during a game
struct Linked_Mem_Try
{
    uint32_t dw_count = 0;
    std::wstring name = {};
    std::wstring description = {};
};

struct Linked_Mem_Diff
{
    uint32_t ui_version = 0;
    uint32_t dw_count = 0;
    std::optional<Three_Dee_Info> avatar = std::nullopt;
    std::optional<Three_Dee_Info> camera = std::nullopt;
    std::optional<std::wstring> identity = std::nullopt;
    std::optional<std::vector<std::byte>> context = std::nullopt;
};

struct Linked_Mem_Cache
{
    uint32_t ui_version = 0;
    uint32_t dw_count = 0;
    TS3_VECTOR avatar_position = {0.f, 0.f, 0.f};
    TS3_VECTOR avatar_front = {0.f, 0.f, 0.f};
    TS3_VECTOR avatar_top = {0.f, 0.f, 0.f};
    std::wstring name = {};
    TS3_VECTOR camera_position = {0.f, 0.f, 0.f};
    TS3_VECTOR camera_front = {0.f, 0.f, 0.f};
    TS3_VECTOR camera_top = {0.f, 0.f, 0.f};
    std::wstring identity = {};
    std::vector<std::byte> context = {};
    std::wstring description = {};
};

class Shared_Mem_Reader
{
  public:
    void set_lm(LinkedMem *);
    [[nodiscard]] std::optional<Linked_Mem_Try> try_read();
    [[nodiscard]] std::optional<Linked_Mem_Diff> read_mem();
    void zero_lm();

    uint32_t ui_version() const;
    uint32_t dw_count() const;
    std::array<TS3_VECTOR, 3> avatar() const;
    std::wstring name() const;
    std::array<TS3_VECTOR, 3> camera() const;
    std::wstring identity() const;
    std::vector<std::byte> context() const;
    std::wstring description() const;

  private:
    std::mutex m_lm_mutex;
    mutable std::shared_mutex m_mutex;

    LinkedMem *lm = nullptr;
    Linked_Mem_Cache m_last_lm;
};

}  // namespace thorwe

bool operator==(const TS3_VECTOR &vec, gsl::span<const float> arr);
bool operator!=(const TS3_VECTOR &vec, gsl::span<const float> arr);
