# Hamming line-matching witness format (version 1)

This generalises to any integer `fibres` with 1 ≤ fibres ≤ 4096 and `clones` with 0 ≤ clones ≤ 1,000,000, subject to the checker’s maximum graph-order and memory safeguards.

The base graph has `Nbase = fibres × 256` vertices. Fibre `f` consists of the 256 four-component base-4 coordinate tuples `(x0,x1,x2,x3)`, encoded `256 f + x0 + 4*x1 + 16*x2 + 64*x3`.

Each fibre contains 256 distinct *lines*: for each of four coordinate axes, fix the remaining three coordinates and let that axis vary over 0,1,2,3. Line ID `f*256+axis*64+index`, where `index` is the base-4 number formed by the three fixed coordinates in increasing axis order. Every base vertex lies on exactly four lines.

Every line `L` has a partner line `partner[L]` in a *different fibre*, and this partner map is a fixed-point-free involution. Each unordered line pair creates a complete bipartite graph K4,4 between its two four-vertex lines. The decoder checks all partner bounds, reciprocity, loops and duplicated edges. Every base vertex therefore has 16 neighbours.

After constructing the entire base, for each clone-source base vertex `s[j]` append a new vertex adjacent to all 16 **original** neighbours of `s[j]`. All original source neighbourhoods are snapshotted before creating any clone; a clone is a false twin of its source in the original base. Sources must be distinct and the checker verifies the resulting maximum degree from scratch.

### Binary representation

- 8-byte ASCII magic `H4LINE01`.
- 4-byte unsigned little-endian `fibres`.
- 4-byte unsigned little-endian `clones`.
- Let `L = fibres × 256`, `b = ceil(log2 L)` (integer bit width).
- Pack `partner[0]` through `partner[L-1]` as b-bit unsigned values, **least significant bit first**, concatenated into a single little-endian bitstream.
- Continue the same bitstream (no alignment) with `source[0]` through `source[clones-1]`, also b-bit unsigned values.
- Trailing zero padding only, to a whole number of bytes; no extra bytes permitted.

Length in bytes is `16 + ceil((L + clones) * b / 8)`.
For this construction, `fibres=624`, `clones=7936`, `L=159744`, `b=18`, and file length = **377,296 bytes**.

The decoder checks the entire graph by BFS from every vertex to the diameter limit. A maintainer does not need the original graph archive or external documents to verify the mathematical witness.
