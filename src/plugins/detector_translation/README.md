# Detector translation plugin

`detector_translation` owns the run-aware electronics-to-detector mapping
service and the generic translation processor. It does not register any
detector or experiment-specific routes.

Load this plugin together with one or more setup plugins which register
translators with `JEventService_DetectorTranslatorsMap`. Link setup code to
the `detector_mapping_api` CMake target.

[`examples/hms_detector_translation/`](examples/hms_detector_translation/)
is an optional, loadable example with HMS hodoscope routes and DigiHit types.
It keeps the plugin name `hms_detector_translation`; loading
`detector_translation` alone does not register those routes. See the
[translator guide](ADDING_TRANSLATOR.md) for setup-owned implementations.
