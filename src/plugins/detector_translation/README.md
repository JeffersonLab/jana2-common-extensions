# detector_translation Plugin

This optional plugin owns the generic run-aware mapping service, type-erased
translator registry, and detector DigiHit processor. It contains no detector-
or hardware-specific routes. Setup plugins register those routes through
`JEventService_DetectorTranslatorsMap`.
