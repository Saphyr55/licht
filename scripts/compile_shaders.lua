local glslc = "glslc"

task("compile-shaders")

    on_run(function (target)
        import("core.base.option")

        local files = option.get("files")
        local output = option.get("output")

        cprint("${green}Compiling shaders...")
        print("output = %s", output)

        os.mkdir(output)

        for _, filepath in pairs(files) do
            cprint("${green}Compiling: %s ... ", filepath)
            local filename = path.filename(filepath)
            os.vrunv(glslc, {filepath, "-o", path.join(output, filename .. ".spv")})
        end

    end)
