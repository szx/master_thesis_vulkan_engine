### World space and clip space

- Use left-handed world coordinates:
  - +X points right
  - +Y points up
  - +Z points forward (which is what makes it left-handed)
  - [Left- vs. Right-handed coordinate systems](https://www.evl.uic.edu/ralph/508S98/coordinates.html)
- Use clip space with reversed-Z:
  - The origin is at the top left.
  - +X points right
  - +X points left
  - -Z points forward (reversed-Z)
  - [Perspective Projections](https://learnwebgl.brown37.net/08_projections/projections_perspective.html)
  - [The Indulgence of Engine Porting](http://whirlicube.com/the-indulgence-of-engine-porting.html)
- Front faces are counterclockwise.

### Config

- Global engine config examples:
    - window width and height
    - window title
- Global engine config in stored in the INI configuration file in _assets/config.ini_.
- Supports multiple config sections:
  - \[graphics\]
    - windowWidth
    - windowHeight
    - windowTitle
  - \[controls\]
    - enabled

### Debugging functionality

- Debug messengers using VK_EXT_debug_utils:
  - Active only in Debug builds.
  - Short-lived debug messenger for instance creation and destruction:
    - Produces validation layer errors.
  - Persistent debug messenger:
    - Produces validation layer errors.
    - Named objects.
    - Command buffer labels.
    - Doesn't use tags - objects have names, which allow for additional data tracking in vulkan_debug_data.
  - https://www.lunarg.com/wp-content/uploads/2018/05/Vulkan-Debug-Utils_05_18_v1.pdf