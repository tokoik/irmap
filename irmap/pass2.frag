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

// ラスタライザから受け取る頂点属性の補間値
in vec2 texcoord;                                     // テクスチャ座標

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                    // フラグメントの色

// カラーのレンダーターゲットのテクスチャ
uniform sampler2D color[4];

// 法線方向のサンプル点の数
uniform int diffuseSamples;

// 法線方向のミップマップのレベル
uniform int diffuseLod;

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
vec4 sample(in vec3 vector, in int lod)
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
  return textureLod(image, t, lod);

#elif MODE == FISHEYE

  //
  // 等距離射影方式の魚眼レンズ画像の場合
  //

  // レンズを上に向けた場合のテクスチャ座標を求める
  vec2 t = fisheye_fov * acos(vector.y) * normalize(vector.xz) + 0.5;
  return textureLod(image, t, lod);

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
  vec4 color_b = textureLod(image, t_b, lod);
  vec4 color_f = textureLod(image, t_f, lod);

  // サンプリングした色をブレンドする
  return mix(color_f, color_b, blend);

#else

  // そのままテクスチャを貼る
  return textureLod(image, texcoord, lod);

#endif
}

// ノイズ発生
uint rand(in vec2 co)
{
  return uint(fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453) * 4294967293.0) + 1u;
}

// 乱数発生
float xorshift(inout uint y)
{
  // shift して xor する
  y = y ^ (y << 13);
  y = y ^ (y >> 17);
  y = y ^ (y << 5);

  // [0, 1] に正規化して返す
  return float(y) * 2.3283064e-10;
}

// サンプル点の生成
vec4 sampler(inout uint seed, in float e)
{
  float z = pow(xorshift(seed), e);
  float r = sqrt(1.0 - z * z);
  float t = 6.2831853 * xorshift(seed);
  vec3 s = normalize(vec3(vec2(cos(t), sin(t)) * r, z));
  return vec4(s, 0.0);
}

void main(void)
{
  // color[0] から albedo を取得
  vec4 albedo = texture(color[0], texcoord);

  // albedo のアルファ値が 0 なら背景色
  if (albedo.a == 0.0)
  {
    fc = vec4(0.0);
    return;
  }

  vec4 fresnel = texture(color[1], texcoord);         // フレネル項 (鏡面反射係数)
  vec3 p = texture(color[2], texcoord).xyz;           // 頂点の位置
  vec3 n = texture(color[3], texcoord).xyz;           // 頂点の法線

  // 陰影
  vec3 v = normalize(p);                              // 視線ベクトル

  // サンプル点を法線方向に回転する変換行列
  vec3 zn = vec3(-n.y, n.x, 0.0);
  float len = length(zn);
  vec3 t = mix(vec3(1.0, 0.0, 0.0), zn / len, step(0.001, len));
  vec3 b = cross(n, t);
  mat3 m = mat3(t, b, n);

  // 乱数のタネ
  uint seed = rand(gl_FragCoord.xy);
  //uint seed = 2463534242u;

  // 放射照度
  vec4 idiff = vec4(0.0);

  // 法線側の個々のサンプル点について
  for (int i = 0; i < diffuseSamples; ++i)
  {
    // サンプル点を生成する
    vec4 d = sampler(seed, 0.5);

    // サンプル点を法線側に回転する
    vec3 l = m * d.xyz;

    // 法線側のサンプル点方向の色を累積する
    idiff += sample(l, diffuseLod);
  }

  // 正反射方向
  vec3 r = reflect(v, n);

  // 正反射方向の色
  vec4 s = sample(r, 0);

  // 画素の陰影を求める
  fresnel.a = 0.0;
  fc = mix(albedo * idiff / float(diffuseSamples), s, fresnel);
}
