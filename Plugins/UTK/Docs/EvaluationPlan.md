# UTK Node Evaluation System - Design & Roadmap

This document captures the design decisions, triggers, data flow, and implementation roadmap for the node evaluation system to be added to the UTK plugin. It references the existing editor/asset code in the plugin (notably `UUTKAsset`, `UUTKGraph`, `UUTKNode`, and `FUTKEditorApp`) and proposes a concrete, incremental plan to implement evaluation and caching behavior similar to the Material Editor and other graph-based editors in Unreal.

Goals
- Evaluate a selected node on demand and cache the result.
- Only re-evaluate when necessary: first evaluation, changes to node properties, upstream nodes changed, or explicit user request.
- Keep evaluation and caching fast and memory sensible; allow manual invalidation and an editor-visible cache state.
- Integrate cleanly with the existing working-copy editor (`FUTKEditorApp`) and the asset save lifecycle.

Overview of existing code (key symbols)
- `UUTKAsset` (Plugins/UTK/Source/UTK/Public/Assets/UTKAsset.h)
  - Data asset that owns a `UUTKGraph` (instanced) and serialized editable properties (e.g. `TerrainScale`).
- `UUTKGraph` (Plugins/UTK/Source/UTK/Public/Graph/UTKGraph.h)
  - Subclass of `UEdGraph` that contains `UUTKNode` nodes.
- `UUTKNode` (Plugins/UTK/Source/UTK/Public/Graph/Nodes/UTKNode.h)
  - Graph node type with runtime properties (instanced structs), pins (via `AllocateDefaultPins`) and a node definition (`FUTKNodeDefinition`).
- `FUTKEditorApp` (Plugins/UTK/Source/UTK/Private/UI/Toolkit/UTKEditorApp.cpp/.h)
  - Asset editor that creates a transient editable working copy of the asset/graph and tracks `bWorkingDirty` to manage saves.

Design contract (inputs/outputs/behaviour)
- Inputs: a target `UUTKNode` to evaluate, a query region/seed/context (depending on node type), and the current `UUTKGraph` state.
- Output: a typed evaluation result (for terrain nodes this may be a heightfield, float texture, or other data structure) and diagnostic metadata (duration, success, upstream versions).
- Error modes: evaluation may fail for missing inputs or invalid parameters: node should return a clear diagnostic and avoid caching invalid results.
- Success criteria: node evaluation runs on demand, caches results, invalidates correctly when inputs change, and exposes UI to force re-eval / clear cache.

Data shapes and storage
- Evaluation result type: a lightweight serializable struct, e.g. `FUTKNodeEvalResult`:
  - TWeakObjectPtr<UObject> OwnerNode (debug)
  - TArray<float> HeightData OR TArray<uint8> SerializedTextureData (depending on node)
  - FVector2D Resolution / Bounds
  - uint64 CacheVersion (monotonic increment per evaluation)
  - FDateTime Timestamp
- Cache storage location:
  - Non-serialized runtime cache stored on `UUTKNode` or in a new `UUTKEvaluationCache` UCLASS that maps node GUID -> `FUTKNodeEvalResult`.
  - Serialized cache (optional): if results are expensive to recompute and should survive editor restarts, add a `UUTKGraphSaveData`/`UUTKGraphNodeSaveData` object to store compressed results on the asset side (defer to future work).

Cache invalidation rules (when to re-evaluate)
1. No cached result exists for the selected node -> evaluate.
2. Selected node properties changed (property change events) -> invalidate node's cache -> evaluate when requested.
3. Upstream node changed (a node that feeds into the selected node through an input pin) -> propagate invalidation downstream -> mark selected node dirty.
4. Graph structural changes that affect connectivity -> recompute dependency graph and invalidate affected nodes.
5. Explicit user action -> "Re-evaluate node" or "Clear cache".

