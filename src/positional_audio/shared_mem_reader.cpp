#include "shared_mem_reader.h"

#include <gsl/gsl_util>

#include <algorithm>

namespace thorwe
{

void Shared_Mem_Reader::set_lm(LinkedMem *val)
{
    auto lm_lock = std::lock_guard(m_lm_mutex);
    lm = val;
}

auto Shared_Mem_Reader::try_read() -> std::optional<Linked_Mem_Try>
{
    auto lm_lock = std::lock_guard(m_lm_mutex);
    if (!lm)
        return std::nullopt;

    std::unique_lock<decltype(m_mutex)> lock(m_mutex);

    auto result = Linked_Mem_Try{};

    const auto version = lm->uiVersion;
    const auto ui_version_changed = version != m_last_lm.ui_version;
    if (ui_version_changed)
        m_last_lm.ui_version = ui_version();

    if (!(version == 1 || version == 2))
        return std::nullopt;

    auto name = std::wstring{};
    if (lm->name[0])
    {
        auto arr = gsl::span{lm->name};
        if (auto it = std::find(std::begin(arr), std::end(arr), L'\0'); it != std::end(arr))
        {
            if (auto index = std::distance(std::begin(arr), it); index > 0)
                name = std::wstring(arr.data(), gsl::narrow_cast<size_t>(index));
        }
        else
            name = std::wstring(std::begin(arr), std::end(arr));
    }
    const auto name_changed = name != m_last_lm.name;
    if (name_changed)
    {
        m_last_lm.name = name;
        result.name = name;
    }

    // name is mandatory
    if (name.empty())
        return std::nullopt;

    result.dw_count = lm->dwcount;

    // description is optional
    auto description = std::wstring{};
    if (lm->description[0])
    {
        auto arr = gsl::span{lm->description};
        if (auto it = std::find(std::begin(arr), std::end(arr), L'\0'); it != std::end(arr))
        {
            if (auto index = std::distance(std::begin(arr), it); index > 0)
                description = std::wstring(arr.data(), gsl::narrow_cast<size_t>(index));
        }
        else
            description = std::wstring(std::begin(arr), std::end(arr));
    }
    const auto description_changed = description != m_last_lm.description;
    if (description_changed)
    {
        m_last_lm.description = description;
        result.description = description;
    }
    return result;
}

auto Shared_Mem_Reader::read_mem() -> std::optional<Linked_Mem_Diff>
{
    auto lm_lock = std::lock_guard(m_lm_mutex);
    if (!lm)
        return std::nullopt;

    std::unique_lock<decltype(m_mutex)> lock(m_mutex);

    const auto dw_count = lm->dwcount;
    const auto dw_count_changed = dw_count != m_last_lm.dw_count;
    if (!dw_count_changed)
        return std::nullopt;

    m_last_lm.dw_count = dw_count;

    Linked_Mem_Diff result;

    // avatar position front top
    {
        const auto &avatar_position = lm->fAvatarPosition;
        const auto avatar_position_changed =
        m_last_lm.avatar_position != gsl::span<const float>{avatar_position};
        if (avatar_position_changed)
        {
            m_last_lm.avatar_position.x = avatar_position[0];
            m_last_lm.avatar_position.y = avatar_position[1];
            m_last_lm.avatar_position.z = avatar_position[2];
        }

        const auto &avatar_front = lm->fAvatarFront;
        const auto avatar_front_changed = m_last_lm.avatar_front != gsl::span<const float>{avatar_front};
        if (avatar_front_changed)
        {
            m_last_lm.avatar_front.x = avatar_front[0];
            m_last_lm.avatar_front.y = avatar_front[1];
            m_last_lm.avatar_front.z = avatar_front[2];
        }

        const auto &avater_top = lm->fAvatarTop;
        const auto avatar_top_changed = m_last_lm.avatar_top != gsl::span<const float>{avater_top};
        if (avatar_top_changed)
        {
            m_last_lm.avatar_top.x = avater_top[0];
            m_last_lm.avatar_top.y = avater_top[1];
            m_last_lm.avatar_top.z = avater_top[2];
        }

        if (avatar_position_changed || avatar_front_changed || avatar_top_changed)
        {
            result.avatar =
            Three_Dee_Info{m_last_lm.avatar_position, m_last_lm.avatar_front, m_last_lm.avatar_top};
        }
    }

    if (lm->uiVersion == 2)
    {
        // identity
        {
            auto identity = std::wstring{};
            {
                auto arr = gsl::span{lm->identity};
                if (auto it = std::find(std::begin(arr), std::end(arr), L'\0'); it != std::end(arr))
                {
                    if (auto index = std::distance(std::begin(arr), it); index > 0)
                        identity = std::wstring(arr.data(), gsl::narrow_cast<size_t>(index));
                }
                else
                    identity = std::wstring(std::begin(arr), std::end(arr));
            }
            if (identity != m_last_lm.identity)
            {
                m_last_lm.identity = identity;
                result.identity = identity;
            }
        }

        // context
        {
            auto context_changed = lm->context_len != m_last_lm.context.size();
            auto arr = gsl::span{reinterpret_cast<std::byte *>(&lm->context), lm->context_len};

            if (!context_changed)
            {
                const auto is_equal = std::equal(std::begin(m_last_lm.context), std::end(m_last_lm.context),
                                                 std::begin(arr), std::end(arr));
                context_changed = !is_equal;
            }
            if (context_changed)
            {
                m_last_lm.context = std::vector<std::byte>(std::begin(arr), std::end(arr));
                result.context = m_last_lm.context;
            }
        }

        // camera position front top
        {
            const auto &camera_position = lm->fCameraPosition;
            const auto camera_position_changed =
            m_last_lm.camera_position != gsl::span<const float>{camera_position};
            if (camera_position_changed)
            {
                m_last_lm.camera_position.x = camera_position[0];
                m_last_lm.camera_position.y = camera_position[1];
                m_last_lm.camera_position.z = camera_position[2];
            }

            const auto &camera_front = lm->fCameraFront;
            const auto camera_front_changed = m_last_lm.camera_front != gsl::span<const float>{camera_front};
            if (camera_front_changed)
            {
                m_last_lm.camera_front.x = camera_front[0];
                m_last_lm.camera_front.y = camera_front[1];
                m_last_lm.camera_front.z = camera_front[2];
            }

            const auto &camera_top = lm->fCameraTop;
            const auto camera_top_changed = m_last_lm.camera_top != gsl::span<const float>{camera_top};
            if (camera_top_changed)
            {
                m_last_lm.camera_top.x = camera_top[0];
                m_last_lm.camera_top.y = camera_top[1];
                m_last_lm.camera_top.z = camera_top[2];
            }

            if (camera_position_changed || camera_front_changed || camera_top_changed)
            {
                result.camera =
                Three_Dee_Info{m_last_lm.camera_position, m_last_lm.camera_front, m_last_lm.camera_top};
            }
        }
    }

    const auto return_result = result.avatar.has_value() || result.camera.has_value() ||
                               result.context.has_value() || result.identity.has_value();
    if (return_result)
    {
        result.ui_version = lm->uiVersion;
        result.dw_count = dw_count;
        return result;
    }
    return std::nullopt;
}

void Shared_Mem_Reader::zero_lm()
{
    // TODO: Is that necessary? Does M do that?
    auto lm_lock = std::lock_guard(m_lm_mutex);
    if (lm)
    {
        lm->dwcount = 0;
        lm->uiVersion = 0;
        lm->name[0] = 0;
        lm->identity[0] = 0;
        lm->description[0] = 0;
    }
}

auto Shared_Mem_Reader::ui_version() const -> uint32_t
{
    std::shared_lock<decltype(m_mutex)> lock(m_mutex);
    return m_last_lm.ui_version;
}

auto Shared_Mem_Reader::dw_count() const -> uint32_t
{
    std::shared_lock<decltype(m_mutex)> lock(m_mutex);
    return m_last_lm.dw_count;
}

auto Shared_Mem_Reader::avatar() const -> std::array<TS3_VECTOR, 3>
{
    std::shared_lock<decltype(m_mutex)> lock(m_mutex);
    return {m_last_lm.avatar_position, m_last_lm.avatar_front, m_last_lm.avatar_top};
}

auto Shared_Mem_Reader::name() const -> std::wstring
{
    std::shared_lock<decltype(m_mutex)> lock(m_mutex);
    return m_last_lm.name;
}

auto Shared_Mem_Reader::camera() const -> std::array<TS3_VECTOR, 3>
{
    std::shared_lock<decltype(m_mutex)> lock(m_mutex);
    return {m_last_lm.camera_position, m_last_lm.camera_front, m_last_lm.camera_top};
}

auto Shared_Mem_Reader::identity() const -> std::wstring
{
    std::shared_lock<decltype(m_mutex)> lock(m_mutex);
    return m_last_lm.identity;
}

auto Shared_Mem_Reader::context() const -> std::vector<std::byte>
{
    std::shared_lock<decltype(m_mutex)> lock(m_mutex);
    return m_last_lm.context;
}

auto Shared_Mem_Reader::description() const -> std::wstring
{
    std::shared_lock<decltype(m_mutex)> lock(m_mutex);
    return m_last_lm.description;
}


}  // namespace thorwe

bool operator==(const TS3_VECTOR &vec, gsl::span<const float> arr)
{
    if (arr.size() != 3)
        return false;

    return ((vec.x == arr[0]) && (vec.y == arr[1]) && (vec.z == arr[2]));
}

bool operator!=(const TS3_VECTOR &vec, gsl::span<const float> arr)
{
    return !(vec == arr);
}
