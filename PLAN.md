### Cel

### Wytyczne

#### Silnik graficzny

- [ ] Debugging & performance
	- [ ] Validation layers
		- [x] Basic debugging layers:
		  VK_LAYER_GOOGLE_unique_objects VK_LAYER_LUNARG_api_dump VK_LAYER_LUNARG_core_validation VK_LAYER_LUNARG_image
		  VK_LAYER_LUNARG_object_tracker VK_LAYER_LUNARG_parameter_validation VK_LAYER_LUNARG_swapchain
		  VK_LAYER_GOOGLE_threading
		- [ ] _SIGGRAPH: "Simplify Vulkan Development with new Ecosystem Enhancements"_
			- [ ] VK_LAYER_LUNARG_device_simulation
			- [ ] GPU assisted validation
			- [ ] Debug printf for shaders
			- [ ] VK_KHR_synchronization2
	- [ ] VK_EXT_debug_utils
		- [x] Object debug names
		- [ ] Command buffer debug markers
	- [ ] VK_KHR_performance_query
		- [ ] Show available performance counters.
	- [ ] Thirdparty tools integration:
		- [x] RenderDoc
		- [ ] Intel GPA
		- [ ] Maybe: GPUView

- [ ] Rendering
	- [ ] Asset pipeline
		- [x] INI configuration
		- [x] glTF scene input
		- [x] Processing primitives
		- [ ] **Processing meshes**
		- [ ] Processing scenes
		- [ ] Processing images
		- [x] SQLite database output
	- [ ] GPU-driven rendering pipeline:
		- [ ] Building uploading to GPU scene geometry buffer
			- [ ] **Building scene geometry buffer**
			- [ ] Uploading scene geometry buffer to GPU
			- [x] One big vertex buffer
			- [ ] One big uniform buffer with object data indexed with object ID in vertex & fragment shaders
		- [ ] MAYBE: Mesh Cluster Rendering
		- [ ] LOD selection & culling using compute shader
		- [ ] Rendering whole scene using vkCmdDrawIndexedIndirect
	- [ ] Render graph.
		- [x] Research.
        - [ ] Implementation.
        - [ ] Forward rendering.
        - [ ] Deferred rendering.
            - [ ] G-buffer compression
        - [ ] Post-processing effects
            - [ ] Motion blur

- [ ] Experiments:
	- [ ] Building and running on Windows.
	- [ ] Example frame in modern game analyzed using RenderDoc.
	- [ ] Rendering example scene.
	- [ ] Forward rendering vs deferred rendering.
	- [ ] Interleaved vs noninterleaved vertex attributes.

#### Praca pisemna

- [ ] 120 000 znaków (60-70 stron)
- [ ] Tekst zajmuje ok. 3/4 pracy.
- [ ] Spis treści:
	- [ ] ...
- [ ] CONTINUE: extracts summary
