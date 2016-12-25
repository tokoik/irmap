#version 150 core

//
// pass2.vert
//
//   フレームバッファオブジェクトの内容を描画するシェーダ
//

// ラスタライザに送る頂点属性
out vec2 texcoord;                                    // テクスチャ座標

void main()
{
  // テクスチャ座標を求める
  texcoord = vec2(gl_VertexID & 1, gl_VertexID >> 1);

  // テクスチャ座標から頂点座標を求めて出力
  gl_Position = vec4(texcoord * 2.0 - 1.0, 0.0, 1.0);
}
