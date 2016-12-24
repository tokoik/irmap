#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass1.frag
//
//   頂点単位に陰影付けを行うシェーダ
//

// ラスタライザから受け取る頂点属性の補間値
in vec4 idiff;                                        // 拡散反射光強度
in vec4 ispec;                                        // 鏡面反射光強度

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                    // フラグメントの色

void main(void)
{
  // 画素の陰影を求める
  fc = idiff + ispec;
}
