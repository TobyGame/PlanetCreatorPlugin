# UTK — Phase 4 Development Plan

*(3D Viewport + Terrain Preview Rendering & Editing Baseline)*

Last updated: 2025-12-17

## Phase 4 Goals

Phase 4 focuses on replacing the Phase 3 “2D preview” workflow with a **single 3D viewport preview** and establishing
the **terrain preview representation** that stays faithful to Gaea’s behavior:

- **Only 3D viewport** is active for preview during Phase 4 (2D preview can return later as an optional mode).
- The preview uses **Resolution + Terrain Definition (Width/Height)**.
- The viewport keeps a **constant XY footprint** (the preview square does not visually resize).
- Changing **Height** stretches the terrain vertically.
- Changing **Width** flattens the terrain (reduces vertical exaggeration) while keeping the footprint constant.
- Multi-output node selection remains supported: the viewport can visualize a selected output pin/layer.

---

## Key Concepts and Definitions

### Terrain Definition (Gaea-style)

These are **global preview settings**, not per-node:

- **PreviewResolution**: heightfield sampling resolution (e.g. 512, 1024).
- **PreviewWidthMeters**: horizontal extent of the terrain square in meters (e.g. 5000 m).
- **PreviewMaxHeightMeters**: maximum elevation represented by heightfield values in meters (e.g. 2500 m).

Derived values (display + future simulation scale-awareness):

- **MetersPerPixel**: `PreviewWidthMeters / PreviewResolution`
    - Note: Gaea’s UI typically uses `/ Resolution` (not `/ (Resolution-1)`).
- **HeightWidthRatio**: `PreviewMaxHeightMeters / PreviewWidthMeters`
    - This ratio drives the **apparent steepness** in the viewport.

### Viewport Preview Footprint (Constant)

The preview mesh XY footprint remains constant in editor space:

- **PreviewFootprintUU**: constant square size in Unreal units for the preview plane.
    - Example: `PreviewFootprintUU = 200000` (depends on your desired visual scale).
- The preview mesh always fills:
    - `X, Y ∈ [-PreviewFootprintUU/2, +PreviewFootprintUU/2]`

### Height Mapping Rule (Matches Gaea behavior)

Let `h01` be a sampled height value in **[0..1]** from the selected layer.

- **Z mapping**:
    - `Z = h01 * PreviewFootprintUU * HeightWidthRatio`

This yields:

- Increasing **PreviewMaxHeightMeters** → ratio increases → terrain looks taller (stretched up).
- Increasing **PreviewWidthMeters** → ratio decreases → terrain looks flatter.
- Keeping `HeightWidthRatio` constant → steepness is consistent even if absolute Width/Height change.

---

## Phase 4 Scope Boundaries

### In Scope (Phase 4)

- 3D viewport tab integrated in UTK editor.
- Minimal preview scene: grey background, grid, simple lighting.
- Camera controls: orbit/pan/zoom + frame.
- Terrain preview mesh generation from evaluated node output (heightfield layer).
- Terrain Definition UI (Resolution / Width / Height) + derived readouts.
- Wiring: evaluation result → preview mesh update.
- Continue to support multi-output selection (output pin dropdown).

### Out of Scope (Deferred)

- Re-introducing 2D preview tab.
- Mask overlay visualization on terrain (texture projection).
- Runtime streaming/LOD/chunking.
- GPU mesh generation / compute erosion.
- Sculpt/paint editing tools in the viewport (true “interactive editing”).
- High-fidelity materials, biomes, splatmaps, tessellation/Nanite decisions.

---

## Phase 4 Deliverables and Sub-Phases

## 4.1 — 3D Viewport Tab Foundation

**Objective:** Create a dedicated 3D viewport tab as the primary preview surface.

### Deliverables

- `SUTK3DViewport` (inherits from `SEditorViewport`)
- `FUTK3DViewportClient` (inherits from `FEditorViewportClient`)
- `FPreviewScene` owned by the viewport client
- `FUTK3DViewportTab` (tab factory and app integration)

### Acceptance Criteria

- Opening a UTK asset shows the 3D viewport with a neutral background and grid.
- The viewport is stable across open/close of the editor.
- No terrain preview required yet (just the viewport and scene).

---

## 4.2 — Minimal Preview Scene

**Objective:** Establish a clean and predictable environment similar to Gaea’s preview.

### Scene Setup

- Grey background / simple clear color.
- Grid floor centered at origin.
- One directional light + skylight (simple, no tuning).

### Acceptance Criteria

- Grid is visible and centered.
- Lighting is sufficient to read geometry without special shading work.

---

## 4.3 — Viewport Controls (Orbit / Pan / Zoom / Frame)

**Objective:** Provide basic navigation.

### Required Interactions

- Orbit (RMB drag) around focus point.
- Pan (MMB drag).
- Zoom (mouse wheel).
- Frame terrain:
    - If terrain exists: focus bounds.
    - Otherwise: focus origin.

### Acceptance Criteria

- Controls feel predictable and do not interfere with graph interaction.
- Camera state persists while editor is open.

---

## 4.4 — Terrain Definition Settings (Resolution / Width / Height)

**Objective:** Add Gaea-like Terrain Definition parameters used by preview and later by simulation.

### Data Location (recommended)

