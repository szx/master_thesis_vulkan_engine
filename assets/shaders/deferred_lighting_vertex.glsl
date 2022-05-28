
// fullscreen triangle directly into clip space
// NOTE: fragment depth == 0 combined with depth test op VK_COMPARE_OP_LESS ensures, that fragment shader is
// executed only for fragments written to by deferred geometry pipeline.
// HIRO Encoding and decoding G-Buffer pattern in common.gls. baseColor needs only 8*3 bits

outPosition = inPosition;
gl_Position = vec4(inPosition, 1.0);