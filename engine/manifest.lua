local _modules = {
    {
        name = "licht.core",
        version = "0.0.1-dev",
        dependencies = {}
    },
    {
        name = "licht.rhi",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core"
        }
    },
    {
        name = "licht.engine",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core"
        }
    },
    {
        name = "licht.launcher",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core",
            "licht.engine"
        }
    },
    {
        name = "licht.rhi.vulkan",
        version = "0.0.1-dev",
        dependencies = {
            "licht.core",
            "licht.rhi"
        }
    }
}

return {
    modules = _modules
}