- Store in the UTK working asset (e.g. `UUTKAsset`):
    - `PreviewResolution`
    - `PreviewWidthMeters`
    - `PreviewMaxHeightMeters`

### UI

- In the global property/details panel (“General” / “Preview” category):
    - Editable:
        - Resolution, Width (m), Height (m)
    - Read-only:
        - MetersPerPixel
        - Height–Width Ratio

### Acceptance Criteria

- Changing any value triggers preview regeneration automatically (Phase 3 behavior reused).
- Derived values update immediately in UI.

---

## 4.5 — Preview Actor/Component Architecture

**Objective:** Cleanly separate viewport concerns from mesh generation.

### Deliverables

- `AUTKTerrainPreviewActor` spawned into the preview scene.
- `UUTKTerrainPreviewComponent` that owns the preview mesh and material.

### Responsibilities

- The viewport client owns scene/actor lifetime.
- The component exposes a single update entrypoint:
    - `UpdateFromTerrain(const FUTKTerrain& Terrain, FName LayerName, const FUTKPreviewMapping& Mapping)`

### Acceptance Criteria

- Exactly one preview actor exists at a time in the preview scene.
- Updates reuse the actor/component; no accumulation of transient actors/meshes.

---

## 4.6 — Heightfield → Mesh Generation

**Objective:** Turn evaluated terrain output into a renderable mesh.

### Input

- `FUTKTerrain` (domain matches PreviewResolution)
- Selected preview layer name (from output-pin selection)
- Terrain Definition values (Width/Height)
- Viewport constant footprint value (PreviewFootprintUU)

### Mesh Rules

- XY footprint fixed in UU (constant).
- Z scaled using **HeightWidthRatio**.

### Vertex Layout (conceptual)

For each sample `(x, y)`:

- `u = x / (Nx - 1)`, `v = y / (Ny - 1)`
- `WorldX = (u - 0.5) * PreviewFootprintUU`
- `WorldY = (v - 0.5) * PreviewFootprintUU`
- `WorldZ = h01 * PreviewFootprintUU * HeightWidthRatio`

### Normals

- Compute normals from height differences (finite differences) or allow mesh utilities to compute normals.

### Mesh Backend Options (choose one for Phase 4)

- **Option A (recommended for Phase 4):** Build a transient `UStaticMesh` via `FMeshDescription`.
- **Option B:** Dynamic mesh component (later, if needed).

### Acceptance Criteria

- Constant node produces a flat plane at consistent height.
- Multi-output node shows distinct shapes per output selection.
- Width/Height changes replicate the Gaea feel:
    - Width↑ → flatter
    - Height↑ → taller
    - Same ratio → similar steepness

---

## 4.7 — Wiring Evaluation to the 3D Preview

**Objective:** The 3D viewport becomes the consumer of the evaluation system.

### Behavior

- `EvaluateCurrentSelectionForPreview()` evaluates:
    - Selected node (or focused node)
    - Selected output pin override (multi-output)
    - Produces: `FUTKTerrain + PreviewLayerName`

Then:

- Calls `Update3DPreviewMesh(Terrain, PreviewLayerName)`

### Trigger Sources (carried over from Phase 3)

- Node selection change
- Node property change
- Global preview settings change (Resolution/Width/Height/Seed)
- Connection topology change (pin rewires)

### Acceptance Criteria

- Preview updates automatically with no buttons.

---

## 4.8 — Terrain “Editing” Baseline (Phase 4 meaning)

**Objective:** Define “editing” for this phase as **node-driven editing** with 3D feedback.

### Definition

- Users edit terrain by:
    - Adjusting node parameters
    - Rewiring nodes
    - Selecting different outputs for preview
- The viewport provides live 3D visualization.

### Acceptance Criteria

- No brush tools yet; editing is graph-driven.
- Preview is stable and responsive enough for iteration.

---

## Implementation Order (Recommended)

1. **4.1**: 3D viewport tab skeleton (no terrain)
2. **4.2**: minimal scene + grid
3. **4.3**: camera controls
4. **4.4**: Terrain Definition settings + derived values + auto re-eval
5. **4.5**: preview actor/component
6. **4.6**: heightfield mesh generation (simple grid)
7. **4.7**: evaluation → mesh update wiring
8. Iterate on usability (frame, default focus, minor UI polish)

---

## Performance and Quality Notes

### Initial Performance Expectations

- Phase 4 preview is expected to rebuild meshes often.
- Resolution should be limited (e.g. 256–1024) for acceptable responsiveness.
- If rebuild becomes expensive, add:
    - Debounce on re-eval requests (next-tick queue).
    - Mesh reuse strategies (update vertex positions without rebuilding mesh asset).

### Data Semantics (Future-proofing)

- Keep the viewport mapping based on **HeightWidthRatio** to match Gaea.
- Also pass **MetersPerPixel** and **MaxHeightMeters** into evaluation context (later) to make simulation nodes
  scale-aware.

---

## Phase 4 Completion Checklist

Phase 4 is complete when:

- A UTK asset opens into a functional 3D viewport.
- Selecting nodes updates the 3D terrain preview automatically.
- Output pin selection updates the preview layer automatically.
- Changing Terrain Definition (Resolution/Width/Height) updates preview in a Gaea-like way.
- Rewiring inputs triggers correct re-evaluation and preview update.
- No 2D preview is required for the Phase 4 milestone.
