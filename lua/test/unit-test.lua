package.path = package.path .. ";../?.lua"


require "HelpHandler"

lu = require "luaunit"




local dummy_table = {"test1", "test2"}
local invalid_arg = {}
local dummy_string = "zzz"
local dummy_number = 314159




function test_version()
    lu.assertErrorMsgContains("Version argument given is not a string or number", HelpHandler.version, nil)
    lu.assertErrorMsgContains("Version argument given is not a string or number", HelpHandler.version, true)
    lu.assertErrorMsgContains("Version argument given is not a string or number", HelpHandler.version, print)
    lu.assertErrorMsgContains("Version argument given is not a string or number", HelpHandler.version, dummy_table)
end


function test_name()
    lu.assertErrorMsgContains("App name given, but is not string type", HelpHandler.name, nil)
    lu.assertErrorMsgContains("App name given, but is not string type", HelpHandler.name, true)
    lu.assertErrorMsgContains("App name given, but is not string type", HelpHandler.name, print)
    lu.assertErrorMsgContains("App name given, but is not string type", HelpHandler.name, dummy_table)
    lu.assertErrorMsgContains("App name given, but is not string type", HelpHandler.name, dummy_number)

    lu.assertErrorMsgContains("App name given, but is empty", HelpHandler.name, "")
end


function test_handle_invalid_arguments()
    -- Nil and empty string values simply set the response to a default placeholder dialogue
    lu.assertErrorMsgContains("Invalid help dialogue argument type given, must be nil, or a string", HelpHandler.handle, true)
    lu.assertErrorMsgContains("Invalid help dialogue argument type given, must be nil, or a string", HelpHandler.handle, print)
    lu.assertErrorMsgContains("Invalid help dialogue argument type given, must be nil, or a string", HelpHandler.handle, dummy_table)
    lu.assertErrorMsgContains("Invalid help dialogue argument type given, must be nil, or a string", HelpHandler.handle, dummy_number)
end

function test_handle_valid_arguments()
    lu.assertEquals(HelpHandler.handle(dummy_string), HELP_HANDLER_SUCCESS)
end

function test_handle_return_values()
    arg[1] = "--version"
    lu.assertEquals(HelpHandler.handle(), HELP_HANDLER_VERSION_MATCHED)

    arg[1] = "--help"
    lu.assertEquals(HelpHandler.handle(), HELP_HANDLER_HELP_MATCHED)

    arg[1] = "--help"
    arg[2] = "--version"
    lu.assertEquals(HelpHandler.handle(), HELP_HANDLER_ALL_MATCHED)
end


function test_handle_file()
    lu.assertErrorMsgContains("File name argument given is not a string", HelpHandler.handleFile, nil)
    lu.assertErrorMsgContains("File name argument given is not a string", HelpHandler.handleFile, true)
    lu.assertErrorMsgContains("File name argument given is not a string", HelpHandler.handleFile, print)
    lu.assertErrorMsgContains("File name argument given is not a string", HelpHandler.handleFile, dummy_table)
    lu.assertErrorMsgContains("File name argument given is not a string", HelpHandler.handleFile, dummy_number)
end


--These should go last to avoid messing with previous tests' results
function test_config_invalid_arguments()
    lu.assertErrorMsgContains("Invalid argument type passed. Must be a number", HelpHandler.config, nil)
    lu.assertErrorMsgContains("Invalid argument type passed. Must be a number", HelpHandler.config, true)
    lu.assertErrorMsgContains("Invalid argument type passed. Must be a number", HelpHandler.config, print)
    lu.assertErrorMsgContains("Invalid argument type passed. Must be a number", HelpHandler.config, dummy_table)
    lu.assertErrorMsgContains("Invalid argument type passed. Must be a number", HelpHandler.config, dummy_string)
    
end


function test_config_valid_arguments()
    lu.assertEquals(HelpHandler.config(DISABLE_NO_ARGS_HELP), HELP_HANDLER_SUCCESS)
    lu.assertEquals(HelpHandler.config(DISABLE_EXTRA_STRINGS), HELP_HANDLER_SUCCESS)
    lu.assertEquals(HelpHandler.config(DISABLE_MATCH_HYPHENS), HELP_HANDLER_SUCCESS)
    lu.assertEquals(HelpHandler.config(ENABLE_UNKNOWN_ARGS_HELP), HELP_HANDLER_SUCCESS)
    lu.assertEquals(HelpHandler.config(ENABLE_HYPHENS_ONLY), HELP_HANDLER_SUCCESS)
end



os.exit(lu.LuaUnit.run())
