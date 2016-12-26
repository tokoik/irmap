#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass2.frag
//
//   フレームバッファオブジェクトの内容を描画するシェーダ
//

#define NONE      0
#define PANORAMA  1
#define FISHEYE   2
#define THETA     3
#define MODE THETA

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

// 環境のテクスチャ
uniform sampler2D image;

// THETA: 環境テクスチャのサイズ
vec2 size = textureSize(image, 0);

// THETA: 環境テクスチャの後方カメラ像のテクスチャ空間上の半径と中心
vec2 radius_b = vec2(-0.25, 0.25 * size.x / size.y);
vec2 center_b = vec2( 0.25, radius_b.t);

// THETA: 環境テクスチャの前方カメラ像のテクスチャ空間上の半径と中心
vec2 radius_f = vec2( 0.25, radius_b.t);
vec2 center_f = vec2( 0.75, center_b.t);

// FISHEYE: 円周角で表した画角の逆数
//const float fisheye_fov = 0.31830989; // 180°
//const float fisheye_fov = 0.27813485; // 206°
const float fisheye_fov = 0.24381183; // 235°
//const float fisheye_fov = 0.21220659; // 270°

// 環境マップのサンプリング
vec4 sample(in vec3 vector)
{
#if MODE == PANORAMA

  //
  // 正距円筒図法のパノラマ画像の場合
  //

  // 経度方向のベクトル
  vec2 u = vector.xy;

  // 緯度方向のベクトル
  vec2 v = vec2(vector.z, length(vector.xz));

  // 緯度と経度からテクスチャ座標を求める
  vec2 t = atan(u, v) * vec2(-0.15915494, -0.31830989) + 0.5;
  return texture(image, t);

#elif MODE == FISHEYE

  //
  // 等距離射影方式の魚眼レンズ画像の場合
  //

  // レンズを上に向けた場合のテクスチャ座標を求める
  vec2 t = fisheye_fov * acos(vector.y) * normalize(vector.xz) + 0.5;
  return texture(image, t);

#elif MODE == THETA

  //
  // RICOH THETA S のライブストリーミング画像の場合
  //

  // この方向ベクトルの相対的な仰角
  float angle = 1.0 - acos(vector.z) * 0.63661977;

  // 前後のテクスチャの混合比
  float blend = smoothstep(-0.02, 0.02, angle);

  // この方向ベクトルの yx 上での方向ベクトル
  vec2 orientation = normalize(vector.yx) * 0.885;

  // 裏と表のテクスチャ座標を求める
  vec2 t_b = (1.0 - angle) * orientation * radius_b + center_b;
  vec2 t_f = (1.0 + angle) * orientation * radius_f + center_f;

  // 裏と表の環境マップをサンプリングする
  vec4 color_b = texture(image, t_b);
  vec4 color_f = texture(image, t_f);

  // サンプリングした色をブレンドする
  return mix(color_f, color_b, blend);
#else

  // そのままテクスチャを貼る
  return texture(image, texcoord);

#endif
}

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

  // 正反射方向
  vec3 r = reflect(v, n);

  // 正反射方向の色
  vec4 s = sample(r);

  // 画素の陰影を求める
  fc = mix(idiff, s, kspec) + ispec;
}
