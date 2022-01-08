### World space and clip space
- We use left-handed world coordinates:
  - +X points right
  - +Y points up
  - +Z points forward (which is what makes it left-handed)
  - [Left- vs. Right-handed coordinate systems](https://www.evl.uic.edu/ralph/508S98/coordinates.html)
- We use clip space with reversed-Z:
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
- We support only 1 section ([config]) in _config.ini_.
