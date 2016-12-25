#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass1.vert
//
//   マルチプルレンダーターゲットに描画するシェーダ
//

// 変換行列
uniform mat4 mw;                                      // 視点座標系への変換行列
uniform mat4 mc;                                      // クリッピング座標系への変換行列
uniform mat4 mg;                                      // 法線ベクトルの変換行列

// 頂点属性
layout (location = 0) in vec4 pv;                     // ローカル座標系での頂点の位置
layout (location = 1) in vec4 nv;                     // ローカル座標系での頂点の法線

// ラスタライザに送る頂点属性
out vec4 p;                                           // 頂点の位置
out vec3 n;                                           // 頂点の法線

void main(void)
{
  gl_Position = mc * pv;

  p = mw * pv;                                        // 視点座標系での頂点の位置
  n = normalize((mg * nv).xyz);                       // 視点座標系での頂点の法線
}
