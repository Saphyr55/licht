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

includes("scripts/xmake/**.lua")

add_requires("libsdl3", "catch2", "vulkan-headers", "lua")

----------------------------------------------------
-- Engine Modules ----------------------------------
----------------------------------------------------
target("licht.core", function()
    set_kind("shared")
    set_group("engine/licht/modules")

    add_packages("libsdl3", "lua")

    add_includedirs("engine/licht/core")

    add_headerfiles("engine/licht/core/**.hpp")
    add_files("engine/licht/core/**.cpp")

    add_defines("LICHT_CORE_EXPORTS")
end)

target("licht.rhi", function()
    set_kind("shared")
    set_group("engine/licht/modules")

    add_deps("licht.core")

    add_includedirs("engine/licht/rhi", "engine/licht/core")

    add_headerfiles("engine/licht/rhi/**.hpp")
    add_files("engine/licht/rhi/**.cpp")

    add_defines("LICHT_RHI_EXPORTS")
end)

target("licht.rhi.vulkan", function()
    set_kind("shared")
    set_group("engine/licht/modules")

    add_deps("licht.core", "licht.rhi")

    add_includedirs(unpack({
        "engine/licht/rhi_vulkan",
        "engine/licht/core",
        "engine/licht/rhi"
    }))

    add_headerfiles("engine/licht/rhi_vulkan/**.hpp")
    add_files("engine/licht/rhi_vulkan/**.cpp")

    add_packages("vulkan-headers", {
        public = true
    })

    add_defines("LICHT_RHI_VULKAN_EXPORTS")

    if is_plat("windows") then
        add_defines("VK_USE_PLATFORM_WIN32_KHR")
        add_syslinks("User32")
    end
end)

target("licht.launcher", function()
    set_kind("shared")
    set_group("engine/licht/modules")

    add_deps("licht.core")

    add_includedirs(unpack({
        "engine/licht/core",
        "engine/licht/launcher"
    }))

    add_headerfiles("engine/licht/launcher/**.hpp")
    add_files("engine/licht/launcher/**.cpp")

    add_defines("LICHT_LAUNCHER_EXPORTS")

    if is_plat("windows") then
        add_syslinks("User32")
    end
end)

----------------------------------------------------
-- Tests -------------------------------------------
----------------------------------------------------
target("licht.core.tests", function()
    set_kind("binary")
    set_group("engine/licht/tests")

    add_deps("licht.core")

    add_packages("catch2")

    add_includedirs("engine/licht/core")

    add_files("tests/licht/core/**.cpp")
end)

----------------------------------------------------
-- Ludo --------------------------------------------
----------------------------------------------------
target("ludo", function()
    set_kind("shared")
    set_group("ludo/modules")

    add_deps("licht.core", "licht.rhi", "licht.rhi.vulkan")

    add_includedirs(unpack({
        "engine/licht/core",
        "engine/licht/rhi",
        "engine/licht/launcher",
        "samples/ludo/ludo"
    }))

    add_files("samples/ludo/ludo/**.cpp")
    add_headerfiles("samples/ludo/ludo/**.hpp")

    add_defines("LICHT_LUDO_EXPORTS")
end)

target("ludo.app", function()
    set_kind("binary")
    set_group("ludo/app")

    local ludo_projectdir = path.join(os.projectdir(), "samples/ludo")
    local licht_enginedir = path.join(os.projectdir(), "engine/licht")

    set_runargs("--projectdir", ludo_projectdir, "--enginedir", licht_enginedir)

    add_deps("ludo", "licht.core")

    add_includedirs(unpack({
        "engine/licht/core",
        "engine/licht/rhi",
        "engine/licht/launcher",
        "samples/ludo/ludo",
        "samples/ludo/app"
    }))

    add_files("samples/ludo/app/**.cpp")
    add_headerfiles("samples/ludo/app/**.hpp")

    after_build(function(target)
        import("core.base.task")
        import("core.project.project")

        task.run("compile-shaders", {
            output = path.join(target:targetdir(), "shaders"),
            files = {
                "shaders/main.frag",
                "shaders/main.vert"
            }
        })

    end)

end)
