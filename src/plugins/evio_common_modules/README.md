# Common EVIO Module Parsers

`evio_common_modules` registers reusable electronics-bank parsers with the
registries provided by `evio_parser`. It requests `evio_parser` automatically.

Load it for CAEN1190, FADC, FADC scaler, TI scaler, helicity, MPD, and VFTDC
banks. Experiment-owned formats should be built and registered by an
experiment plugin instead of being added here.
