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

----------------------------------------------------
-- Engine Module -----------------------------------
----------------------------------------------------
target("licht.engine.core", function()
    set_kind("shared")
    set_group("licht.engine/source")

    add_packages("libsdl3", "lua")

    add_includedirs("source/licht-engine/core")

    add_headerfiles("source/licht-engine/core/**.hpp")
    add_files("source/licht-engine/core/**.cpp")

    add_defines("LICHT_CORE_EXPORTS")

end)

target("licht.engine.rhi", function()
    set_kind("shared")
    set_group("licht.engine/source")

    add_deps("licht.engine.core")

    add_includedirs("source/licht-engine/rhi", "source/licht-engine/core")

    add_headerfiles("source/licht-engine/rhi/**.hpp")
    add_files("source/licht-engine/rhi/**.cpp")

    add_defines("LICHT_RHI_EXPORTS")
end)

target("licht.engine.rhi.vulkan", function()
    set_kind("shared")
    set_group("licht.engine/source")

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

    if is_plat("windows") then
        add_defines("VK_USE_PLATFORM_WIN32_KHR")
        add_syslinks("User32")
    end
end)

target("licht.engine.launcher", function()
    set_kind("shared")
    set_group("licht.engine/source")

    add_deps("licht.engine.core")

    add_includedirs(unpack({
        "source/licht-engine/core",
        "source/licht-engine/launcher"
    }))

    add_headerfiles("source/licht-engine/launcher/**.hpp")
    add_files("source/licht-engine/launcher/**.cpp")

    add_defines("LICHT_LAUNCHER_EXPORTS")

    if is_plat("windows") then
        add_syslinks("User32")
    end
end)

----------------------------------------------------
-- Tests -------------------------------------------
----------------------------------------------------

target("licht.engine.core.tests", function()
    set_kind("binary")
    set_group("licht.engine/tests")

    add_deps("licht.engine.core")

    add_packages("catch2")

    add_includedirs("source/licht-engine/core")

    add_files("tests/core/**.cpp")

end)

----------------------------------------------------
-- Ludo --------------------------------------------
----------------------------------------------------

target("ludo", function()
    set_kind("shared")
    set_group("ludo/source")

    add_deps("licht.engine.core", "licht.engine.rhi", "licht.engine.rhi.vulkan")

    add_includedirs(unpack({
        "source/licht-engine/core",
        "source/licht-engine/rhi",
        "source/licht-engine/launcher",
        "source/ludo/ludo"
    }))

    add_files("source/ludo/ludo/**.cpp")
    add_headerfiles("source/ludo/ludo/**.hpp")
    
    add_defines("LICHT_LUDO_EXPORTS")
end)

target("ludo.app", function()
    set_kind("binary")
    set_group("ludo/app")

    add_defines("LICHT_MAIN")

    add_deps("ludo", "licht.engine.core")
    
    add_includedirs(unpack({
        "source/licht-engine/core",
        "source/licht-engine/rhi",
        "source/licht-engine/launcher",
        "source/ludo/ludo",
        "source/ludo/app"
    }))

    add_files("source/ludo/app/**.cpp")
    add_headerfiles("source/ludo/app/**.hpp")

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
