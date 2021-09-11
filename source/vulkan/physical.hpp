#pragma once

#include "external.hpp"
#include "instance.hpp"
#include "noncopyable.hpp"
#include "impl.hpp"

namespace vulkan
{

class Physical : NonCopyable
{
public:
    inline Physical(const Instance &instance);

    inline ~Physical() = default;

    inline const impl::PhysicalDevice &operator*() const;

    inline uint32_t get_queue_family_index() const;

private:
    impl::PhysicalDevice impl;

    uint32_t queue_family_index;
};

inline Physical::Physical(const Instance &instance)
: impl(impl::choose_physical_device(*instance))
, queue_family_index(impl::select_queue_family_index(impl))
{
}

inline const VkPhysicalDevice &Physical::operator*() const
{
    return impl;
}

inline uint32_t Physical::get_queue_family_index() const
{
    return queue_family_index;
}

}
