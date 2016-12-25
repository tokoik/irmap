#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass1.frag
//
//   マルチプルレンダーターゲットに描画するシェーダ
//

// 材質
uniform vec4 kamb;                                    // 環境光の反射係数
uniform vec4 kdiff;                                   // 拡散反射係数
uniform vec4 kspec;                                   // 鏡面反射係数
uniform float kshi;                                   // 輝き係数

// ラスタライザから受け取る頂点属性の補間値
in vec4 p;                                            // 頂点の位置
in vec3 n;                                            // 頂点の法線

// フレームバッファに出力するデータ
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 fresnel;
layout (location = 2) out vec3 position;
layout (location = 3) out vec3 normal;

void main(void)
{
  color = vec4(kdiff.rgb, kamb.a);
  fresnel = vec4(kspec.rgb, kshi * 0.0078125);
  position = p.xyz / p.w;
  normal = normalize(n);
}
