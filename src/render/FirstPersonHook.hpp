#pragma once

#include <cstdint>


namespace levifreecam::render {


class FirstPersonHook final {


public:


    static FirstPersonHook&
    instance()
    noexcept;



    bool install()
    noexcept;



    void uninstall()
    noexcept;



    bool installed()
    const noexcept;



    void setEnabled(
        bool enabled
    )
    noexcept;



    bool isEnabled()
    const noexcept;



    static std::uintptr_t
    targetAddress()
    noexcept;



private:


    FirstPersonHook()
        = default;



    bool mInstalled{
        false
    };


    bool mEnabled{
        false
    };


};



}
