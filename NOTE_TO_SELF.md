# wow

For whatever reason, GitHub won't let me push direct changes to the nucc++ submodule, and i cannot be bothered to
figure out why. So, here's a note to myself to remember what I changed in the submodule,
in case I need to update it later lmao.

## Changes:

1. Disable debug/verbose logging in `xfbin.hpp` and `chunk.hpp`
2. Change the `read` function in `xfbin.hpp` so i can dump data easily
    ```cpp
   void xfbin::read(kojo::binary_view data) {
   log.verbose("Reading header...");
    read_header(data);

    log.verbose("Reading index...");
    read_index(data);

    log.verbose("Reading chunks...");
    read_chunks(data);

    log.verbose("Reading complete!");

    // Dump the loaded data into a file
    std::ofstream output_file(filename + "-decrypted.xfbin", std::ios::binary);
    if (output_file.is_open()) {
        output_file.write(reinterpret_cast<const char *>(data.data()), size);

        output_file.close();
        log.info(std::format("Decrypted XFBIN written to \"{}-decrypted.xfbin\".", filename));
    } else {
        log.error(
            kojo::logger::status::null_file,
            std::format("Failed to write decrypted XFBIN to file \"{}-decrypted.xfbin\".", filename),
            "Ensure you have write permissions in the current directory."
        );
        }
   }
    ```