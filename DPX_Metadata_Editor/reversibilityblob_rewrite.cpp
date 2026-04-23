#include "reversibilityblob_rewrite.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <zlib.h>

#include "QtCore/qlogging.h"
#include "rawcookedreversibilityextractor.h"

namespace {

constexpr uint32_t kEbmlHeaderId      = 0x1A45DFA3;
constexpr uint32_t kRawcookedInfoId   = 0x207273;
constexpr uint32_t kRawcookedBaseId   = 0x207274;
constexpr uint32_t kRawcookedBlockId  = 0x207262;
constexpr uint32_t kDescriptorId      = 0x91; // filename-related compressed block
constexpr uint32_t kPayloadId         = 0x83; // before-data compressed block

} // namespace

ReversibilityBlob::ReversibilityBlob() = default;
ReversibilityBlob::~ReversibilityBlob() = default;

bool ReversibilityBlob::Load(const std::string& filename,std::string bpath) {
    ClearParsedState();
    blob_.clear();
    mkvfile=filename;
    RawCookedReversibilityExtractor extractor (filename,bpath);


    blob_.resize(165535);
    if (!extractor.extract(blob_))
        return false;

    return ParseDocument();
}
bool ReversibilityBlob::Inject(const std::string& filename,std::string bpath) {

    RawCookedReversibilityExtractor extractor (mkvfile,bpath);

    std::cout << "loading : " << mkvfile << std::endl;
    extractor.replaceFromFile(filename);
     std::cout << "injecting : " << filename << std::endl;
}
bool ReversibilityBlob::Load(const uint8_t* data, size_t size) {
    ClearParsedState();
    blob_.assign(data, data + size);
    return ParseDocument();
}

void ReversibilityBlob::ClearParsedState() {
    ebml_header_payload_.clear();
    info_payload_.clear();
    records_.clear();
    reconstructed_before_cache_.clear();
    modified_before_.clear();
    modified_filename_.clear();
    reconstructed_filename_cache_.clear();

    num_frames_ = 0;
}

bool ReversibilityBlob::ReadEbmlId(const uint8_t* data, size_t size, size_t& offset, uint32_t& id, size_t& id_len) {
    if (offset >= size)
        return false;

    const uint8_t first = data[offset];
    if (!first)
        return false;

    uint8_t mask = 0x80;
    id_len = 1;
    while (!(first & mask) && id_len < 4) {
        mask >>= 1;
        ++id_len;
    }

    if (!(first & mask) || offset + id_len > size)
        return false;

    id = 0;
    for (size_t i = 0; i < id_len; ++i)
        id = (id << 8) | data[offset + i];

    offset += id_len;
    return true;
}

bool ReversibilityBlob::ReadEbmlVint(const uint8_t* data, size_t size, size_t& offset, uint64_t& value, size_t& vint_len) {
    if (offset >= size)
        return false;

    const uint8_t first = data[offset];
    if (!first)
        return false;

    uint8_t mask = 0x80;
    vint_len = 1;
    while (!(first & mask) && vint_len < 8) {
        mask >>= 1;
        ++vint_len;
    }

    if (!(first & mask) || offset + vint_len > size)
        return false;

    value = first & (mask - 1);
    for (size_t i = 1; i < vint_len; ++i)
        value = (value << 8) | data[offset + i];

    offset += vint_len;
    return true;
}

bool ReversibilityBlob::ReadEbmlElement(const uint8_t* data, size_t size, size_t& offset, EbmlElement& out) {
    const size_t start = offset;

    uint32_t id = 0;
    size_t id_len = 0;
    if (!ReadEbmlId(data, size, offset, id, id_len))
        return false;

    uint64_t payload_size = 0;
    size_t size_len = 0;
    if (!ReadEbmlVint(data, size, offset, payload_size, size_len))
        return false;

    if (payload_size > size - offset)
        return false;

    out.id = id;
    out.size = payload_size;
    out.header_size = offset - start;
    out.data_offset = offset;
    return true;
}

