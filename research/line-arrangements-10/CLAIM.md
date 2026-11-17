# Explicit construction: at least 1,000,001 ten-line arrangements

**Public research claim dated 18 November 2026.**

Let `A(10)` denote the number of combinatorially inequivalent arrangements of ten distinct straight lines in the real affine plane, in general position (no parallel pair, no triple concurrence), using the line-arrangement convention of Leaps in Bounds.

## Result

\[
A(10)\ge 1,000,001.
\]

The same one million and one arrangements also give this lower bound in the unrestricted affine-line-arrangement category.

This is a **construction claim**, not a claim that all historic published work has been surpassed, nor a claim of acceptance by Leaps in Bounds.

## Constructive witness and reproducibility

The exact witness is 1,000,001 rows of 30 integers, with each row encoding ten lines
\[
-i\,x+y=t_i,\qquad i=0,1,\ldots,9,\quad t_0=0.
\]

The intercepts are generated deterministically by `research/line-arrangements-10/generate_stream.cpp` using `std::mt19937_64(20261118)`. Candidates with triple intersections or an already seen canonical vertex signature are rejected.

The precise uncompressed witness from the original certificate has SHA-256:

`41f887098b9f9dc2c4d84d4c512738649fa3c940fa4880173a6e99b47869bf64`

The original 31,806,488-byte xz witness SHA-256 is:

`6a467ea11f55e3352ebbf48f7b1e5f774e0a0d4056bcaeb37925b34534e26ca3`

The generator (and matching witness hash) fully specifies the construction. The SHA-256 of a newly compressed copy may differ with xz threading/options despite identical uncompressed content.

Reproduction, on Linux with a C++17 compiler:

```sh
g++ -O3 -std=c++17 research/line-arrangements-10/generate_stream.cpp -o /tmp/gen
g++ -O3 -std=c++17 research/line-arrangements-10/independent_vertex_audit.cpp -o /tmp/audit
/tmp/gen 1000001 > /tmp/ten-lines.txt
printf '41f887098b9f9dc2c4d84d4c512738649fa3c940fa4880173a6e99b47869bf64  /tmp/ten-lines.txt\n' | sha256sum -c -
/tmp/audit < /tmp/ten-lines.txt
```

The independent C++ check requires all 45 pairwise intersections for every row to be ordinary, and computes all 40 infinity-end renumberings/reversals of the vertex covector set. Two topologically equivalent affine arrangements necessarily have the same canonical vertex signature; all 1,000,001 signatures were distinct.

The full original ZIP underwent an independent clean reproduction, with exact-rational cross-checks against the complete Leaps in Bounds-style covector method for 109 spread-sampled witnesses.

## Site status and limitation

On 18 November 2026, the publicly checked Leaps in Bounds lower bound for 10 lines in general position was 4,000. This construction is larger, but the site requires under 500 kB of in-repository certificate evidence; the compressed million-row witness is 31.8 MB. The correct full submission route is a permanent external witness link and site-compliant attestation and maintainer review, not pretending the public site accepted it already.

Research code is publicly available in this branch. The accompanying GitHub Actions workflow is intended to regenerate, audit and permanently commit the large witness file to this repository branch.

## Provenance

Computer-assisted construction and independent computational checks carried out interactively with OpenAI GPT-6 in November 2026. User-commissioned research. The connected GitHub contributor account is `bargainlandz-code`. Formal third-party review and historic priority determination remain outstanding.
