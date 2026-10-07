# detector_translation Plugin

This optional plugin owns the generic run-aware mapping service, type-erased
translator registry, and detector DigiHit processor. It contains no detector-
or hardware-specific routes. Setup plugins register those routes through
`JEventService_DetectorTranslatorsMap`.

Generic detector addresses and translation tables live in `detector_mapping/`.
The run-aware table and translator registry live in `services/`. The plugin
exports them to setup plugins through `detector_mapping_api`.
`JEventProcessor_DetectorDigiHits` is the plugin's primary event-processing
component and therefore lives directly in this directory.

Setup plugins provide a registration `JService` during plugin loading. In its
`Init()`, register configuration roots through a
`Service<JEventService_DetectorMappingCatalogs>` dependency and translators
through a `Service<JEventService_DetectorTranslatorsMap>` dependency.
Use `service()` to obtain a reference and `service->` to call methods.

The first `getTable()` request freezes catalogs and loads all run tables once,
after JANA has initialized all registration services. Concurrent first requests
share synchronized initialization; subsequent lookups read immutable tables.
Mapping errors therefore surface on the first translation event.
If no catalogs or explicit mapping directory are supplied, the service warns
once and supplies an empty table for every run, allowing raw-hit processing to
continue without translated DigiHits. Terminal warnings are yellow; redirected
logs stay plain text. Invalid configured mappings remain errors.

Each directory contains its own root `manifest.map`. The translation-table
service merges detectors from all providers and rejects duplicate provider or
detector names. Supplying `TRANSLATION:DIRECTORY` bypasses these providers and
loads that single directory instead.

Every route registered with `JEventService_DetectorTranslatorsMap` owns exactly
one typed CSV row-writing function. Register the raw type, output type,
conversion function, CSV header, and row writer together. Conversion returns one DigiHit;
core publishes it to the normal untagged event collection and records its
route provenance for dumping. The service owns a `DetectorTranslationDump` helper for dump metadata,
path generation, collision checks, and event-writing callbacks. There is no
separate writer service.
Only the optional `detector_translation_dump` processor opens files.
See the [writer registration guide](../detector_translation_dump/README.md).
