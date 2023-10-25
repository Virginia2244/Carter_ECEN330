add_executable(lab5.elf main.c)
target_link_libraries(lab5.elf ${330_LIBS} intervalTimer interrupts touchscreen)
set_target_properties(lab5.elf PROPERTIES LINKER_LANGUAGE CXX)