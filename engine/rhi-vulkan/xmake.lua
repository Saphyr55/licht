
target("licht.rhi.vulkan", function()
    set_kind("shared")
    set_group("engine")

    add_deps("licht.core", "licht.rhi")

    target_files_default({
        public = false
    })

    add_packages("vulkan-headers", {
        public = true
    })

    add_defines("LICHT_RHI_VULKAN_EXPORTS")

    if is_plat("windows") then
        add_defines("VK_USE_PLATFORM_WIN32_KHR")
        add_syslinks("User32")
    end
end)
