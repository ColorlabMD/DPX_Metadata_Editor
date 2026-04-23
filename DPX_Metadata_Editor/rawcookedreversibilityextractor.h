#pragma once

#include <string>
#include <vector>
#include <stdexcept>

class RawCookedReversibilityExtractor {
public:
    explicit RawCookedReversibilityExtractor(const std::string& mkvPath,const std::string& mkvBinPath)
        : m_filePath(mkvPath),binpath(mkvBinPath)
    {
    }
    ~RawCookedReversibilityExtractor();

    // Primary method - tries libav first, falls back to mkvextract
    bool extract(std::vector<uint8_t>& outBlob);

    // Direct MKVToolNix method (most reliable for RAWcooked)
    bool extractWithMkvExtract(std::vector<uint8_t>& outBlob);

    bool saveToFile(const std::string& outputPath = "reversibility.rawcooked");

    // New: replace existing RAWcooked reversibility attachment from memory
    bool replace(const std::vector<uint8_t>& newBlob);

    // New: replace existing RAWcooked reversibility attachment from a file
    bool replaceFromFile(const std::string& inputPath);

private:
    std::string m_filePath;
std::string binpath;
    // Helper to find attachment ID using mkvmerge
    int findAttachmentId() const;

    static std::string shellQuote(const std::string& s);
};
