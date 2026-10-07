# evio_common_modules Plugin

This optional plugin registers JCE's reusable CAEN, FADC, scaler, helicity,
MPD, and VFTDC bank parsers with `evio_parser`. It requests the
core plugin automatically.

Experiment plugins can register additional parsers through the installed
`evio_parser_api` without modifying this plugin. Parser plugins provide a JANA
service which adds shared `ModuleParser` instances during initialization;
duplicate IDs and registration after parsing begins are rejected.
