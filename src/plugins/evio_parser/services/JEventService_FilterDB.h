#pragma once

#include <JANA/JService.h>
#include <JANA/JException.h>

#include <array>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>              // I/O manipulators for formatting
#include <algorithm>

#include "jce_config_paths.h"

/**
 * @class JEventService_FilterDB
 * @brief JANA service providing ROC/slot/module/bank allow-list filtering
 * 
 * This service maintains a filter database loaded from a text file that specifies
 * which ROC IDs, slots, modules, and banks are allowed for processing. When filtering
 * is enabled, only data matching the allow-list will be processed. If filtering is
 * disabled or no filter database is loaded, all data is allowed.
 * 
 * Filter file format (one entry per line):
 *   rocid slot module bank
 * 
 * Example:
 *   # rocid  slot  module  bank
 *   21  3  250  250
 *   22  5  250  250
 */
class JEventService_FilterDB : public JService {

private:
    /**
     * @brief Filter database data structure
     * 
     * Nested map structure: rocid -> ("banks" | "slots" | "modules") -> list of allowed values
     * 
     * Example:
     *   data[21]["banks"] = {250, 251}
     *   data[21]["slots"] = {3}
     *   data[21]["modules"] = {250}
     */
    std::map<int, std::map<std::string, std::vector<int>>> data;

    std::set<std::array<int, 4>> m_rows;
    Parameter<std::string> m_core_config_dir {this, "JCE:CORE_CONFIG_DIR",
        JCE_INSTALL_CONFIG_DIR, "Core configuration directory", true};

    void printSummaryTable(std::ostream& os) const;
    void fillDB(const std::string& filename);

public:

    /**
     * @brief Filtering control parameters
     *
     * The JANA Parameter constructor params order and description are:
     * - owner: this service (for automatic registration)
     * - name: shared parameter name used on the command line / config
     * - default_value: default if not specified
     * - description: help text
     * - is_shared: true so the name is used as-is (no prefix added)
     *
     * Usage from the command line:
     *   -PFILTER:ENABLE=1 -PFILTER:FILE=my_filter.db
     */
    Parameter<bool> m_filter_enable {this, "FILTER:ENABLE", false,
                                     "Enable ROC/bank filtering using FILTER:FILE (true/false, default false)", true};
    Parameter<std::string> m_filter_file {this, "FILTER:FILE",
                                          "",
                                          "Core filter file override (directory additions still apply), lines: 'rocid slot module bank'", true};

    void Init() override;
   
    bool isROCAllowed(int rocid);

    bool isBankAllowed(int rocid, int bank);

    bool isSlotAllowed(int rocid, int slot);

    bool isModuleAllowed(int rocid, int module);
};
