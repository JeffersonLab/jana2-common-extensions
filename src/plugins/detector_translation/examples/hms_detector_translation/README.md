# hms_detector_translation Plugin

This setup plugin registers HMS detector translators and its detector-mapping
catalog. Source mappings live under `config/detector_mappings/` and install to
`config/<namespace>/hms_detector_translation/detector_mappings/`.

This optional setup plugin registers HMS hodoscope translation routes. It
depends on the generic `detector_translation` plugin and common FADC raw-hit
types, both of which it requests automatically.

Each of its six translation routes registers a CSV header and typed row-writing function through
`JEventService_DetectorTranslatorsMap`. One `InitHMSHodoscopeTranslators` call
registers all conversions and formatting functions during plugin loading. Registration
opens no files; load `hms_detector_translation,detector_translation_dump`
to write the CSVs under `HMS_HODOSCOPE/<RawHitType>.csv`.
The schemas and row formatting live beside the translators in
`HMSHodoscope/FADC/FADCDumpWriter.cc` and
`HMSHodoscope/FADCScaler/FADCScalerDumpWriter.cc`.
