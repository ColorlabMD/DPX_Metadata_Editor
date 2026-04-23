#include "RawCookedReversibilityExtractor.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QByteArray>
#include <fstream>
#include <iostream>
#include <cstdio>      // for popen / pclose
#include <cstdlib>     // for system
#include <sstream>
#include <array>
#include <ctime>
#include <filesystem>


RawCookedReversibilityExtractor::~RawCookedReversibilityExtractor() = default;

int RawCookedReversibilityExtractor::findAttachmentId() const
{
        std::string cmd = binpath+ "mkvmerge -J \"" + m_filePath + "\" 2>&1";

        std::array<char, 512> buffer{};
        std::string output;

        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe)
            return -1;

        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
            output += buffer.data();

        int rc = pclose(pipe);
        if (rc != 0)
        {
            std::cerr << "mkvmerge failed:\n" << output << std::endl;
            return -1;
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(
            QByteArray::fromStdString(output), &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject())
        {
            std::cerr << "Failed to parse mkvmerge JSON: "
                      << parseError.errorString().toStdString() << std::endl;
            return -1;
        }

        QJsonObject root = doc.object();

        bool hasFFV1VideoTrack = false;

        QJsonArray tracks = root.value("tracks").toArray();
        for (const QJsonValue& v : tracks)
        {
            if (!v.isObject())
                continue;

            QJsonObject track = v.toObject();
            QString type = track.value("type").toString();
            QString codec = track.value("codec").toString();

            if (type == "video" && codec == "V_FFV1")
            {
                hasFFV1VideoTrack = true;
                break;
            }
        }

        if (!hasFFV1VideoTrack)
        {
            std::cerr << "No FFV1 video track found." << std::endl;
            return -1;
        }

        QJsonArray attachments = root.value("attachments").toArray();
        for (const QJsonValue& v : attachments)
        {
            if (!v.isObject())
                continue;

            QJsonObject att = v.toObject();
            QString fileName = att.value("file_name").toString();

            if (fileName == "RAWcooked reversibility data")
            {
                int id = att.value("id").toInt(-1);
                if (id != -1)
                {
                    std::cout << "Detected RAWcooked reversibility attachment ID: "
                              << id << std::endl;
                    return id;
                }
            }
        }

        std::cerr << "RAWcooked reversibility attachment not found." << std::endl;
        return -1;
    }
bool RawCookedReversibilityExtractor::extractWithMkvExtract(std::vector<uint8_t>& outBlob)
{
    outBlob.clear();

    int attachId = findAttachmentId();   // This function needs updating too (see below)

    if (attachId < 0) {
        std::cerr << "Could not determine attachment ID. Trying common values..." << std::endl;
        attachId = 1;   // Most common for your current file
    }

    std::string tempFile = "/tmp/rawcooked_reversibility_" + std::to_string(std::time(nullptr)) + ".bin";

    std::string cmd = binpath+"mkvextract attachments \"" + m_filePath + "\" "
                      + std::to_string(attachId) + ":\"" + tempFile + "\"";

    std::cout << "Running: " << cmd << std::endl;

    if (std::system(cmd.c_str()) != 0) {
        std::cerr << "mkvextract failed." << std::endl;
        std::remove(tempFile.c_str());
        return false;
    }

    // Read into memory
    std::ifstream ifs(tempFile, std::ios::binary | std::ios::ate);
    if (!ifs) {
        std::cerr << "Failed to read temporary file." << std::endl;
        std::remove(tempFile.c_str());
        return false;
    }

    auto size = ifs.tellg();
    if (size <= 0) {
        std::cerr << "Extracted file is empty." << std::endl;
        ifs.close();
        std::remove(tempFile.c_str());
        return false;
    }

    outBlob.resize(static_cast<size_t>(size));
    ifs.seekg(0);
    ifs.read(reinterpret_cast<char*>(outBlob.data()), size);
    ifs.close();
    std::remove(tempFile.c_str());

    std::cout << "✅ Successfully extracted " << outBlob.size() << " bytes of RAWcooked reversibility data." << std::endl;
    return true;
}
bool RawCookedReversibilityExtractor::extract(std::vector<uint8_t>& outBlob)
{
    // You can keep a simple libav attempt here if you want, but for now we go straight to reliable method
    return extractWithMkvExtract(outBlob);
}

bool RawCookedReversibilityExtractor::saveToFile(const std::string& outputPath)
{
    std::vector<uint8_t> blob;
    if (!extract(blob) || blob.empty()) {
        return false;
    }

    std::ofstream ofs(outputPath, std::ios::binary | std::ios::trunc);
    if (!ofs) {
        std::cerr << "Failed to open output file: " << outputPath << std::endl;
        return false;
    }

    ofs.write(reinterpret_cast<const char*>(blob.data()), blob.size());
    bool success = ofs.good();
    ofs.close();

    if (success) {
        std::cout << "Saved " << blob.size() << " bytes to: " << outputPath << std::endl;
    }
    return success;
}
std::string RawCookedReversibilityExtractor::shellQuote(const std::string& s)
{
    std::string out = "'";
    for (char c : s)
    {
        if (c == '\'')
            out += "'\\''";
        else
            out += c;
    }
    out += "'";
    return out;
}

bool RawCookedReversibilityExtractor::replaceFromFile(const std::string& inputPath)
{
    int attachId = findAttachmentId();

    if (attachId < 0)
    {
        std::cerr << "Could not determine RAWcooked reversibility attachment ID." << std::endl;
        return false;
    }

    if (!std::filesystem::exists(inputPath))
    {
        std::cerr << "Replacement file does not exist: " << inputPath << std::endl;
        return false;
    }

    std::string cmd =
     binpath  + "mkvpropedit " + shellQuote(m_filePath) +
        " --replace-attachment " + std::to_string(attachId) + ":" + shellQuote(inputPath);

    std::cout << "Running: " << cmd << std::endl;

    int rc = std::system(cmd.c_str());
    if (rc != 0)
    {
        std::cerr << "mkvpropedit failed." << std::endl;
        return false;
    }

    std::cout << "✅ Successfully replaced RAWcooked reversibility attachment ID "
              << attachId << std::endl;
    return true;
}

bool RawCookedReversibilityExtractor::replace(const std::vector<uint8_t>& newBlob)
{
    if (newBlob.empty())
    {
        std::cerr << "Replacement blob is empty." << std::endl;
        return false;
    }

    std::string tempFile =
        "/tmp/rawcooked_replacement_" + std::to_string(std::time(nullptr)) + ".bin";

    {
        std::ofstream ofs(tempFile, std::ios::binary | std::ios::trunc);
        if (!ofs)
        {
            std::cerr << "Failed to create temporary replacement file." << std::endl;
            return false;
        }

        ofs.write(reinterpret_cast<const char*>(newBlob.data()), newBlob.size());
        if (!ofs.good())
        {
            std::cerr << "Failed writing replacement blob to temporary file." << std::endl;
            ofs.close();
            std::remove(tempFile.c_str());
            return false;
        }
    }

    bool ok = replaceFromFile(tempFile);
    std::remove(tempFile.c_str());
    return ok;
}
