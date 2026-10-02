# hms_detector_translation Plugin

This setup plugin registers HMS detector translators and its detector-mapping
catalog. Source mappings live under `config/detector_mappings/` and install to
`config/<namespace>/hms_detector_translation/detector_mappings/`.

This optional setup plugin registers HMS hodoscope translation routes. It
depends on the generic `detector_translation` plugin and common FADC raw-hit
types, both of which it requests automatically.
