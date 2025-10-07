local tests_passed = 0
local tests_failed = 0

-- A simple function to run and report tests
function test(description, func)
    local success, err = pcall(func)
    if success then
        print(string.format("  ✅ PASS: %s", description))
        tests_passed = tests_passed + 1
    else
        print(string.format("  ❌ FAIL: %s", description))
        print(string.format("     └─> Error: %s", tostring(err)))
        tests_failed = tests_failed + 1
    end
end

-- ============================================================
--                 THE TESTS
-- ============================================================

function RunAllTests()
    print("\n-- 🧪 Testing Primitive & Pointer Constructors --")
    test("IntPtr() creates a pointer with the correct value", function()
        local p = IntPtr(42)
        assert(ffi.istype("int[1]", p))
        assert(p[0] == 42)
    end)
    test("FloatPtr() creates a pointer with the default value", function()
        local p = FloatPtr()
        assert(ffi.istype("float[1]", p))
        assert(p[0] == 0.0)
    end)
    test("BoolPtr() correctly stores a boolean value", function()
        local p = BoolPtr(true)
        assert(p[0] == true)
        p[0] = false
        assert(p[0] == false)
    end)

    ----------------------------------------------------------------

    print("\n-- 🧪 Testing Array Constructors --")
    test("Float4() creates a float[4] array", function()
        local arr = Float4(0.1, 0.2, 0.3, 0.4)
        assert(ffi.istype("float[4]", arr))
        assert(math.abs(arr[0] - 0.1) < 1e-6)
        assert(math.abs(arr[3] - 0.4) < 1e-6)
    end)
    ----------------------------------------------------------------

    print("\n-- 🧪 Testing Utility Functions --")
    test("FromPtr() correctly extracts a value from an IntPtr", function()
        local p = IntPtr(123)
        local val = FromPtr(p)
        assert(val == 123)
    end)

    ----------------------------------------------------------------

    print("\n-- 🧪 Testing CString Wrapper --")
    test("CString() creates an object with correct initial values", function()
        local str_obj, len = CString("hello", 10)
        assert(type(str_obj) == "table")
        assert(len == 10)
        assert(str_obj:get() == "hello")
    end)
    test("CString:set() correctly modifies the buffer", function()
        local str_obj, len = CString("initial", 20)
        str_obj:set("changed")
        assert(str_obj:get() == "changed")
    end)
    test("CString:set() respects buffer length (safety check)", function()
        local str_obj, len = CString("", 5) -- Buffer can hold "1234" + null
        str_obj:set("123456789")
        assert(str_obj:get() == "1234")
    end)
    test("CString __tostring allows direct printing", function()
        local str_obj = CString("printable")
        assert(tostring(str_obj) == "printable")
    end)

    ----------------------------------------------------------------

    print("\n-- 🧪 Testing ImVec2 Wrapper --")
    test("ImVec2() component access and modification works", function()
        local v = ImVec2(10, 20)
        assert(v.x == 10)
        v.y = 25
        assert(v.y == 25)
    end)
    test("ImVec2 __tostring formats correctly", function()
        local v = ImVec2(5.5, -3.1)
        assert(tostring(v) == "ImVec2(5.50, -3.10)")
    end)
    test("ImVec2 vector addition works", function()
        local v1 = ImVec2(10, 20)
        local v2 = ImVec2(5, 5)
        local v3 = v1 + v2
        assert(v3.x == 15 and v3.y == 25)
    end)
    test("ImVec2 scalar multiplication works", function()
        local v = ImVec2(10, -5)
        local v_scaled = v * 3
        local v_scaled_rev = 3 * v
        assert(v_scaled.x == 30 and v_scaled.y == -15)
        assert(v_scaled_rev.x == 30 and v_scaled_rev.y == -15)
    end)
    test("ImVec2 dot product works", function()
        local v1 = ImVec2(3, 4)
        local v2 = ImVec2(5, 2)
        local dot = v1 * v2
        assert(dot == (3*5 + 4*2))
    end)
    test("ImVec2 :cdata() returns the correct pointer type", function()
        local v = ImVec2()
        assert(ffi.istype("ImVec2_C[1]", v:cdata()))
    end)

    ----------------------------------------------------------------

    print("\n-- 🧪 Testing ImVec4 Wrapper --")
    test("ImVec4() component access works", function()
        local v = ImVec4(1, 2, 3, 4)
        assert(v.x == 1 and v.y == 2 and v.z == 3 and v.w == 4)
    end)
    test("ImVec4 vector subtraction works", function()
        local v1 = ImVec4(10, 10, 10, 10)
        local v2 = ImVec4(1, 2, 3, 4)
        local v3 = v1 - v2
        assert(v3.x == 9 and v3.y == 8 and v3.z == 7 and v3.w == 6)
    end)
    test("ImVec4 dot product works", function()
        local v1 = ImVec4(1, 2, 3, 4)
        local v2 = ImVec4(5, 6, 7, 8)
        local dot = v1 * v2
        assert(dot == (1*5 + 2*6 + 3*7 + 4*8))
    end)

    ----------------------------------------------------------------
    
    print("\n-- 📊 Test Summary --")
    print(string.format("  Total Passed: %d", tests_passed))
    print(string.format("  Total Failed: %d", tests_failed))
    if tests_failed > 0 then
        print("\n  ⚠️ Some tests failed!")
    else
        print("\n  🎉 All tests passed successfully!")
    end
end

return {RunAllTests = RunAllTests}