void ReversibilityBlob::WriteEbmlId(std::vector<uint8_t>& out, uint32_t id) {
    if (id <= 0xFF) {
        out.push_back(static_cast<uint8_t>(id));
    } else if (id <= 0xFFFF) {
        out.push_back(static_cast<uint8_t>((id >> 8) & 0xFF));
        out.push_back(static_cast<uint8_t>(id & 0xFF));
    } else if (id <= 0xFFFFFF) {
        out.push_back(static_cast<uint8_t>((id >> 16) & 0xFF));
        out.push_back(static_cast<uint8_t>((id >> 8) & 0xFF));
        out.push_back(static_cast<uint8_t>(id & 0xFF));
    } else {
        out.push_back(static_cast<uint8_t>((id >> 24) & 0xFF));
        out.push_back(static_cast<uint8_t>((id >> 16) & 0xFF));
        out.push_back(static_cast<uint8_t>((id >> 8) & 0xFF));
        out.push_back(static_cast<uint8_t>(id & 0xFF));
    }
}

void ReversibilityBlob::WriteEbmlVint(std::vector<uint8_t>& out, uint64_t value) {
    for (size_t width = 1; width <= 8; ++width) {
        const uint64_t max_value =
            (width == 8) ? std::numeric_limits<uint64_t>::max()
                         : ((uint64_t{1} << (7 * width)) - 1);

        if (value <= max_value) {
            const uint64_t encoded = value | (uint64_t{1} << (7 * width));
            for (size_t i = 0; i < width; ++i) {
                const size_t shift = (width - 1 - i) * 8;
                out.push_back(static_cast<uint8_t>((encoded >> shift) & 0xFF));
            }
            return;
        }
    }

    throw std::runtime_error("EBML VINT too large");
}

void ReversibilityBlob::WriteEbmlElement(std::vector<uint8_t>& out, uint32_t id, const std::vector<uint8_t>& payload) {
    WriteEbmlId(out, id);
    WriteEbmlVint(out, payload.size());
    out.insert(out.end(), payload.begin(), payload.end());
}

bool ReversibilityBlob::DecodeRawcookedCompressedPayload(const buffer_view& in, buffer& out) {
    out.clear();
    if (!in.data || !in.size)
        return false;

    size_t offset = 0;
    uint64_t uncompressed_size = 0;
    size_t prefix_len = 0;
    if (!ReadEbmlVint(in.data, in.size, offset, uncompressed_size, prefix_len))
        return false;

    if (uncompressed_size == 0 || uncompressed_size > 512ull * 1024ull * 1024ull)
        return false;

    out.resize(static_cast<size_t>(uncompressed_size));
    uLongf dest_len = static_cast<uLongf>(uncompressed_size);

    const int res = uncompress(out.data(), &dest_len,
                               in.data + offset,
                               static_cast<uLong>(in.size - offset));

    if (res != Z_OK || dest_len != uncompressed_size) {
        out.clear();
        return false;
    }

    return true;
}

bool ReversibilityBlob::EncodeRawcookedCompressedPayload(const buffer& in, buffer& out) {
    out.clear();
    if (in.empty())
        return false;

    uLongf compressed_size = compressBound(static_cast<uLong>(in.Size()));
    std::vector<uint8_t> compressed(compressed_size);

    const int res = compress2(compressed.data(), &compressed_size,
                              in.Data(), static_cast<uLong>(in.Size()), 1);
    if (res != Z_OK)
        return false;

    compressed.resize(compressed_size);

    WriteEbmlVint(out, in.Size());
    out.insert(out.end(), compressed.begin(), compressed.end());
    return true;
}

buffer ReversibilityBlob::ComputeDiff(const buffer& base, const buffer& modified) {
    const size_t len = std::min(base.Size(), modified.Size());
    buffer diff;
    diff.resize(len, 0);

    for (size_t i = 0; i < len; ++i) {
        const int delta = static_cast<int>(modified[i]) - static_cast<int>(base[i]);
        diff[i] = static_cast<uint8_t>(delta & 0xFF);
    }

    return diff;
}

buffer ReversibilityBlob::ApplyDiff(const buffer& base, const buffer& diff) {
    buffer result = base;
    const size_t len = std::min(result.Size(), diff.Size());

    for (size_t i = 0; i < len; ++i) {
        if (diff[i])
            result[i] = static_cast<uint8_t>((static_cast<unsigned>(result[i]) + diff[i]) & 0xFFu);
    }

    return result;
}

bool ReversibilityBlob::ParseEbmlHeader(size_t& offset) {
    EbmlElement e;
    if (!ReadEbmlElement(blob_.data(), blob_.size(), offset, e))
        return false;
    if (e.id != kEbmlHeaderId)
        return false;

    ebml_header_payload_.assign(blob_.begin() + e.data_offset,
                                blob_.begin() + e.data_offset + static_cast<size_t>(e.size));
    offset = e.data_offset + static_cast<size_t>(e.size);
    return true;
}

