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
