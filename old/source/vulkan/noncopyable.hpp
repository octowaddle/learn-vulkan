#pragma once

class NonCopyable
{
public:
    NonCopyable(NonCopyable &) = delete;

protected:
    NonCopyable() = default;

    ~NonCopyable() = default;
};
