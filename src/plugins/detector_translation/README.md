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

Setup plugins register configuration roots through
`JEventService_DetectorMappingCatalogs` during plugin loading:

```cpp
app->AddPlugin("detector_translation");
app->GetService<JEventService_DetectorMappingCatalogs>()->addCatalog(
    "my_experiment",
    "/installed/path/to/detector_mappings");
```

Each directory contains its own root `manifest.map`. The translation-table
service merges detectors from all providers and rejects duplicate provider or
detector names. Supplying `TRANSLATION:DIRECTORY` bypasses these providers and
loads that single directory instead.
