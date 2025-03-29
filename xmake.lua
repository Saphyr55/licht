set_project("licht")
set_version("0.0.1-dev")
set_languages("c++20")

set_targetdir("build/bin/$(plat)_$(arch)_$(mode)")

add_rules("mode.debug", "mode.release")

add_requires("catch2")

if is_mode("debug") then
    add_defines("LDEBUG")
end


target("licht.core")
    set_kind("shared")

    add_files("core/**.cpp")
    add_headerfiles("core/**.hpp")
    add_includedirs(".", { public = true })
    add_defines("LICHT_CORE_EXPORTS")


target("licht.display")
    set_kind("shared")

    add_deps("licht.core")
    add_files("runtime/display/**.cpp")
    add_headerfiles("runtime/display/**.hpp")
    add_includedirs("runtime", { public = true })
    add_defines("LICHT_DISPLAY_EXPORTS")


target("licht.platform")
    set_kind("static")

    add_deps("licht.core", "licht.display")
    add_files("platform/**.cpp")
    add_headerfiles("platform/**.hpp", { public = true })
    add_includedirs("platform/")

    if is_plat("windows") then
        add_defines("LICHT_WIN32")
        add_defines("UNICODE", "_UNICODE")
        -- add_ldflags("/subsystem:windows", "/entry:mainCRTStartup")
        add_syslinks("user32", "gdi32", "kernel32")
    end


target("licht.demo")
    set_kind("binary")

    add_deps("licht.display", "licht.core", "licht.platform")
    add_files("samples/demo/**.cpp")
    add_headerfiles("samples/demo/**.hpp")
    add_includedirs("samples/demo/")


target("licht.core.tests")
    set_kind("binary")

    add_deps("licht.core")
    add_packages("catch2")
    add_files("tests/core/**.cpp")
