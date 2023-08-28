

.PHONY: default
default: build-pc build-nm0

configure-pc:
	cmake -B build -A x64

build-pc: configure-pc
	cmake --build build

build-nm0:
	nmc-gcc -o main0.abs nm0/main.cpp -Tnm0/mc12101brd-nmc0.lds -Wl,--whole-archive -lmc12101load_nm -lnm6407_io_nmc -Wl,--no-whole-archive \
	-L../../lib -L$(MC12101)/lib \
	-I../../include \
	-ldtp-nm

run0: build-nm0
	mc12101run -p -a0 -b0 main0.abs