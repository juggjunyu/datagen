set_project("subdiv-ccd")
set_languages("cxxlatest")
set_optimize("faster")

add_rules("mode.debug", "mode.release")
set_defaultmode("debug")

add_requires("eigen","fmt")
add_requires("nlohmann_json")
add_requires("spdlog 1.13.0")

target("rational")
    set_kind("static")
    add_includedirs("rational-cpp/src", {public = true})
    add_includedirs("gmp/include", {public = true})
    add_linkdirs("gmp/lib")
    add_links("libgmp-3")
    add_headerfiles("gmp/include/*.h")
    add_headerfiles("rational-cpp/src/**.hpp")
    add_files      ("rational-cpp/src/**.cpp")

target("ccd_io")
    set_kind("static")
    add_deps("rational")
    add_packages("spdlog", { public = true })
    add_packages("nlohmann_json", { public = true })
    add_includedirs("CCD-Query-IO/src", {public = true})
    add_headerfiles("CCD-Query-IO/src/**.hpp")
    add_files      ("CCD-Query-IO/src/**.cpp")

target("generator")
    set_kind("binary")
    add_deps("ccd_io")
    add_packages("eigen","fmt", {public = true})
    add_files("generator/**.cpp")
    add_ldflags("-static", {force = true})
    --  if is_mode("debug") then
    --     add_cxflags("/Zi")  -- 生成 PDB 文件
    --     add_ldflags("/DEBUG")
    -- end
    -- if is_mode("debug") then
    -- add_cxflags("-pg")
    -- add_ldflags("-pg")
    -- end
    
toolchain("mingw")
    set_toolset("cc", "gcc")
    set_toolset("cxx", "g++")
    set_toolset("ld", "g++")

-- target("benchmark")
--     set_kind("binary")
--     add_deps("ccd_io")
--     add_packages("eigen","fmt", {public = true})
--     add_files("benchmark/**.cpp")

-- target("rationalBenchmark")
--     set_kind("binary")
--     add_deps("ccd_io")
--     add_packages("eigen","fmt", {public = true})
--     add_files("rationalBenchmark/**.cpp")

        