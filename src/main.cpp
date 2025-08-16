//
// Created by kapil on 15.08.2025.
//

#include <cstdint>
#include <iostream>

#include <nucc/xfbin.hpp>

constexpr uint8_t ASBR_KEY[8] = {0x00, 0x2A, 0x02, 0x5B, 0xEF, 0x73, 0x44, 0x1B};
constexpr uint8_t EOH_DEMO_KEY[8] = {0x02, 0x03, 0xBD, 0x8B, 0x01, 0x54, 0x8F, 0xB5};

std::array<uint8_t, 8> parse_key_from_str(const std::string& str) {
    // Convert string into a byte array
    std::array<uint8_t, 8> key = {0};
    if (str.size() != 16) {
        throw std::runtime_error("Key string should be exactly 16 characters long.");
    }

    for (size_t i = 0; i < 8; ++i) {
        std::string byte_str = str.substr(i * 2, 2);
        key[i] = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
    }

    return key;
}

void atexit_handler() {
    std::cout << "\nPress Enter to exit..." << std::endl;
    std::cin.get();
}

// argc argv
int main(int argc, char* argv[]) {
    atexit(atexit_handler);

    kojo::logger log{"XFBIN Decryptor", true, true};

    if (argc < 2) {
        log.error(
            kojo::logger::status::bad_value,
            "No XFBIN file path provided.",
            "Usage: <program_name> <xfbin_file_path>"
        );

        return 1;
    }

    // Try to open the key.txt file
    // std::cout << "Looking for key.txt..." << std::endl;
    log.info("Looking for key.txt...");
    std::ifstream key_file("key.txt");
    if (!key_file.is_open()) {
        log.error(
            kojo::logger::status::null_file,
            "Could not open key.txt.",
            "Ensure key.txt exists in the current directory or provide a valid key."
        );

        return 1;
    }

    // Parse key.txt
    std::string key_line;
    if (!std::getline(key_file, key_line)) {
        log.error(
            kojo::logger::status::bad_value,
            "Could not read key from key.txt.",
            "Ensure key.txt contains a valid 16-character key string."
        );

        return 1;
    }

    const auto parsed_key = parse_key_from_str(key_line);

    // Check if a provided path proper
    std::filesystem::path input_path(argv[1]);
    if (!std::filesystem::exists(input_path)) {
        log.error(
            kojo::logger::status::null_file,
            std::format("The provided path \"{}\" does not exist.", input_path.string()),
            "Ensure the path points to a valid XFBIN file/directory."
        );
        return 1;
    }

    std::vector<std::string> files_to_decrypt;
    if (std::filesystem::is_directory(input_path)) {
        log.info(std::format("Scanning directory \"{}\" for XFBIN files...", input_path.string()));
        for (const auto& entry : std::filesystem::directory_iterator(input_path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".xfbin") {
                files_to_decrypt.push_back(entry.path().string());
            }
        }
    } else if (input_path.extension() == ".xfbin") {
        files_to_decrypt.push_back(input_path.string());
    } else {
        log.error(
            kojo::logger::status::bad_value,
            std::format("The provided path \"{}\" is not a valid XFBIN file or directory.", input_path.string()),
            "Ensure the path points to a valid XFBIN file or directory containing XFBIN files."
        );
        return 1;
    }

    for (const auto& file : files_to_decrypt) {
        log.info(std::format("Decrypting XFBIN file: {}", file));

        try {
            const nucc::xfbin xfbin(file, parsed_key.data());
        } catch (const std::exception& e) {
            log.error(
                kojo::logger::status::bad_value,
                std::format("An error occurred: {}", e.what()),
                "Ensure the XFBIN file is valid and the key is correct."
            );
            return 1;
        }
    }

    return 0;
}