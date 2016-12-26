#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass1.frag
//
//   画素単位に陰影付けを行うシェーダ
//

// 光源
uniform vec4 lamb;                                    // 環境光成分
uniform vec4 ldiff;                                   // 拡散反射光成分
uniform vec4 lspec;                                   // 鏡面反射光成分
uniform vec4 lpos;                                    // 位置

// 材質
uniform vec4 kamb;                                    // 環境光の反射係数
uniform vec4 kdiff;                                   // 拡散反射係数
uniform vec4 kspec;                                   // 鏡面反射係数
uniform float kshi;                                   // 輝き係数

// ラスタライザから受け取る頂点属性の補間値
in vec4 p;                                            // 頂点の位置
in vec3 n;                                            // 頂点の法線

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                    // フラグメントの色

void main(void)
{
  // 陰影
  vec3 v = normalize(p.xyz / p.w);                    // 視線ベクトル
  vec3 l = normalize((lpos * p.w - p * lpos.w).xyz);  // 光線ベクトル
  vec3 h = normalize(l - v);                          // 中間ベクトル

  // 拡散反射光成分
  vec4 idiff = max(dot(n, l), 0.0) * kdiff * ldiff + kamb * lamb;

  // 鏡面反射光成分
  vec4 ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;

  // 画素の陰影を求める
  fc = idiff + ispec;
}
