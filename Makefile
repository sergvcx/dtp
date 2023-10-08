build: build_nm build_x86 update

setenv:
	setx DTP "%CD%"

mingw:
	cmake --preset x86-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
	cmake --build build\x86-mingw
	
build_nm:
	cmake --preset nm-release
	cmake --build build/nm/release
	cmake --preset nm-debug
	cmake --build build/nm/debug

build_x86:
	cmake --preset x86-msvc
	cmake --build build/x86-msvc --config Release
	cmake --build build/x86-msvc --config Debug
	

update:
	git submodule update --init --recursive