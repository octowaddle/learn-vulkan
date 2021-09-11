#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "physical.hpp"
#include "impl.hpp"

namespace vulkan
{

class Device : NonCopyable
{
public:
    inline Device(const Physical &physical);

    inline ~Device();

    inline void wait_idle() const;

    inline const impl::Device &operator*() const;

private:
    impl::Device impl;
};

inline Device::Device(const Physical &physical)
: impl(impl::create_device(physical.get_queue_family_index(), *physical))
{
}

inline Device::~Device()
{
    impl::destroy_device(impl);
}

inline void Device::wait_idle() const
{
    impl::device_wait_idle(impl);
}

inline const VkDevice &Device::operator*() const
{
    return impl;
}

}
