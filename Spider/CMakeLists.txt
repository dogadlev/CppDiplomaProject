cmake_minimum_required(VERSION 3.20)

add_executable(main 
    main.cpp 
    ../ConfigParser/config.h 
    ../ConfigParser/config.cpp
    )

target_compile_features(main PRIVATE cxx_std_17)