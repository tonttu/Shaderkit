#version 150 compatibility

precision highp float;

uniform sampler2D image;
uniform float s = 5.0;
in vec2 uv;

void main() {
  float y = s / 768.0;
  gl_FragColor = texture2D(image, uv + vec2(0.0, -16.0 * y)) * 0.00418146514886757
    + texture2D(image, uv + vec2(0.0, -15.0 * y)) * 0.00573729720806776
    + texture2D(image, uv + vec2(0.0, -14.0 * y)) * 0.00771299521877476
    + texture2D(image, uv + vec2(0.0, -13.0 * y)) * 0.0101595769356863
    + texture2D(image, uv + vec2(0.0, -12.0 * y)) * 0.0131118819981262
    + texture2D(image, uv + vec2(0.0, -11.0 * y)) * 0.0165802581471795
    + texture2D(image, uv + vec2(0.0, -10.0 * y)) * 0.0205425518248068
    + texture2D(image, uv + vec2(0.0, -9.0 * y)) * 0.0249375820436808
    + texture2D(image, uv + vec2(0.0, -8.0 * y)) * 0.0296613654482998
    + texture2D(image, uv + vec2(0.0, -7.0 * y)) * 0.0345672463627964
    + texture2D(image, uv + vec2(0.0, -6.0 * y)) * 0.0394707407930915
    + texture2D(image, uv + vec2(0.0, -5.0 * y)) * 0.0441593444046262
    + texture2D(image, uv + vec2(0.0, -4.0 * y)) * 0.04840684796659
    + texture2D(image, uv + vec2(0.0, -3.0 * y)) * 0.0519909602458754
    + texture2D(image, uv + vec2(0.0, -2.0 * y)) * 0.0547123942781217
    + texture2D(image, uv + vec2(0.0, -1.0 * y)) * 0.0564131628472774
    + texture2D(image, uv + vec2(0.0, 0.0 * y)) * 0.0569917543430618
    + texture2D(image, uv + vec2(0.0, 1.0 * y)) * 0.0564131628472774
    + texture2D(image, uv + vec2(0.0, 2.0 * y)) * 0.0547123942781217
    + texture2D(image, uv + vec2(0.0, 3.0 * y)) * 0.0519909602458754
    + texture2D(image, uv + vec2(0.0, 4.0 * y)) * 0.04840684796659
    + texture2D(image, uv + vec2(0.0, 5.0 * y)) * 0.0441593444046262
    + texture2D(image, uv + vec2(0.0, 6.0 * y)) * 0.0394707407930915
    + texture2D(image, uv + vec2(0.0, 7.0 * y)) * 0.0345672463627964
    + texture2D(image, uv + vec2(0.0, 8.0 * y)) * 0.0296613654482998
    + texture2D(image, uv + vec2(0.0, 9.0 * y)) * 0.0249375820436808
    + texture2D(image, uv + vec2(0.0, 10.0 * y)) * 0.0205425518248068
    + texture2D(image, uv + vec2(0.0, 11.0 * y)) * 0.0165802581471795
    + texture2D(image, uv + vec2(0.0, 12.0 * y)) * 0.0131118819981262
    + texture2D(image, uv + vec2(0.0, 13.0 * y)) * 0.0101595769356863
    + texture2D(image, uv + vec2(0.0, 14.0 * y)) * 0.00771299521877476
    + texture2D(image, uv + vec2(0.0, 15.0 * y)) * 0.00573729720806776
    + texture2D(image, uv + vec2(0.0, 16.0 * y)) * 0.00418146514886757;
}
