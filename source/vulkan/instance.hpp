#pragma once

#include "external.hpp"
#include "noncopyable.hpp"
#include "impl.hpp"

namespace vulkan
{

class Instance : NonCopyable
{
public:
    inline Instance();

    inline ~Instance();

    inline const VkInstance &operator*() const;

private:
    impl::Instance impl;
};

inline Instance::Instance()
: impl(impl::create_instance())
{

}

inline Instance::~Instance()
{
    impl::destroy_instance(impl);
}

inline const VkInstance &Instance::operator*() const
{
    return impl;
}

}
