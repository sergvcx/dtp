build: build_nm build_x86 update

setenv:
	setx DTP "%CD%"
	
build_nm:
	cmake --preset nm-release
	cmake --build build/nm/release

build_x86:
	cmake --preset x86-msvc
	cmake --build build/x86-msvc
	

update:
	git submodule update --init --recursive