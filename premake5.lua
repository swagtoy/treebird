if not os.isfile('config.h') then
	print("Note: `config.h' not found, until a new Treebird release comes out,");
	print("      please copy `config.def.h' and edit it.");
	os.exit(1);
end

workspace "Treebird";
configurations { "Debug", "Release" };

local c_files = {
	'main.c',
	'types.c',
	'cookie.c',
	'easprintf.c',
	'error.c',
	'memory.c',
	'helpers.c',
	'key.c',
	'local_config.c',
	'local_config_set.c',
	'query.c',
	'string.c',
	'string_helpers.c',
	'request.c',
	'session.c',
	'path.c',
	'mime.c',
};

-- Not actually real 'pages', but the ones we compile in / create definitions
-- This setup is pretty jank, but it helped me transition the Async refactor
local pages = {
	'about.c',
	'account.c',
	'applications.c',
	'attachments.c',
	'base_page.c',
	'conversations.c',
	'emoji.c',
	'emoji_reaction.c',
	'global_cache.c',
	'global_perl.c',
	'hashtag.c',
	'http.c',
	'index.c',
	'lists.c',
--	'login.c',
--	'memory_page.c',
	'notifications.c',
--	'page_config.c',
	'scrobble.c',
--	'search.c',
	'status.c',
	'timeline.c',
};

local definitions = {};

-- Create definitions for page enabling
for i=1, #pages  do
	local def = 'CMP_ENABLE_' .. string.upper(string.sub(pages[i], 0, -3));
	table.insert(definitions, def);
end

-- Prepend with 'src/'
for i=1, #c_files do c_files[i] = 'src/'.. c_files[i] end
for i=1, #pages   do pages[i] = 'src/'.. pages[i] end

-- BEGIN Mastodont project
project("treebird");
kind("ConsoleApp");
language("C");
-- Merge pages into c_files
for _, v in ipairs(pages) do
	table.insert(c_files, v);
end
files(c_files);
includedirs { "include/" };
defines(definitions);

-- For some reason this one doesn't have a pkg-config file
local libfcgi = os.findlib("libfcgi");
if not libfcgi then
	print("Couldn't find libfcgi (aka fcgi-devkit). Probably gonna need that.\n");
	os.exit(1);
end

filter { "action:gmake*" };
	linkoptions{ "`pkg-config --libs mastodont` `perl -MExtUtils::Embed -e ldopts`" };
	buildoptions{ "`pkg-config --cflags mastodont` `perl -MExtUtils::Embed -e ccopts`" };
	links{"fcgi"};

filter { "toolset:clang" };
	buildoptions{"-Wno-compound-token-split-by-macro"};

filter { "configurations:Debug" };
defines { "DEBUG" };
symbols("On");

filter { "configurations:Release" };
defines { "NDEBUG" };
optimize("On");
-- END Mastodont-c

local prefix = os.getenv("PREFIX") or "/usr/local";

newaction {
	trigger = "install",
	description = "install binary",
	execute = function()
		os.copyfile("treebird", prefix .. "/bin");
		os.mkdir(prefix .. "/share/treebird");
		os.execute("cp -r dist/ " .. prefix .. "/share/treebird/dist/");
		os.execute("cp -r perl/ " .. prefix .. "/share/treebird/perl/");
	end
}
