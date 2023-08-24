cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

add_executable(test-nm6407-dma dma.cpp)
add_executable(test-nm6407-half-duplex-input half-duplex-test.cpp)
add_executable(test-nm6407-half-duplex-output half-duplex-test.cpp)
add_executable(test-nm6407-buffer-nm0 buffer.cpp)
add_executable(test-nm6407-buffer-nm1 buffer.cpp)

set(targets 
    test-nm6407-dma
    test-nm6407-half-duplex-input
    test-nm6407-half-duplex-output
    test-nm6407-buffer-nm0
    test-nm6407-buffer-nm1
)
target_compile_definitions(test-nm6407-half-duplex-input PUBLIC INPUT)
target_compile_definitions(test-nm6407-half-duplex-output PUBLIC OUTPUT)

target_compile_definitions(test-nm6407-buffer-nm0 PUBLIC LISTEN)
target_link_options(test-nm6407-half-duplex-input PUBLIC -Wl,-T${CMAKE_CURRENT_LIST_DIR}/mc12101brd-nm0.lds	)
target_link_options(test-nm6407-half-duplex-output PUBLIC -Wl,-T${CMAKE_CURRENT_LIST_DIR}/mc12101brd-nm1.lds	)
target_link_options(test-nm6407-dma PUBLIC -Wl,-T${CMAKE_CURRENT_LIST_DIR}/mc12101brd-nm0.lds	)
target_link_options(test-nm6407-buffer-nm0 PUBLIC -Wl,-T${CMAKE_CURRENT_LIST_DIR}/mc12101brd-nm0.lds	)
target_link_options(test-nm6407-buffer-nm1 PUBLIC -Wl,-T${CMAKE_CURRENT_LIST_DIR}/mc12101brd-nm1.lds	)

foreach(target ${targets})
    target_include_directories(${target}
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/../../include
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/../../nmassert/include
            $ENV{MC12101}/include
            )

    target_link_directories(${target}
        PRIVATE
        $ENV{MC12101}/lib
        )
    target_link_libraries(${target}
        PRIVATE
            dtp::dtp
    )

    target_link_options(${target} PUBLIC
    	-Xlinker -Map=${target}.map
        -Wl,--whole-archive 
	        -lmc12101load_nm 
		    -lnm6407int
		    -lnm6407_io_nmc
		    #-lmc12101_stub_nmc4_fixed
        -Wl,--no-whole-archive)

    add_test(NAME ${target}
    COMMAND mc12101run -p -a0 ${target})
endforeach()


