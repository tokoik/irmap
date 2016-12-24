#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass1.vert
//
//   頂点単位に陰影付けを行うシェーダ
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

// 変換行列
uniform mat4 mw;                                      // 視点座標系への変換行列
uniform mat4 mc;                                      // クリッピング座標系への変換行列
uniform mat4 mg;                                      // 法線ベクトルの変換行列

// 頂点属性
layout (location = 0) in vec4 pv;                     // ローカル座標系での頂点の位置
layout (location = 1) in vec4 nv;                     // ローカル座標系での頂点の法線

// ラスタライザに送る頂点属性
out vec4 idiff;                                       // 拡散反射光強度
out vec4 ispec;                                       // 鏡面反射光強度

void main(void)
{
  gl_Position = mc * pv;

  vec4 p = mw * pv;                                   // 視点座標系での頂点の位置
  vec3 n = normalize((mg * nv).xyz);                  // 視点座標系での頂点の法線

  // 陰影
  vec3 v = normalize(p.xyz / p.w);                    // 視線ベクトル
  vec3 l = normalize((lpos * p.w - p * lpos.w).xyz);  // 光線ベクトル
  vec3 h = normalize(l - v);                          // 中間ベクトル

  // 拡散反射光成分
  idiff = max(dot(n, l), 0.0) * kdiff * ldiff + kamb * lamb;

  // 鏡面反射光成分
  ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;
}
