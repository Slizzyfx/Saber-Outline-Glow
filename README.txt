SLIZZY NEON OUTLINE v3.0.0 — EXPERIMENTAL PLUG-IN

New in v3
- Three user-selectable colour pickers (Colour 1/2/3) and custom multicolour gradient.
- Glow Threshold restored as its own control.
- Edge Smoothness control; pre-smoothing plus multiple bloom scales.
- Bright core, medium bloom and soft outer halo.
- Neon-only output at Blend with Original = 0%; blend back toward source at higher values.
- Lower default flicker for a calmer, smoother appearance.

Install
1. In EffectCraft choose Effect > Load Effect Plug-in…
2. Select Slizzy_Neon_Outline_v3.wasm.
3. Apply “Slizzy Neon Outline v3” from Effects & Presets.

Important limitations
- Experimental build. It has been compiled and its WebAssembly module format checked, but not run inside your particular EffectCraft installation.
- This is luminance/alpha edge detection, not semantic character segmentation. On unmasked anime scenes it may outline background details too. For a character-only outline, use a cutout/mask first.
- Smoothness and bloom add processing cost; on a Celeron N4020, use quarter/half preview resolution while adjusting.