Change detection strategies
- Node-local change detection: each `UUTKNode` keeps a `uint64 NodeRevision` counter incremented when node properties change (via `PostEditChangeProperty` / editor property-change hooks). The `FUTKEvalKey` includes the `NodeRevision`.
- Graph-level change detection: maintain a per-graph `uint64 GraphRevision` updated on graph edits (pin connect/disconnect, node add/remove). When graph revision changes, downstream nodes are validated against the revision they were evaluated at.
- Upstream dependency hashing: compute a lightweight hash for node inputs (e.g. combine upstream NodeRevision values + parameter hashes) and store it with the cached result. On evaluation request compare hash; if different, invalidate.

Evaluation flow (happy path)
1. User selects a node in the editor (or user clicks "Evaluate").
2. Editor queries evaluation subsystem: "Do you have a valid cached result for Node X?"
3. If valid -> show result (preview) immediately.
4. If not valid -> schedule a synchronous or asynchronous evaluation (start with synchronous for simplicity).
5. On evaluation completion: store `FUTKNodeEvalResult` in runtime cache, set node diagnostic (success/time), and update preview UI.

Integration with `FUTKEditorApp` and save lifecycle
- Working copy vs original: keep evaluation cache strictly per-working copy (stored on the transient `WorkingObject` or in an editor-only manager). Avoid writing cache into the saved asset unless explicitly requested — this prevents spurious package dirty states.
- When `ApplyWorkingToOriginal()` is called (user saves), only copy *editable* properties and the graph structure — do not copy transient cached results. That preserves save behavior like Material Editor.
- Editor should not mark `EditingObject` dirty simply because a node was evaluated; any cache stored on working object must be transient (non-CPF_Edit and within transient package or as UPROPERTY with CPF_Transient).

UI: node properties panel and evaluation controls
- Add a small evaluation section in the node properties panel (SDetails customization):
  - Preview thumbnail / small viewport (if applicable).
  - Buttons: Evaluate / Re-evaluate / Clear Cache.
  - A small status line: Cached / Needs Eval / Last evaluated Xs ago / Error message.

Concurrency and long-running evaluations
- Start with synchronous evals (blocking) to keep implementation simple.
- Design evaluation functions to be reentrant and allow future migration to background worker threads.
- Plan for a task queue and cancellation token (if a new eval request supersedes a running one).

Diagnostic & telemetry
- Each `UUTKNode` should expose `FUTKNodeDiagnostics` (already present) with fields for the last run duration, status, and messages. Show these in the properties panel.

Implementation roadmap (phases)
Phase 1 — Minimal runtime cache and editor hooks (approx 1–2 days)
- Create `FUTKNodeEvalResult` struct and `UUTKEvaluationCache` UCLASS (editor-only, transient) that maps Node GUID -> result.
- Hook property-change notifications in `FUTKNode` (or listen to editor delegates in `FUTKEditorApp`) to increment `NodeRevision` and mark the node as needing eval.
- Add `EvaluateNode(UUTKNode* Node)` function entrypoint in a new `UTK::Evaluation` module (or in `Graph/Core`) that returns `FUTKNodeEvalResult`.
- Wire up a simple UI: add Evaluate/Clear buttons in the node details customization.
- Ensure cache is stored on the working copy only (avoid marking the saved asset dirty).

Phase 2 — Dependency tracking and invalidation (1–2 days)
- Implement dependency traversal: from a selected node walk upstream pins and collect upstream NodeRevisions/hashes.
- Store the dependency signature with the cached result, and validate on request.
- Update graph change handlers (e.g. `OnWorkingGraphChanged`, `OnObjectPropertyChanged`) to update `GraphRevision` and invalidate affected nodes.

Phase 3 — Background evaluation & persistence (2–4 days)
- Move heavy evaluations to an asynchronous task queue with cancellation support.
- Add optional serialized cache (graph-node save data). Make saving the cache explicit via an editor action so running evaluation doesn't mark asset dirty.

