-- imgui_bootstrap.lua

local ffi = require("ffi")
local lib = ffi.load("imgui_lua_bindings")

local header_file = io.open("imgui_gen.h", "r")
if not header_file then
    error("Could not open imgui_gen.h!")
    return
end
local header_content = header_file:read("*a")
header_file:close()

-- local imvector_definition = [[
--     typedef struct ImVector {
--         int   Size;
--         int   Capacity;
--         void* Data;
--     } ImVector;
-- ]]
-- local full_cdef = imvector_definition .. header_content
ffi.cdef(header_content)

-- Load the wrapper module and initialize it
_G.ImGui = require("imgui_gen")
_G.ImGui.init(lib) -- This now sets up all the functions and enums

print("ImGui Lua bindings initialized successfully!")