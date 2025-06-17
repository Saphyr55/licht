set_project("licht")
set_version("0.0.1-dev")
set_languages("c++20")

set_targetdir("build/bin/$(plat)_$(arch)_$(mode)")

add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    add_defines("LDEBUG")
end


add_requires("libsdl3", "catch2", "vulkan-headers")


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


target("licht.rhi-vulkan")
    set_kind("shared")
    
    add_deps("licht.core", "licht.platform")

    add_includedirs("source")
    add_files("source/licht/rhi_vulkan/**.cpp")
    add_headerfiles("source/licht/rhi_vulkan/**.hpp")

    add_packages("vulkan-headers", { public = true })

    add_defines("LICHT_RHI_VULKAN_EXPORTS")

	if is_plat("windows", "mingw") then
		add_defines("VK_USE_PLATFORM_WIN32_KHR")
		add_syslinks("User32")
    end


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

    -- TODO: Add rhi-vulkan as a dependency when the demo uses it, with an option to enable it.
    add_deps("licht.core", "licht.platform", "licht.rhi-vulkan")

    add_includedirs("source")

    add_files("source/licht-demo/**.cpp")
    add_headerfiles("source/licht-demo/**.hpp")
