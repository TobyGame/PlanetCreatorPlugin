# 🌌 Unreal Planet Creator Plugin

A modular plugin suite for Unreal Engine 5.5+ designed to enable the creation of full-scale procedural planets, moons,
ecosystems, weather systems, and planetary terrains for space exploration and world simulation games.

## 🔧 Plugin Structure

This repository contains multiple specialized tools, each implemented as an Unreal Engine plugin module:

### 🔹 1. Planet Creator Plugin

The central framework managing planetary systems and their components. It provides integration and orchestration for the
following tools:

- **Celestial Body Tool**: Procedural planets and moons using Nanite and PCG.
- **Atmosphere & Weather Tool**: Realistic atmospheric scattering and dynamic weather.
- **Biome & Ecosystem Tool**: Ecosystem simulation with procedural flora, fauna, and resource distribution.
- **Asteroid Belt Tool**: GPU-accelerated asteroid belts and dust fields.
- **Planetary System Tool**: Real-time control of planetary orbits and system visualization.
- **Planetary Data Optimization Tool**: World Partition-based streaming, LOD management, and memory profiling.
- **Visualization & Debugging Suite**: Real-time overlays and performance diagnostics.

> **Note:** The terrain system is now a separate plugin: **Unreal Terrain Kit (UTK)**.

---

### 🔹 2. Unreal Terrain Kit (UTK)

> **Standalone plugin for procedural terrain creation**

A powerful node-based terrain generation tool, designed to replace third-party solutions like Gaea or World Machine and
fully operate within Unreal Engine.

#### Key Features

- 🧠 **Graph-Based Node Editor** – Built using `WorkflowOrientedApp` with full multi-tab layout.
- ⚙️ **GPU-Accelerated** – Fast terrain calculations and erosion processing.
- 🌄 **Nanite Support** – Output compatible with high-detail runtime rendering.
- 🔁 **Realtime Iteration** – Immediate feedback for terrain edits inside the viewport.
- 📤 **Export Support** – Outputs masks and heightmaps for reuse in other tools (e.g., Planet Creator)
