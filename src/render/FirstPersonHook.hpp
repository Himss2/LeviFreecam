#pragma once

#include <atomic>


namespace levifreecam::render {


class FirstPersonHook final
{

public:


    static FirstPersonHook&
    instance()
    noexcept;



    bool install()
    noexcept;



    void uninstall()
    noexcept;



    void setEnabled(
        bool enabled
    )
    noexcept;



    [[nodiscard]]
    bool enabled()
    const noexcept;



private:


    FirstPersonHook() = default;


    std::atomic_bool
        mEnabled{
            false
        };


    std::atomic_bool
        mInstalled{
            false
        };


};



}
