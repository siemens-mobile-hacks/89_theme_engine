# 89 Theme Engine – Native ELF Theme Engine for Siemens Mobile Phones

89 Theme Engine is a native ELF daemon that replaces the standard UI theme system. It builds the entire interface from a single wallpaper image plus a skin file (.89), allowing full customisation of colours, borders, overlays, and more.

## Building
The SDK must be located at `../sdk` relative to the project root.

Building elf:
```bash
mkdir build_elf && cd build_elf
cmake .. -DBUILD_TARGET=elf
make
```

Building patch:
```bash
mkdir build_patch && cd build_patch
cmake .. -DBUILD_TARGET=patch
make
```

## Usage

1. **Apply the patch** (e.g., `E71_45.vkp`). The patch is required for full functionality.  
   In the patch database it is listed as [89ThemeEngine addon](https://patches.kibab.com/patches/search.php5?action=search&kw=89ThemeEngine+addon). 
2. Copy `89ThemeEngine.elf` to `X:\ZBin\daemons\`. The ELF runs as a daemon.
3. **Associate the `.89` extension** with `89ThemeEngine.elf`:
   - **RUN** → `89ThemeEngine.elf` – applies the theme.
   - **ALTRUN** → `CfgEdit.elf` – allows editing the skin file. 
4. Run the ELF once manually. It will create a default skin file named `Viktor.89` in the same folder as the ELF. 
5. Click on a `.89` file (e.g., `Viktor.89`).  
   The ELF will parse the skin, combine it with the current wallpaper, and apply the theme immediately.
