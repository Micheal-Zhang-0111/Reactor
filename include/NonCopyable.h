#pragma once

class NonCopyable
{
protected:
    NonCopyable(){
    }
    ~NonCopyable(){
    }

    NonCopyable(const NonCopyable& rhs) = delete;
    NonCopyable &operator=(const NonCopyable& rhs) = delete;

};