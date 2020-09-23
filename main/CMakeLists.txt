idf_component_register(SRCS 
                            "main.c"
                            "storage/sd-storage.c"
                            "bluetooth/bt_app_core.c"
                            "bluetooth/bt_module.c"
                            "display/u8g2_esp32_hal.c"
                            "display/display_module.c"
                            "menu/menu_module.c"
                    INCLUDE_DIRS "." "includes/" )
