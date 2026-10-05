#pragma once

#include <JANA/JEvent.h>
#include <JANA/JException.h>

#include <cstdint>
#include <cstdlib>
#include <cxxabi.h>
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <typeinfo>
#include <unordered_set>
#include <utility>
#include <vector>

// Event-owned, non-owning references distinguish outputs of routes which
// publish the same DigiHit type. The normal DigiHit collection owns each hit.
template <typename RawHitT, typename DigiHitT>
struct DetectorTranslationOutput {
    std::vector<const DigiHitT*> hits;
};

// Dump descriptions only. The translator service owns registration/lifecycle;
// the diagnostic processor owns streams. No file I/O happens here.
class DetectorTranslationDump {
public:
    struct Output {
        std::string detector;
        std::string filename;
        std::string header;
        std::function<void(std::ostream&, const JEvent&)> writeEvent;
    };

    template <typename DigiHitT>
    using RowWriter = std::function<void(
        std::ostream&, std::uint64_t, const DigiHitT&)>;

    // Reversible encoding keeps distinct detector/type names distinct, and
    // prevents names from introducing directories into generated paths.
    static std::string pathComponent(const std::string& name) {
        const char* hex = "0123456789ABCDEF";
        std::string result;
        for (unsigned char ch : name) {
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                (ch >= '0' && ch <= '9') || ch == '_' || ch == '-') {
                result += ch;
            } else {
                result += '%';
                result += hex[ch >> 4];
                result += hex[ch & 15];
            }
        }
        return result;
    }

    template <typename RawHitT>
    static std::string rawHitFilename() {
        int status = 0;
        std::unique_ptr<char, decltype(&std::free)> name(
            abi::__cxa_demangle(typeid(RawHitT).name(), nullptr, nullptr, &status),
            &std::free);
        if (status != 0 || !name) {
            throw JException("Cannot determine detector dump raw-hit type name");
        }
        return pathComponent(name.get()) + ".csv";
    }

    template <typename RawHitT, typename DigiHitT>
    void addWriter(const std::string& detector, std::string header,
        RowWriter<DigiHitT> writeRow) {
        if (header.empty() || !writeRow) {
            throw JException("A detector dump requires a CSV header and row writer");
        }
        const auto directory = pathComponent(detector);
        const auto filename = rawHitFilename<RawHitT>();
        const auto path = directory + "/" + filename;
        // Distinct C++ identities can have the same displayed type name.
        if (m_paths.contains(path)) {
            throw JException("Duplicate generated detector dump path '%s'", path.c_str());
        }
        Output output {directory, filename, std::move(header),
            [writeRow = std::move(writeRow), detector](
                std::ostream& stream, const JEvent& event) {
                for (const auto* collection :
                        event.Get<DetectorTranslationOutput<RawHitT, DigiHitT>>(detector, false)) {
                    for (const auto* hit : collection->hits) {
                        writeRow(stream, event.GetEventNumber(), *hit);
                    }
                }
            }};
        m_paths.insert(path);
        m_outputs.push_back(std::move(output));
    }

    const std::vector<Output>& outputs() const { return m_outputs; }

private:
    std::unordered_set<std::string> m_paths;
    std::vector<Output> m_outputs;
};
