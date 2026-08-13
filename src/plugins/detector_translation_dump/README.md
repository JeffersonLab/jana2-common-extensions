# Detector Translation Dump Plugin

This diagnostic plugin writes every published detector DigiHit type to its own
CSV inside one output directory. Each file contains only columns applicable to
that type. Waveform samples and scaler counts use `|` as the separator within
their CSV field.
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

- `hms_fadc_pulse.csv`
- `hms_fadc_waveform.csv`
- `hms_fadc_pulse_integral.csv`
- `hms_fadc_pulse_time.csv`
- `hms_fadc_pulse_peak.csv`
- `hms_fadc_scaler.csv`

A file containing only its header means the input produced no matching
translated hits of that type.
