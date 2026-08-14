# Detector Translation Dump Plugin

This diagnostic plugin groups published DigiHits by detector subdirectory and
writes each DigiHit type to its own CSV. Each file contains only columns
applicable to that type. Waveform samples and scaler counts use `|` as the
separator within their CSV field. Channel-addressed files keep the DAQ address
columns together as `rocid,slot,channel` before `module_id` and detector fields.
Load it after `evio_parser` so detector translation runs before the diagnostic
processor:

```bash
jana \
  -Pplugins=evio_parser,detector_translation_dump \
  -PTRANSLATION:DIRECTORY=/path/to/config/evio_parser/detector_mappings \
  -Pdetector_translation_dump:OUTPUT_DIRECTORY=detector_translation_dump \
  /path/to/input.evio
```

The current HMS mapping expects FADC data at DAQ address `(1, 3, 0)` and
translates it to `(plane=1, bar=1, signal=0)`. The output directory contains:

```text
hms_hodoscope/
├── fadc_pulse.csv
├── fadc_waveform.csv
├── fadc_pulse_integral.csv
├── fadc_pulse_time.csv
├── fadc_pulse_peak.csv
└── fadc_scaler.csv
```

A file containing only its header means the input produced no matching
translated hits of that type.
