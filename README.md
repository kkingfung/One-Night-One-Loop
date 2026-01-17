# One-Night-One-Loop

## Dawnlight - Unreal Engine 5 Game Project

A game project built with Unreal Engine 5.7, following AAA development standards.

## Requirements

- **Unreal Engine**: 5.7
- **Visual Studio**: 2022 (with C++ game development workload)
- **Platform**: Windows

## Getting Started

### 1. Clone the Repository

```bash
git clone <repository-url>
cd One-Night-One-Loop
```

### 2. Generate Project Files

Right-click on `Dawnlight/Dawnlight.uproject` and select:
- **Generate Visual Studio project files**

Or use the command line:
```bash
"C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -projectfiles -project="Dawnlight/Dawnlight.uproject" -game -rocket -progress
```

### 3. Open in Editor

Double-click `Dawnlight.uproject` or open from Epic Games Launcher.

## Project Structure

```
One-Night-One-Loop/
├── .gitignore              # Git ignore rules
├── CLAUDE.md               # AI assistant guidelines and coding standards
├── README.md               # This file
└── Dawnlight/              # UE5 Project
    ├── Config/             # Engine and project configuration
    ├── Content/            # Game assets (Blueprints, Maps, etc.)
    ├── Source/             # C++ source code (when created)
    └── Dawnlight.uproject  # Project file
```

## Documentation

- **[CLAUDE.md](CLAUDE.md)** - Project overview, architecture, and coding standards for AI assistance
- **[Dawnlight/CODING_STANDARDS.md](Dawnlight/CODING_STANDARDS.md)** - Detailed C++ coding conventions

## Coding Standards

- All comments must be written in **Japanese**
- Follow Epic's Unreal Engine coding standards
- Use UPROPERTY/UFUNCTION macros appropriately
- See [CODING_STANDARDS.md](Dawnlight/CODING_STANDARDS.md) for detailed guidelines

## Version Control

### Branching Strategy

- `main` - Stable, release-ready code
- `develop` - Integration branch
- `feature/*` - Feature development
- `bugfix/*` - Bug fixes
- `hotfix/*` - Emergency fixes

### Commit Message Format (Japanese)

```
[機能] 新機能の説明
[修正] バグ修正の説明
[改善] 改善内容の説明
[リファクタ] リファクタリングの説明
```

## License

All rights reserved.

---

**Engine Version**: Unreal Engine 5.7
**Last Updated**: 2025-01-15
