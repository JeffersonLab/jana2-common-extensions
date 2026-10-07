#include "JEventService_BankToModuleMap.h"
#include "JEventService_FilterDB.h"

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unistd.h>

namespace fs = std::filesystem;

void write(const fs::path& path, const std::string& contents) {
    std::ofstream file(path);
    file << contents;
    assert(file.good());
}

void configure(JService& service, const std::map<std::string, std::string>& parameters) {
    for (auto* parameter : service.GetAllParameters()) parameter->Wire({}, parameters);
}

template<typename F> void rejects(F action) {
    bool rejected = false;
    try { action(); }
    catch (const JException&) { rejected = true; }
    assert(rejected);
}

int main() {
    char temporary[] = "/tmp/jce-config-layers-XXXXXX";
    const auto directory = mkdtemp(temporary);
    assert(directory);
    const fs::path root(directory), core = root / "core", first = root / "first",
                   second = root / "second", optional = root / "optional";
    for (const auto& path : {core, first, second, optional}) fs::create_directory(path);
    write(core / "mapping.db", "250 250\n9001 9001\n");
    write(first / "mapping.db", " # comment\n253 595\n250 250\n253 595\n");
    write(second / "mapping.db", "999 250 # intentional override\n");
    write(core / "filter.db", "1 3 250 250\n");
    write(first / "filter.db", " # comment\n1 3 250 250\n2 5 253 595\n");
    write(second / "filter.db", "3 7 9001 9001 # extra\n");
    const std::string directories = ":" + first.string() + ":" +
        (root / "missing").string() + ":" + optional.string() + ":" + second.string() + ":";
    const char* previous_env = std::getenv("JCE_CONFIG_DIR");
    const bool had_env = previous_env != nullptr;
    const std::string saved_env = previous_env ? previous_env : "";
    setenv("JCE_CONFIG_DIR", directories.c_str(), 1);
    const std::map<std::string, std::string> parameters {{"JCE:CORE_CONFIG_DIR", core.string()}};
    std::ostringstream warnings;
    JEventService_BankToModuleMap routes;
    routes.GetLogger().destination = &warnings;
    configure(routes, parameters);
    routes.Init();
    assert(routes.getModuleId(250) == 999);
    assert(routes.getModuleId(595) == 253);
    assert(routes.getModuleId(9001) == 9001);
    assert(warnings.str().find("skipping") != std::string::npos);
    assert(warnings.str().find("250 mapping overridden") != std::string::npos);
    assert(warnings.str().find((core / "mapping.db").string() + ":1") != std::string::npos);
    assert(warnings.str().find((second / "mapping.db").string() + ":1") != std::string::npos);
    assert(warnings.str().find('\033') == std::string::npos);
    rejects([&] { routes.loadMappingFile(core / "mapping.db"); });

    // An explicit file replaces core while directory additions still apply.
    const auto explicit_map = root / "explicit.db";
    write(explicit_map, "777 77\n123 250\n");
    JEventService_BankToModuleMap replacement;
    configure(replacement, {{"JCE:CORE_CONFIG_DIR", (root / "missing").string()},
                            {"BANKMAP:FILE", explicit_map.string()}});
    replacement.Init();
    assert(replacement.getModuleId(77) == 777);
    assert(replacement.getModuleId(250) == 999);
    assert(replacement.getModuleId(595) == 253);
    assert(replacement.getModuleId(9001) == -1);

    write(root / "bad.db", "250 250\n251 250\n");
    JEventService_BankToModuleMap bad;
    rejects([&] { bad.loadMappingFile(root / "bad.db"); });
    for (const auto& row : {"garbage\n", "250 250 trailing\n", "250 65536\n"}) {
        write(root / "bad.db", row);
        rejects([&] { bad.loadMappingFile(root / "bad.db"); });
    }
    // Empty additions are allowed; equal routes deduplicate silently.
    write(root / "empty.db", " # no additions\n");
    bad.loadMappingFile(root / "empty.db");
    bad.loadMappingFile(core / "mapping.db");
    bad.loadMappingFile(core / "mapping.db");
    assert(bad.getModuleId(250) == 250);

    JEventService_FilterDB filter;
    filter.GetLogger().destination = &warnings;
    auto filter_parameters = parameters;
    filter_parameters["FILTER:ENABLE"] = "true";
    configure(filter, filter_parameters);
    std::ostringstream summary;
    auto* output = std::cout.rdbuf(summary.rdbuf());
    filter.Init();
    std::cout.rdbuf(output);
    assert(filter.isBankAllowed(1, 250));
    assert(filter.isBankAllowed(2, 595));
    assert(filter.isBankAllowed(3, 9001));
    assert(!filter.isBankAllowed(2, 250));
    assert(!filter.isROCAllowed(4));
    // Three distinct rows; the duplicate core row must not be printed twice.
    const auto text = summary.str();
    assert(std::count(text.begin(), text.end(), '\n') == 6);

    const auto explicit_filter = root / "explicit_filter.db";
    write(explicit_filter, "7 7 777 77\n");
    JEventService_FilterDB only;
    configure(only, {{"JCE:CORE_CONFIG_DIR", (root / "missing").string()},
                     {"FILTER:ENABLE", "true"}, {"FILTER:FILE", explicit_filter.string()}});
    only.Init();
    assert(only.isBankAllowed(2, 595));
    assert(only.isBankAllowed(3, 9001));
    assert(only.isBankAllowed(7, 77));
    // Clearing additions makes explicit function files the only input.
    setenv("JCE_CONFIG_DIR", "", 1);
    JEventService_BankToModuleMap exclusive_map;
    configure(exclusive_map, {{"BANKMAP:FILE", explicit_map.string()}});
    exclusive_map.Init();
    assert(exclusive_map.getModuleId(250) == 123);
    assert(exclusive_map.getModuleId(595) == -1);
    JEventService_FilterDB exclusive_filter;
    configure(exclusive_filter, {{"FILTER:ENABLE", "true"},
                                 {"FILTER:FILE", explicit_filter.string()}});
    exclusive_filter.Init();
    assert(exclusive_filter.isBankAllowed(7, 77));
    assert(!exclusive_filter.isROCAllowed(2));
    JEventService_FilterDB disabled;
    configure(disabled, {{"FILTER:FILE", (root / "missing").string()}});
    disabled.Init();
    assert(disabled.isBankAllowed(99, 99));
    for (const auto& row : {"1 3 250\n", "1 3 250 250 trailing\n", "-1 3 250 250\n"}) {
        write(root / "bad.db", row);
        JEventService_FilterDB malformed;
        configure(malformed, {{"FILTER:ENABLE", "true"}, {"FILTER:FILE", (root / "bad.db").string()}});
        rejects([&] { malformed.Init(); });
    }
    JEventService_FilterDB missing;
    configure(missing, {{"FILTER:ENABLE", "true"}, {"FILTER:FILE", (root / "missing").string()}});
    rejects([&] { missing.Init(); });
    if (had_env) setenv("JCE_CONFIG_DIR", saved_env.c_str(), 1);
    else unsetenv("JCE_CONFIG_DIR");
    fs::remove_all(root);
}