bool ReversibilityBlob::ParseInfoElement(const EbmlElement& e) {
    if (e.id != kRawcookedInfoId)
        return false;

    info_payload_.assign(blob_.begin() + e.data_offset,
                         blob_.begin() + e.data_offset + static_cast<size_t>(e.size));
    return true;
}

bool ReversibilityBlob::ParseRecordElement(const EbmlElement& e) {
    if (e.id != kRawcookedBaseId && e.id != kRawcookedBlockId)
        return false;

    Record record;
    record.container_id = e.id;

    size_t inner_offset = e.data_offset;
    const size_t inner_end = e.data_offset + static_cast<size_t>(e.size);

    while (inner_offset < inner_end) {
        EbmlElement child;
        if (!ReadEbmlElement(blob_.data(), inner_end, inner_offset, child))
            return false;

        const uint8_t* child_data = blob_.data() + child.data_offset;
        const size_t child_size = static_cast<size_t>(child.size);

        if (child.id == kDescriptorId) {
            record.descriptor_encoded.assign(child_data, child_data + child_size);

            // Auxiliary chunk: decode if possible, but don't let it kill frame decoding.
            buffer decoded;
            if (DecodeRawcookedCompressedPayload(buffer_view(child_data, child_size), decoded))
                record.descriptor_decoded = std::move(decoded);
            else
                record.descriptor_decoded = record.descriptor_encoded;
        } else if (child.id == kPayloadId) {
            record.payload_encoded.assign(child_data, child_data + child_size);

            // Critical chunk: this is the actual base header (frame 0) or diff (later frames).
            if (!DecodeRawcookedCompressedPayload(buffer_view(child_data, child_size), record.payload_decoded))
                return false;
        }

        inner_offset = child.data_offset + child_size;
    }

    // Payload is mandatory. Descriptor is optional for your decoding path.
    if (record.payload_encoded.empty() || record.payload_decoded.empty())
        return false;

    records_.push_back(std::move(record));
    return true;
}

bool ReversibilityBlob::ParseDocument() {
    ClearParsedState();
    if (blob_.empty())
        return false;

    size_t offset = 0;
    if (!ParseEbmlHeader(offset))
        return false;

    while (offset < blob_.size()) {
        EbmlElement e;
        if (!ReadEbmlElement(blob_.data(), blob_.size(), offset, e))
            return false;

        if (e.id == kRawcookedInfoId) {
            if (!ParseInfoElement(e))
                return false;
        } else if (e.id == kRawcookedBaseId || e.id == kRawcookedBlockId) {
            if (!ParseRecordElement(e))
                return false;
        }

        offset = e.data_offset + static_cast<size_t>(e.size);
    }

    if (records_.empty())
        return false;

    // Must have a usable frame-0 base header immediately after load.
    if (records_.front().payload_decoded.empty())
        return false;

    num_frames_ = records_.size();
    return true;
}

buffer ReversibilityBlob::GetHeaderChunk() {
    if (records_.empty())
        return {};

    auto mod_it = modified_before_.find(0);
    if (mod_it != modified_before_.end())
        return mod_it->second;

    return records_.front().payload_decoded;
}
buffer ReversibilityBlob::GetFileNameChunk(size_t frame) const {
    if (frame >= records_.size())
        return {};

    auto mod_it = modified_filename_.find(frame);
    if (mod_it != modified_filename_.end())
        return mod_it->second;

    auto strip_filename_prefix = [](const buffer& in) -> buffer {
        if (in.empty())
            return {};

        // RAWcooked filename base chunk appears to have a 1-byte prefix.
        if (in[0] == 0x80) {
            buffer out;
            out.insert(out.end(), in.begin() + 1, in.end());
            return out;
        }

        return in;
    };

    if (frame == 0)
        return strip_filename_prefix(records_.front().descriptor_decoded);

    auto cached = reconstructed_filename_cache_.find(frame);
    if (cached != reconstructed_filename_cache_.end())
        return cached->second;

    buffer base = strip_filename_prefix(records_.front().descriptor_decoded);
    if (base.empty())
        return {};

    buffer diff = records_[frame].descriptor_decoded;
    buffer result = ApplyDiff(base, diff);
    reconstructed_filename_cache_[frame] = result;
    return result;
}

