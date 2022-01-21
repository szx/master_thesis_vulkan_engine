### Cel

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
	- [ ] Asset pipeline
		- [x] INI configuration
		- [x] glTF scene input
		- [x] Processing primitives
		- [x] Processing meshes
		- [x] Processing nodes.
		- [ ] **Processing scenes.**
		- [ ] Processing images
		- [ ] LOD generation using meshoptimizer.
		- [x] SQLite database output
	- [ ] GPU-driven rendering pipeline:
		- [ ] Building uploading to GPU scene geometry buffer
			- [ ] **Building scene geometry buffer**
			- [ ] Uploading scene geometry buffer to GPU
			- [x] One big vertex buffer
			- [ ] One big uniform buffer with object data indexed with object ID in vertex & fragment shaders
		- [ ] LOD selection & culling using compute shader
		- [ ] Rendering whole scene using vkCmdDrawIndexedIndirect
		- [ ] Maybe: Mesh Cluster Rendering
	- [ ] Render graph.
		- [x] Research.
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
	- [ ] Forward rendering vs deferred rendering.
	- [ ] Interleaved vs noninterleaved vertex attributes.


#### Praca pisemna

- [ ] 120 000 znaków (60-70 stron)
- [ ] Tekst zajmuje ok. 3/4 pracy.
- [ ] Spis treści:
	- [ ] ...
- [ ] CONTINUE: extracts summary
