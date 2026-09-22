#pragma once

#include "BankParser.h"

// Compatibility alias for downstream parsers. New code should derive from
// BankParser because EVIO data blocks are not necessarily electronics modules.
using ModuleParser = BankParser;
