set_project("licht")
set_version("0.0.1-dev")
set_languages("c++20")

set_targetdir("build/bin/$(plat)_$(arch)_$(mode)")

add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    add_defines("LDEBUG")
end


add_requires("libsdl3", "catch2")


target("licht.core")
    set_kind("shared")

    add_includedirs("source")

    add_files("source/licht/core/**.cpp")
    add_headerfiles("source/licht/core/**.hpp")

    add_defines("LICHT_CORE_EXPORTS")


target("licht.rhi")
    set_kind("shared")
    
    add_deps("licht.core")

    add_includedirs("source")
    add_files("source/licht/rhi/**.cpp")
    add_headerfiles("source/licht/rhi/**.hpp")

    add_defines("LICHT_RHI_EXPORTS")


target("licht.platform")
    set_kind("shared")

    add_deps("licht.core")

    add_packages("libsdl3")

    add_includedirs("source")

    add_files("source/licht/platform/**.cpp")
    add_headerfiles("source/licht/platform/**.hpp")

    add_defines("LICHT_PLATFORM_EXPORTS")


target("licht.core.tests")
    set_kind("binary")

    add_deps("licht.core")
    
    add_packages("catch2")

    add_includedirs("source")
    
    add_files("tests/core/**.cpp")


target("licht.demo")
    set_kind("binary")

    add_deps("licht.core", "licht.platform")

    add_includedirs("source")

    add_files("source/licht-demo/**.cpp")
    add_headerfiles("source/licht-demo/**.hpp")
