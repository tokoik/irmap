#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass2.frag
//
//   フレームバッファオブジェクトの内容を描画するシェーダ
//

// ラスタライザから受け取る頂点属性の補間値
in vec2 texcoord;                                     // テクスチャ座標

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                    // フラグメントの色

// カラーのレンダーターゲットのテクスチャ
uniform sampler2D color;

void main(void)
{
  // 画素の陰影を求める
  fc = texture(color, texcoord);
}
