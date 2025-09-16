local _modules = {
    {
        name = "licht.engine.core",
        version = "0.0.1-dev",
        dependencies = {}
    },
    {
        name = "licht.engine.rhi",
        version = "0.0.1-dev",
        dependencies = {}
    },
    {
        name = "licht.engine.rhi.vulkan",
        version = "0.0.1-dev",
        dependencies = {
            "licht.engine.rhi"
        }
    }
}

return {
    modules = _modules
}
