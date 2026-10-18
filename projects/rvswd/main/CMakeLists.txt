# Deliberately explicit: the pre-existing programmer reference is NEVER linked.
idf_component_register(SRCS "app_main.c" "rvswd_ro.c" "rvswd_diag.c" "rvswd_memory_lab.c" "rvswd_progbuf_lab.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_driver_gpio esp_driver_uart esp_timer vfs)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
if(CONFIG_DIAG_ABSTRACT_MEMORY_LAB)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE RV_ABSTRACT_MEMORY_LAB)
endif()

if(CONFIG_DIAG_PROGBUF_LAB)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE RV_PROGBUF_LAB)
endif()
