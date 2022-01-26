### Termin

[Regulamin studiów:](https://pg.edu.pl/documents/10754/0/Regulamin%20studi%C3%B3w%202021_2022.pdf)

```
Student składa pracę dyplomową w formie papierowej nie później niż:  
1) do końca prowadzenia zajęć w semestrze dyplomowym - na studiach pierwszego stopnia,  
2) do 30 kwietnia - na studiach drugiego stopnia kończących się w semestrze zimowym,  
3) do 30 września - na studiach drugiego stopnia kończących się w semestrze letnim.
```

### Cel

**Temat**: "Silnik renderujący używający technik cieniowania odroczonego."
Co należy wyprodukować:

- silnik renderujący
- pracę magisterską
	- Luźny temat. Akcenty rozłożyć jak chcę.
- Co tydzień wysyłać info.

### Wytyczne

#### Silnik graficzny

- [ ] Debugging & performance
	- [ ] Validation layers
		- [x] Basic debugging layers: (VK_LAYER_LUNARG_standard_validation)
		- [ ] _SIGGRAPH 2021: "Simplify Vulkan Development with new Ecosystem Enhancements"_
			- [ ] VK_LAYER_LUNARG_device_simulation
			- [ ] GPU assisted validation
			- [ ] Debug printf for shaders
			- [ ] VK_KHR_synchronization2
	- [ ] VK_EXT_debug_utils
		- [x] Object debug names
		- [ ] Command buffer debug markers
	- [ ] Measure draw time using query pools and vkCmdWriteTimestamp()
	- [ ] Thirdparty tools integration:
		- [x] RenderDoc
		- [ ] Intel GPA
		- [ ] Maybe: GPUView

- [ ] Rendering
	- [ ] Asset pipeline (30.01)
		- [x] INI configuration
        - [x] glTF scene input
        - [x] Processing primitives
        - [x] Processing meshes
        - [x] Processing nodes.
        - [x] Processing scenes.
        - [ ] Processing images
        - [ ] Maybe: LOD generation using meshoptimizer.
		- [x] SQLite database output
		- [x] Write test comparing outputs of glFT with asset database.
    - [ ] GPU-driven rendering pipeline (14.02):
		- [x] Loading scene data from asset database.
		- [ ] **Research**
		- [ ] Scene geometry buffer
			- [ ] **Building scene geometry buffer**
			- [ ] Uploading scene geometry buffer to GPU
			- [x] One big vertex buffer
			- [ ] One big uniform buffer with object data indexed with object ID in vertex & fragment shaders
        - [ ] Culling using compute shader
        - [ ] Maybe: LOD selection
        - [ ] Rendering whole scene using vkCmdDrawIndexedIndirect
        - [ ] Maybe: Mesh Cluster Rendering
    - [ ] Render graph (30.02).
		- [ ] **Research**.
		- [ ] Implementation.
		- [ ] Forward rendering.
		- [ ] Deferred rendering.
			- [ ] Lighting model.
				- [ ] Phong model.
				- [ ] Maybe: PBR (BRDF).
			- [ ] G-buffer compression.
		- [ ] Post-processing effects.
			- [ ] Bloom.
			- [ ] Motion blur.
			- [ ] Maybe: SSAO
	- [ ] Rendering example scenes.

- [ ] Experiments:
	- [ ] Building and running on Windows.
	- [ ] Example frame in modern game analyzed using RenderDoc.
	- [ ] **Forward rendering vs deferred rendering.**
	- [ ] Interleaved vs noninterleaved vertex attributes.

#### Praca pisemna

- [ ] 120 000 znaków (60-70 stron)
- [ ] Tekst zajmuje ok. 3/4 pracy.
- [ ] Spis treści:
	- [ ] ...
- [ ] Źródła:
	- [ ] Sergey Kosarevsky, Viktor Latypov, "3D Graphics Rendering Cookbook: A comprehensive guide to exploring
	  rendering algorithms in modern OpenGL and Vulkan", 1 Edycja
	- [ ] Randima Fernando, Mark J. Kilgard, "Język Cg"
	- [ ] The Khronos® Vulkan Working Group, "Vulkan® 1.2.203 - A Specification (with all registered Vulkan extensions)"
- [ ] CONTINUE: extracts summary
