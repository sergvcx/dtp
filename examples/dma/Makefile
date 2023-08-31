

.PHONY: default
default: build

build:
	nmc-g++ -o main.abs main.cpp -std=c++11 -Tmc12101brd-nmc0.lds -Wl,--whole-archive -lmc12101load_nm -lnm6407_io_nmc -lnm6407int -Wl,--no-whole-archive \
	-L../../lib -L$(MC12101)/lib \
	-I../../include \
	-ldtp-nm

run: build
	mc12101run -p -a0 -b0 main.abs