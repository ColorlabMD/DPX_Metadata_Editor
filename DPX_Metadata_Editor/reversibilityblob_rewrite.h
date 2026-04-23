
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

struct buffer : public std::vector<uint8_t> {
    using std::vector<uint8_t>::vector;

    const uint8_t* Data() const { return empty() ? nullptr : data(); }
    uint8_t* Data() { return empty() ? nullptr : data(); }
    size_t Size() const { return size(); }
};
class ReversibilityBlob {
public:
    ReversibilityBlob();
    ~ReversibilityBlob();

    bool Load(const std::string& filename, std::string bpath);
    bool Load(const uint8_t* data, size_t size);

    buffer GetHeaderChunk();
    buffer GetFileNameChunk(size_t frame) const;   // Kept for compatibility; returns decoded element 0x91 payload
    buffer GetBeforeData(size_t frame);

    bool SetBeforeData(size_t frame, const buffer& newBeforeData);
    bool SetFileNameChunk(size_t frame, const buffer& newPosData);

    bool SaveModified(const std::string& output_filename, std::string binpath);
    bool SaveModifiedG(const std::string& output_filename, std::string binpath) { return SaveModified(output_filename,binpath); }
    bool Inject(const std::string &filename, std::string bpath);
    size_t GetFrameCount() const { return num_frames_; }
    bool IsLoaded() const { return !records_.empty(); }

private:
     std::string mkvfile;
    struct buffer_view {
        const uint8_t* data = nullptr;

        size_t size = 0;
        buffer_view() = default;
        buffer_view(const uint8_t* d, size_t s) : data(d), size(s) {}
    };

    struct EbmlElement {
        uint32_t id = 0;
        uint64_t size = 0;
        size_t header_size = 0;
        size_t data_offset = 0;
    };

    struct Record {
        uint32_t container_id = 0;      // 0x207274 or 0x207262
        buffer descriptor_encoded;      // raw 0x91 payload as stored in file
        buffer payload_encoded;         // raw 0x83 payload as stored in file

        buffer descriptor_decoded;      // decoded 0x91 payload
        buffer payload_decoded;         // decoded 0x83 payload
    };

    bool ParseDocument();
    bool ParseEbmlHeader(size_t& offset);
    bool ParseInfoElement(const EbmlElement& e);
    bool ParseRecordElement(const EbmlElement& e);

    static bool ReadEbmlId(const uint8_t* data, size_t size, size_t& offset, uint32_t& id, size_t& id_len);
    static bool ReadEbmlVint(const uint8_t* data, size_t size, size_t& offset, uint64_t& value, size_t& vint_len);
    static bool ReadEbmlElement(const uint8_t* data, size_t size, size_t& offset, EbmlElement& out);

    static void WriteEbmlId(std::vector<uint8_t>& out, uint32_t id);
    static void WriteEbmlVint(std::vector<uint8_t>& out, uint64_t value);
    static void WriteEbmlElement(std::vector<uint8_t>& out, uint32_t id, const std::vector<uint8_t>& payload);

    static bool DecodeRawcookedCompressedPayload(const buffer_view& in, buffer& out);
    static bool EncodeRawcookedCompressedPayload(const buffer& in, buffer& out);

    static buffer ComputeDiff(const buffer& base, const buffer& modified);
    static buffer ApplyDiff(const buffer& base, const buffer& diff);

    buffer BuildBlob() const;
    void ClearParsedState();

    std::vector<uint8_t> blob_;

    std::vector<uint8_t> ebml_header_payload_; // payload of 0x1A45DFA3
    std::vector<uint8_t> info_payload_;        // payload of 0x207273

    std::vector<Record> records_;
    mutable std::unordered_map<size_t, buffer> reconstructed_before_cache_;

    std::unordered_map<size_t, buffer> modified_before_;
    std::unordered_map<size_t, buffer> modified_filename_;
    mutable std::unordered_map<size_t, buffer> reconstructed_filename_cache_;


    size_t num_frames_ = 0;
};
