

.PHONY: default
default: build0 build1

build0:
	nmc-g++ -o main0.abs main.cpp -std=c++11 -Tmc12101brd-nmc0.lds \
	-Wl,--whole-archive -lmc12101load_nm -lnm6407_io_nmc -lnm6407int -Wl,--no-whole-archive \
	-D CORE0 \
	-L../../lib -L$(MC12101)/lib \
	-I../../include \
	-ldtp-nm

build1:
	nmc-g++ -o main1.abs main.cpp -std=c++11 -Tmc12101brd-nmc1.lds \
	-Wl,--whole-archive -lmc12101load_nm -lnm6407_io_nmc -lnm6407int -Wl,--no-whole-archive \
	-L../../lib -L$(MC12101)/lib \
	-I../../include \
	-ldtp-nm

run0: build0
	mc12101run -p -a0 -b0 main0.abs

run1: build1
	mc12101run -p -a1 -b0 main1.abs