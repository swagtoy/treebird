solution "Treebird"
configurations { "Debug", "Release" };

-- BEGIN Mastodont project
project("treebird");
kind("ConsoleApp");
language("C");
cdialect("C99");
files { "src/*.h", "src/*.c" };
includedirs { "include/" };

configuration { "linux", "bsd", "gmake" };
linkoptions { "`pkg-config --libs mastodont`" };

configuration { "Debug" };
defines { "DEBUG" };
flags("Symbols");

configuration { "Release" };
defines { "NDEBUG" };
flags("Optimize");
-- END Mastodont-c

local prefix = os.getenv("PREFIX") or "/usr/local";

newaction {
	trigger = "install",
	description = "install binary",
	execute = function()
		os.copyfile("treebird", prefix .. "/bin");
		os.mkdir(prefix .. "/share/treebird");
		os.execute("install -d dist/ " .. prefix .. "/share/treebird");
	end
}