buffer ReversibilityBlob::GetBeforeData(size_t frame) {
    if (frame >= records_.size())
        return {};

    auto mod_it = modified_before_.find(frame);
    if (mod_it != modified_before_.end())
        return mod_it->second;

    if (frame == 0)
        return GetHeaderChunk();

    auto cached = reconstructed_before_cache_.find(frame);
    if (cached != reconstructed_before_cache_.end())
        return cached->second;

    const buffer base = GetHeaderChunk();
    if (base.empty())
        return {};

    const buffer result = ApplyDiff(base, records_[frame].payload_decoded);
    reconstructed_before_cache_[frame] = result;
    return result;
}

bool ReversibilityBlob::SetBeforeData(size_t frame, const buffer& newBeforeData) {
    if (frame >= records_.size())
        return false;

    modified_before_[frame] = newBeforeData;
    reconstructed_before_cache_.erase(frame);
    return true;
}

bool ReversibilityBlob::SetFileNameChunk(size_t frame, const buffer& newFileName) {
    if (frame >= records_.size())
        return false;

    modified_filename_[frame] = newFileName;
    reconstructed_filename_cache_.erase(frame);
    return true;
}
buffer ReversibilityBlob::BuildBlob() const {
    std::vector<uint8_t> out;

    WriteEbmlElement(out, kEbmlHeaderId, ebml_header_payload_);

    if (!info_payload_.empty())
        WriteEbmlElement(out, kRawcookedInfoId, info_payload_);

    const buffer base_header = [&]() -> buffer {
        auto it = modified_before_.find(0);
        if (it != modified_before_.end())
            return it->second;
        return records_.front().payload_decoded;
    }();

    if (base_header.empty())
        return {};

    for (size_t frame = 0; frame < records_.size(); ++frame) {
        const auto& original = records_[frame];

        buffer descriptor_encoded;

        auto fn_it = modified_filename_.find(frame);
        if (fn_it == modified_filename_.end()) {
            // Preserve original descriptor bytes exactly if not modified.
            descriptor_encoded = original.descriptor_encoded;
        } else {
            if (frame == 0) {
                // Frame 0 filename block is RAW, not compressed.
                buffer descriptor_raw;
                descriptor_raw.push_back(0x80);
                descriptor_raw.insert(descriptor_raw.end(),
                                      fn_it->second.begin(),
                                      fn_it->second.end());
                descriptor_encoded = descriptor_raw;
            } else {
                // Later filename blocks: keep diff/compressed behavior.
                buffer base_filename = GetFileNameChunk(0);
                buffer descriptor_decoded = ComputeDiff(base_filename, fn_it->second);

                if (!EncodeRawcookedCompressedPayload(descriptor_decoded, descriptor_encoded))
                    return {};
            }
        }

        buffer payload_decoded;
        if (frame == 0) {
            payload_decoded = base_header;
        } else if (auto it = modified_before_.find(frame); it != modified_before_.end()) {
            payload_decoded = ComputeDiff(base_header, it->second);
        } else {
            payload_decoded = original.payload_decoded;
        }

        buffer payload_encoded;
        if (!EncodeRawcookedCompressedPayload(payload_decoded, payload_encoded))
            return {};

        std::vector<uint8_t> record_payload;

        if (!descriptor_encoded.empty())
            WriteEbmlElement(record_payload, kDescriptorId, descriptor_encoded);

        WriteEbmlElement(record_payload, kPayloadId, payload_encoded);
        WriteEbmlElement(out, original.container_id, record_payload);
    }

    buffer result;
    result.insert(result.end(), out.begin(), out.end());
    return result;
}
bool ReversibilityBlob::SaveModified(const std::string& output_filename, std::string binpath) {
    if (records_.empty()) {
        std::cerr << "Error: No reversibility blob loaded.\n";
        return false;
    }

    buffer rebuilt = BuildBlob();
    if (rebuilt.empty()) {
        std::cerr << "Error: Failed to rebuild reversibility blob.\n";
        return false;
    }

    std::ofstream out(output_filename, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Cannot open " << output_filename << " for writing.\n";
        return false;
    }

    out.write(reinterpret_cast<const char*>(rebuilt.data()),
              static_cast<std::streamsize>(rebuilt.size()));
    return static_cast<bool>(out);
}
