# CLAP Saw Demo with madronalib

A demo CLAP audio plugin showcasing plugin development using madronalib's AudioContext and EventsToSignals architecture.

- AudioContext handles all voice management, event processing, and timing
- Complete MIDI/MPE support via EventsToSignals
- SIMD-optimized DSP: DSPVector processing throughout for maximum performance
- Essential CLPA extensions (audio-ports, note-ports) fully implemented
- Cross-platform-ish: macOS, Linux, and Windows support via madronalib, but I've only tested on my M2 mac.

## Architecture Overview

```
CLAP Host → CLAPWrapper → AudioContext (EventsToSignals) → ClapSawDemo → Audio Output
```

The entire plugin implementation is reduced to a single `processAudioContext()` method, with madronalib handling all complexity.

## Building the Plugin

### Prerequisites

- CMake 3.15+
- C++17 compatible compiler
- Git with submodule support

### Quick Build

```bash
git clone https://github.com/your-repo/clap-saw-demo
cd clap-saw-demo
git submodule update --init --recursive
mkdir -p ignore/build
cmake -Bignore/build -DCMAKE_BUILD_TYPE=Release -DCOPY_AFTER_BUILD=ON -DCSD_INCLUDE_GUI=OFF
cmake --build ignore/build --target clap-saw-demo -j
```

The plugin automatically installs to:
- **macOS**: `~/Library/Audio/Plug-Ins/CLAP/clap-saw-demo.clap`
- **Linux**: `~/.clap/clap-saw-demo.clap`

**Note**: For the moment, the GUI is disabled (`-DCSD_INCLUDE_GUI=OFF`) to focus on core CLAP + madronalib functionality.

## Development Scripts

### Testing and Validation

**Quick Plugin Testing**:
```bash
scripts/test-plugin.sh
```
Runs CLAP specification validation using clap-validator.

**Comprehensive Debugging**:
```bash
scripts/debug-plugin.sh
```
Full debugging suite including:
- Dependency verification
- Tool building (clap-info, clap-host with automatic dependency installation)
- Plugin inspection
- CLAP validator testing

**Test Plugin with CLAP Host**:
```bash
tools/clap-host/build/host/clap-host ~/Library/Audio/Plug-Ins/CLAP/clap-saw-demo.clap
```
Test plugin functionality in a reference CLAP host environment.

### Platform-Specific Testing

**Windows Testing**:
```bash
scripts/test-plugin.bat
```

## Plugin Validation

The plugin passes CLAP specification testing:

```bash
# Run validator directly
cd libs/clap-validator
cargo run --release -- validate ~/Library/Audio/Plug-Ins/CLAP/clap-saw-demo.clap
```

**Current Status**: 10/21 tests pass, 0 failures, 11 skipped (params/state extensions not yet implemented)

## Project Structure

```
clap-saw-demo/
├── src/                          # Plugin implementation (~70 lines total!)
│   ├── clap-saw-demo.cpp         # Main DSP processing
│   ├── clap-saw-demo.h           # Plugin interface
│   └── clap-saw-demo-entry.cpp   # One-line CLAP export
├── libs/
│   ├── madronalib/               # Audio DSP framework with EventsToSignals
│   ├── clap/                     # CLAP specification
│   ├── clap-helpers/             # CLAP utility library
│   └── clap-validator/           # CLAP testing tool
├── scripts/
│   ├── debug-plugin.sh           # Debugging suite
│   ├── test-plugin.sh            # Quick CLAP validation
│   └── test-plugin.bat           # Windows testing
└── tools/
    ├── clap-info/                # Plugin inspection tool
    └── clap-host/                # Reference CLAP host
```

## Understanding the Code

**Key files**:
- `src/clap-saw-demo.cpp` - The `processAudioContext()` method has all the interesting DSP stuff
- `libs/madronalib/include/CLAPExport.h` - Fairly general CLAP wrapper for a madronalib [`ml::SignalProcessor`](https://github.com/madronalabs/madronalib/blob/master/source/app/MLSignalProcessor.h)
- `src/clap-saw-demo-entry.cpp` - One-liner plugin export macro

The `processAudioContext()` method receives pre-processed voice signals (pitch, gate, mod) from EventsToSignals and only needs to implement DSP.

## CLAP Extensions Status

- ✅ **audio-ports**: Stereo I/O with in-place processing
- ✅ **note-ports**: MIDI input with CLAP and MIDI dialect support
- ✅ **Factory interface**: Plugin enumeration and creation
- ✅ **Core lifecycle**: Complete plugin lifecycle management
- 🚧 **params**: Parameter automation (in progress)
- 📋 **state**: Preset management (future enhancement)
- 📋 **gui**: Plugin interface (future enhancement)

## Development Workflow

1. **Make changes** to plugin code
2. **Build and auto-install**: `cmake --build ignore/build --target clap-saw-demo`
3. **Validate**: `scripts/test-plugin.sh`
4. **Test in DAW**: Load in Bitwig/Reaper/etc.
5. **Debug if needed**: `scripts/debug-plugin.sh`

## madronalib Integration Benefits

- **Voice management**: Complete polyphonic note handling via EventsToSignals
- **Event processing**: MIDI/MPE support with sample-accurate timing
- **Parameter automation**: Smoothing and modulation
- **Cross-platform**: Identical DSP core for CLAP, AU, AAX, standalones, whatever your heart desires
- **Performance**: SIMD-optimized DSPVector processing throughout

## Contributing

**Future enhancements welcome**:
- Parameter extension implementation
- Enhanced DSP features (unison, filtering)
- Performance optimization and profiling
- Cross-platform validation

## License

MIT License - See LICENSE file for details.

## Refs

- [CLAP Specification](https://github.com/free-audio/clap)
- [madronalib Documentation](https://github.com/madronalabs/madronalib)
