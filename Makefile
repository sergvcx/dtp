build: build_nm build_x86

setenv:
	setx DTP "%CD%"
	
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