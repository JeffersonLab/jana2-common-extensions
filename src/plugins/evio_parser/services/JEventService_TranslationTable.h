#pragma once

#include <JANA/JService.h>

#include <cstdint>
#include <memory>
#include <string>

#include "TranslationTable.h"
#include "jce_config_paths.h"

class JEventService_TranslationTable : public JService {
public:
    explicit JEventService_TranslationTable(
        std::string default_mapping_directory = jce_config_path(
            "evio_parser/detector_mappings",
            "TRANSLATION:DIRECTORY"));

    void Init() override;

    std::shared_ptr<const TranslationTable> GetTable(std::uint64_t run_number) const;

private:
    Parameter<std::string> m_mapping_directory;
    std::shared_ptr<const TranslationTable> m_table;
};
