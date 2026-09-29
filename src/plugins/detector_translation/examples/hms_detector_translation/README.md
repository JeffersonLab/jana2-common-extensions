# HMS detector translation plugin

This optional plugin contains the HMS hodoscope translation routes and DigiHit
types. It depends on `detector_translation`, which it requests automatically.
It lives under `detector_translation/examples/` as a reference, not as part
of the generic `detector_translation` plugin. Other DAQ setups should follow
this pattern in their own repositories instead of adding detector identities
to `evio_parser`.
