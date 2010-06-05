#version 150 compatibility

precision highp float;

uniform sampler2D image;
in vec2 uv;

void main() {
  float x = 5.0 / 768.0;
  gl_FragColor = texture2D(image, uv + vec2(-16.0 * x, 0.0)) * 0.00418146514886757
    + texture2D(image, uv + vec2(-15.0 * x, 0.0)) * 0.00573729720806776
    + texture2D(image, uv + vec2(-14.0 * x, 0.0)) * 0.00771299521877476
    + texture2D(image, uv + vec2(-13.0 * x, 0.0)) * 0.0101595769356863
    + texture2D(image, uv + vec2(-12.0 * x, 0.0)) * 0.0131118819981262
    + texture2D(image, uv + vec2(-11.0 * x, 0.0)) * 0.0165802581471795
    + texture2D(image, uv + vec2(-10.0 * x, 0.0)) * 0.0205425518248068
    + texture2D(image, uv + vec2(-9.0 * x, 0.0)) * 0.0249375820436808
    + texture2D(image, uv + vec2(-8.0 * x, 0.0)) * 0.0296613654482998
    + texture2D(image, uv + vec2(-7.0 * x, 0.0)) * 0.0345672463627964
    + texture2D(image, uv + vec2(-6.0 * x, 0.0)) * 0.0394707407930915
    + texture2D(image, uv + vec2(-5.0 * x, 0.0)) * 0.0441593444046262
    + texture2D(image, uv + vec2(-4.0 * x, 0.0)) * 0.04840684796659
    + texture2D(image, uv + vec2(-3.0 * x, 0.0)) * 0.0519909602458754
    + texture2D(image, uv + vec2(-2.0 * x, 0.0)) * 0.0547123942781217
    + texture2D(image, uv + vec2(-1.0 * x, 0.0)) * 0.0564131628472774
    + texture2D(image, uv + vec2(0.0 * x, 0.0)) * 0.0569917543430618
    + texture2D(image, uv + vec2(1.0 * x, 0.0)) * 0.0564131628472774
    + texture2D(image, uv + vec2(2.0 * x, 0.0)) * 0.0547123942781217
    + texture2D(image, uv + vec2(3.0 * x, 0.0)) * 0.0519909602458754
    + texture2D(image, uv + vec2(4.0 * x, 0.0)) * 0.04840684796659
    + texture2D(image, uv + vec2(5.0 * x, 0.0)) * 0.0441593444046262
    + texture2D(image, uv + vec2(6.0 * x, 0.0)) * 0.0394707407930915
    + texture2D(image, uv + vec2(7.0 * x, 0.0)) * 0.0345672463627964
    + texture2D(image, uv + vec2(8.0 * x, 0.0)) * 0.0296613654482998
    + texture2D(image, uv + vec2(9.0 * x, 0.0)) * 0.0249375820436808
    + texture2D(image, uv + vec2(10.0 * x, 0.0)) * 0.0205425518248068
    + texture2D(image, uv + vec2(11.0 * x, 0.0)) * 0.0165802581471795
    + texture2D(image, uv + vec2(12.0 * x, 0.0)) * 0.0131118819981262
    + texture2D(image, uv + vec2(13.0 * x, 0.0)) * 0.0101595769356863
    + texture2D(image, uv + vec2(14.0 * x, 0.0)) * 0.00771299521877476
    + texture2D(image, uv + vec2(15.0 * x, 0.0)) * 0.00573729720806776
    + texture2D(image, uv + vec2(16.0 * x, 0.0)) * 0.00418146514886757;
}
