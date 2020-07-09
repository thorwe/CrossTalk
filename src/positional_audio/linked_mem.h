#pragma once

#include <cstdint>

namespace thorwe::Mu_Linked_Mem
{
constexpr const int32_t kMuNameBufferSize = 256;
constexpr const int32_t kMuIdentityBufferSize = 256;
constexpr const int32_t kMuContextBufferSize = 256;
constexpr const int32_t kMuDescBufferSize = 2048;
}  // namespace thorwe::Mu_Linked_Mem

struct LinkedMem
{
    uint32_t uiVersion = 0;
    uint32_t dwcount = 0;
    float fAvatarPosition[3] = {0};
    float fAvatarFront[3] = {0};
    float fAvatarTop[3] = {0};
    wchar_t name[thorwe::Mu_Linked_Mem::kMuNameBufferSize] = {0};
    float fCameraPosition[3] = {0};
    float fCameraFront[3] = {0};
    float fCameraTop[3] = {0};
    wchar_t identity[thorwe::Mu_Linked_Mem::kMuIdentityBufferSize] = {0};
    uint32_t context_len = 0;
    unsigned char context[thorwe::Mu_Linked_Mem::kMuContextBufferSize] = {0};
    wchar_t description[thorwe::Mu_Linked_Mem::kMuDescBufferSize] = {0};
};
