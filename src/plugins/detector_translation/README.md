# Detector translation plugin

`detector_translation` owns the run-aware electronics-to-detector mapping
service and the generic translation processor. It does not register any
detector or experiment-specific routes.

Load this plugin together with one or more setup plugins which register
translators with `JEventService_DetectorTranslatorsMap`. Link setup code to
the `detector_mapping_api` CMake target.
