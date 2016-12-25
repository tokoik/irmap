#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass2.frag
//
//   フレームバッファオブジェクトの内容を描画するシェーダ
//

// 光源
uniform vec4 lamb;                                    // 環境光成分
uniform vec4 ldiff;                                   // 拡散反射光成分
uniform vec4 lspec;                                   // 鏡面反射光成分
uniform vec4 lpos;                                    // 位置

// ラスタライザから受け取る頂点属性の補間値
in vec2 texcoord;                                     // テクスチャ座標

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                    // フラグメントの色

// カラーのレンダーターゲットのテクスチャ
uniform sampler2D color[4];


void main(void)
{
  vec3 p = texture(color[2], texcoord).xyz;           // 頂点の位置
  vec3 n = texture(color[3], texcoord).xyz;           // 頂点の法線

  // 陰影
  vec3 v = normalize(p);                              // 視線ベクトル
  vec3 l = normalize(lpos.xyz - p * lpos.w);          // 光線ベクトル
  vec3 h = normalize(l - v);                          // 中間ベクトル

  // 拡散反射光成分
  vec4 kdiff = texture(color[0], texcoord);
  vec4 idiff = kdiff * (max(dot(n, l), 0.0) * ldiff + lamb);

  // 鏡面反射光成分
  vec4 kspec = texture(color[1], texcoord);
  float kshi = kspec.a * 128.0;
  vec4 ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;

  // 画素の陰影を求める
  fc = idiff + ispec;
}
