add_jana_plugin(evio_common_modules
    SOURCES
        InitPlugin.cc
        ../evio_parser/module_parsers/InitModuleParsers.cc
)

target_link_libraries(evio_common_modules
    PRIVATE
        ${JANA_LIBRARY}
        evio_parser_api
        caen1190_parser
        fadc_parser
        fadc_scaler_parser
        tiscaler_parser
        helicity_decoder_parsers
        mpd_parser
        vftdc_parser
)
