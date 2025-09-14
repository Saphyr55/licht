set_project("licht")
set_version("0.0.1-dev")
set_languages("c++20")

set_targetdir("build/bin/$(plat)_$(arch)_$(mode)")

add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate", {
    outputdir = ".xmake"
})

if is_mode("debug") then
    add_defines("LDEBUG")
end

includes("scripts/**.lua")

add_requires("libsdl3", "catch2", "vulkan-headers", "lua")

target("licht.engine.core", function()
    set_kind("shared")

    add_packages("libsdl3", "lua")

    add_includedirs("source/licht-engine/core")

    add_headerfiles("source/licht-engine/core/**.hpp")
    add_files("source/licht-engine/core/**.cpp")

    add_defines("LICHT_CORE_EXPORTS")

end)

target("licht.engine.rhi", function()
    set_kind("shared")

    add_deps("licht.engine.core")

    add_includedirs("source/licht-engine/rhi", "source/licht-engine/core")

    add_headerfiles("source/licht-engine/rhi/**.hpp")
    add_files("source/licht-engine/rhi/**.cpp")

    add_defines("LICHT_RHI_EXPORTS")

end)

target("licht.engine.rhi.vulkan", function()
    set_kind("shared")

    add_deps("licht.engine.core", "licht.engine.rhi")

    add_includedirs(unpack({
        "source/licht-engine/rhi_vulkan",
        "source/licht-engine/core",
        "source/licht-engine/rhi"
    }))

    add_headerfiles("source/licht-engine/rhi_vulkan/**.hpp")
    add_files("source/licht-engine/rhi_vulkan/**.cpp")

    add_packages("vulkan-headers", {
        public = true
    })

    add_defines("LICHT_RHI_VULKAN_EXPORTS")

    if is_plat("windows", "mingw") then
        add_defines("VK_USE_PLATFORM_WIN32_KHR")
        add_syslinks("User32")
    end
end)

----------------------------------------------------
-- Tests -------------------------------------------
----------------------------------------------------

target("licht.engine.core.tests", function()
    set_kind("binary")

    add_deps("licht.engine.core")

    add_packages("catch2")

    add_includedirs("source/licht-engine/core")

    add_files("tests/core/**.cpp")

end)

----------------------------------------------------
-- Demos -------------------------------------------
----------------------------------------------------

target("licht.demo", function()
    set_kind("binary")

    add_deps("licht.engine.core", "licht.engine.rhi")

    add_includedirs("source/licht-engine/core", "source/licht-engine/rhi", "source/licht-demo")

    add_files("source/licht-demo/**.cpp")
    add_headerfiles("source/licht-demo/**.hpp")

    local shaders_file = {
        "shaders/main.frag",
        "shaders/main.vert"
    }

    after_build(function(target)

        import("core.base.task")
        import("core.project.project")

        local output_shaders = path.join(target:targetdir(), "shaders")

        task.run("compile-shaders", {
            files = shaders_file,
            output = output_shaders
        })

    end)

end)
