idf_component_register(SRCS 
        "main.c" 
        "../components/wifi_app/wifi_app.c" 
        "../components/http_server/http_server.c" 
        "../components/adc/adc.c" 
        "../components/uart/uart.c" 
        "../components/led_strip/led_strip.c" 
        "../components/led_strip_encoder/led_strip_encoder.c" 
    INCLUDE_DIRS 
        "." 
        "../components/wifi_app"
        "../components/http_server"
        "../components/adc"
        "../components/uart"
        "../components/tasks_common"
        "../components/led_strip"
        "../components/led_strip_encoder"
                    EMBED_FILES ../webpage/app.css ../webpage/app.js ../webpage/favicon.ico ../webpage/index.html)
