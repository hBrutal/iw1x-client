dependencies =
{
	basePath = "./deps"
}

function dependencies.load()
	dir = path.join(dependencies.basePath, "premake/*.lua")
	deps = os.matchfiles(dir)
	for i, dep in pairs(deps) do
		dep = dep:gsub(".lua", "")
		require(dep)
	end
end

function dependencies.imports()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.import()
		end
	end
end

function dependencies.projects()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.project()
		end
	end
end

dependencies.load()

workspace "cod-mod"
startproject "client"
location "./build"
objdir "%{wks.location}/obj"
targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"
configurations {"Debug", "Release"}
language "C++"
cppdialect "C++20"
architecture "x86"
platforms "Win32"
systemversion "latest"
symbols "On"
staticruntime "On"
editandcontinue "Off"
warnings "Extra"
characterset "ASCII"

flags
{
	"NoIncrementalLink",
	"NoMinimalRebuild",
	"MultiProcessorCompile",
	"No64BitChecks"
}

filter "platforms:Win*"
	defines {"_WINDOWS", "WIN32"}
filter {}

filter "configurations:Release"
	optimize "Size"
	buildoptions {"/GL"}
	linkoptions {"/LTCG"}
	defines {"NDEBUG"}
	fatalwarnings { "All" }
filter {}

filter "configurations:Debug"
	optimize "Debug"
	defines {"DEBUG", "_DEBUG"}
filter {}

project "common"
kind "StaticLib"
language "C++"
files {"./src/common/**.hpp", "./src/common/**.cpp"}
includedirs {"./src/common", "%{prj.location}/src"}
resincludedirs {"$(ProjectDir)src"}
dependencies.imports()

project "client"
kind "WindowedApp"
language "C++"
targetname "cod-mod"
pchheader "std_include.hpp"
pchsource "src/client/std_include.cpp"
linkoptions {"/DYNAMICBASE:NO", "/SAFESEH:NO", "/LARGEADDRESSAWARE", "/LAST:._text", "/PDBCompress"}
files {"./src/client/**.rc", "./src/client/**.hpp", "./src/client/**.cpp", "./src/resources/**.*"}
includedirs {"./src/client", "./src/common", "%{prj.location}/src"}
resincludedirs {"$(ProjectDir)src"}
links {"common"}
dependencies.imports()

group "Dependencies"
dependencies.projects()