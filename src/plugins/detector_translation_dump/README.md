# Detector Translation Dump Plugin

This diagnostic plugin writes translated `HMSHodoscopeFADCPulseDigiHit` objects to
CSV.
Load it after `evio_parser` so detector translation runs before the diagnostic
processor:

```bash
jana \
  -Pplugins=evio_parser,detector_translation_dump \
  -PTRANSLATION:DIRECTORY=/path/to/config/evio_parser/detector_mappings \
  -Pdetector_translation_dump:OUTPUT=detector_translation_dump.csv \
  /path/to/input.evio
```

The current HMS mapping expects an FADC pulse at DAQ address `(1, 3, 0)` and
translates it to `(plane=1, bar=1, signal=0)`. A CSV containing only its header
means the input produced no matching translated hits.
