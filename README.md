# 89 Theme Engine – Native ELF Theme Engine for Siemens Mobile Phones

89 Theme Engine is a native ELF daemon that replaces the standard UI theme system. It builds the entire interface from a single wallpaper image plus a skin file (.89), allowing full customisation of colours, borders, overlays, and more.

## Building
The SDK must be located at `../sdk` relative to the project root.
```bash
mkdir build_elf && cd build_elf
cmake .. -DBUILD_TARGET=elf
make 
```

## Usage

1. Copy `89ThemeEngine.elf` to `X:\ZBin\daemons\`. The ELF runs as a daemon.
2. **Associate the `.89` extension** with `89ThemeEngine.elf`:
   - **RUN** → `89ThemeEngine.elf` – applies the theme.
   - **ALTRUN** → `CfgEdit.elf` – allows editing the skin file.
3. Run the ELF once manually. It will create a default skin file named `Default.89` in the same folder as the ELF.
4. Click on a `.89` file (e.g., `Default.89`).  
   The ELF will parse the skin, combine it with the current wallpaper, and apply the theme immediately.