Phase 4 — Performance tuning and visual preview (ongoing)
- Implement streaming / tiling for large results (heightfields/textures).
- Add preview thumbnails and incremental previews.

Concrete file hooks & references to modify
- `FUTKEditorApp` (`Plugins/UTK/Source/UTK/Private/UI/Toolkit/UTKEditorApp.*`)
  - Ensure evaluation cache lives in the working copy: add a UPROPERTY transient pointer to an evaluation manager (editor-only). Do NOT copy the cache into `EditingObject` during `ApplyWorkingToOriginal()`.
  - Add menu/toolbar actions to "Evaluate Selected Node" / "Clear Node Cache" that call into the evaluation subsystem.
- `UUTKNode` (`Plugins/UTK/Source/UTK/Public/Graph/Nodes/UTKNode.h` / .cpp)
  - Add NodeRevision field and small helper APIs: `IncrementRevision()`, `bool IsCacheValid(const FDependencySignature&)`.
  - Expose `GetRuntimeProperties()` / `GetPropertyHash()` for hashing inputs.
- `UUTKGraph` / `UTKGraphSchema`
  - Update OnGraphChanged hooks to mark graph revision and propagate node invalidation.
- New files to add (suggested):
  - `Source/UTK/Public/Evaluation/UTKEvaluationTypes.h` (structs)
  - `Source/UTK/Private/Evaluation/UTKEvaluationManager.h/.cpp` (cache manager, evaluation entrypoints)
  - `Source/UTK/Private/UI/Details/UTKNodeDetailsCustomization.*` (properties UI)

Edge cases and testing checklist
- Empty graphs / nodes with missing inputs -> evaluation should fail gracefully and not cache.
- Circular dependencies -> detect and report an error to the user (prevent stack overflow).
- Large results to avoid excessive memory usage: cap resolution by default; add streaming/tile strategy if needed.
- Undo/Redo interactions: changes that increment `NodeRevision` must be transactional (use `Modify()` and transactions) while keeping caches transient.

Quality gates and tests
- Unit tests for dependency signature hashing and cache invalidation.
- Small editor smoke test: select node -> Evaluate -> see preview; change a property upstream -> Evaluate again -> cache invalidated and updated.

Next actionable steps (short term)
1. Add a transient `UTKEvaluationManager` to the working copy in `FUTKEditorApp::InitializeWorkingAsset` (editor-only). Store its lifetime with the working asset.
2. Implement `FUTKEvalResult` struct and a synchronous `EvaluateNode()` stub that returns a deterministic, small result (for now: a float value or small array).
3. Add Evaluate/Clear buttons to the node details panel and hook them to call `EvaluateNode()` through the editor.
4. Confirm that evaluating nodes no longer marks the saved asset as dirty (cache kept on working copy and transient).

Notes about save behavior and recent code changes
- Recent edits to `FUTKEditorApp::InitializeWorkingAsset()` replaced flag-negation duplication with `DuplicateObject` for the working copy and duplicated the original graph into the working copy. This change reduces bad object flags leaking into the saved package and is expected to fix spurious dirty flags and the "save prompt on close without modifications" bug.
- Important: keep all evaluation cache data transient (non-saved) to avoid reintroducing dirty state. If you do want persistent caches, make saving explicit and controlled by the user.

References and inspiration
- Material Editor and Blueprint Editor: look at how they manage working copies, caches (preview materials), and details customizations.
- `FEdGraph` and `UEdGraphNode` APIs for graph traversal and pin iteration.

If you want, I can: 
- Implement Phase 1 (create `UTKEvaluationManager`, `FUTKNodeEvalResult`, add basic Evaluate/Clear UI and wiring) now.
- Or produce a minimal prototype that evaluates a selected node to a deterministic float and shows it in the details panel so you can validate the save behavior.

Choose next step: "Implement Phase 1 now" or "Create prototype eval stub" or ask for changes to the plan.

