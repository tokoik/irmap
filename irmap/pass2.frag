#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass2.frag
//
//   �t���[���o�b�t�@�I�u�W�F�N�g�̓��e��`�悷��V�F�[�_
//

#define NONE      0
#define PANORAMA  1
#define FISHEYE   2
#define THETA     3
#define MODE THETA

// ���X�^���C�U����󂯎�钸�_�����̕�Ԓl
in vec2 texcoord;                                     // �e�N�X�`�����W

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec4 fc;                    // �t���O�����g�̐F

// �J���[�̃����_�[�^�[�Q�b�g�̃e�N�X�`��
uniform sampler2D color[4];

// �@�������̃T���v���_�̐�
uniform int diffuseSamples;

// �@�������̃~�b�v�}�b�v�̃��x��
uniform int diffuseLod;

// ���̃e�N�X�`��
uniform sampler2D image;

// THETA: ���e�N�X�`���̃T�C�Y
vec2 size = textureSize(image, 0);

// THETA: ���e�N�X�`���̌���J�������̃e�N�X�`����ԏ�̔��a�ƒ��S
vec2 radius_b = vec2(-0.25, 0.25 * size.x / size.y);
vec2 center_b = vec2( 0.25, radius_b.t);

// THETA: ���e�N�X�`���̑O���J�������̃e�N�X�`����ԏ�̔��a�ƒ��S
vec2 radius_f = vec2( 0.25, radius_b.t);
vec2 center_f = vec2( 0.75, center_b.t);

// FISHEYE: �~���p�ŕ\������p�̋t��
//const float fisheye_fov = 0.31830989; // 180��
//const float fisheye_fov = 0.27813485; // 206��
const float fisheye_fov = 0.24381183; // 235��
//const float fisheye_fov = 0.21220659; // 270��

// ���}�b�v�̃T���v�����O
vec4 sample(in vec3 vector, in int lod)
{
#if MODE == PANORAMA

  //
  // �����~���}�@�̃p�m���}�摜�̏ꍇ
  //

  // �o�x�����̃x�N�g��
  vec2 u = vector.xy;

  // �ܓx�����̃x�N�g��
  vec2 v = vec2(vector.z, length(vector.xz));

  // �ܓx�ƌo�x����e�N�X�`�����W�����߂�
  vec2 t = atan(u, v) * vec2(-0.15915494, -0.31830989) + 0.5;
  return textureLod(image, t, lod);

#elif MODE == FISHEYE

  //
  // �������ˉe�����̋��჌���Y�摜�̏ꍇ
  //

  // �����Y����Ɍ������ꍇ�̃e�N�X�`�����W�����߂�
  vec2 t = fisheye_fov * acos(vector.y) * normalize(vector.xz) + 0.5;
  return textureLod(image, t, lod);

#elif MODE == THETA

  //
  // RICOH THETA S �̃��C�u�X�g���[�~���O�摜�̏ꍇ
  //

  // ���̕����x�N�g���̑��ΓI�ȋp
  float angle = 1.0 - acos(vector.z) * 0.63661977;

  // �O��̃e�N�X�`���̍�����
  float blend = smoothstep(-0.02, 0.02, angle);

  // ���̕����x�N�g���� yx ��ł̕����x�N�g��
  vec2 orientation = normalize(vector.yx) * 0.885;

  // ���ƕ\�̃e�N�X�`�����W�����߂�
  vec2 t_b = (1.0 - angle) * orientation * radius_b + center_b;
  vec2 t_f = (1.0 + angle) * orientation * radius_f + center_f;

  // ���ƕ\�̊��}�b�v���T���v�����O����
  vec4 color_b = textureLod(image, t_b, lod);
  vec4 color_f = textureLod(image, t_f, lod);

  // �T���v�����O�����F���u�����h����
  return mix(color_f, color_b, blend);

#else

  // ���̂܂܃e�N�X�`����\��
  return textureLod(image, texcoord, lod);

#endif
}

// �m�C�Y����
uint rand(in vec2 co)
{
  return uint(fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453) * 4294967293.0) + 1u;
}

// ��������
float xorshift(inout uint y)
{
  // shift ���� xor ����
  y = y ^ (y << 13);
  y = y ^ (y >> 17);
  y = y ^ (y << 5);

  // [0, 1] �ɐ��K�����ĕԂ�
  return float(y) * 2.3283064e-10;
}

// �T���v���_�̐���
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
  // color[0] ���� albedo ���擾
  vec4 albedo = texture(color[0], texcoord);

  // albedo �̃A���t�@�l�� 0 �Ȃ�w�i�F
  if (albedo.a == 0.0)
  {
    fc = vec4(0.0);
    return;
  }

  vec4 fresnel = texture(color[1], texcoord);         // �t���l���� (���ʔ��ˌW��)
  vec3 p = texture(color[2], texcoord).xyz;           // ���_�̈ʒu
  vec3 n = texture(color[3], texcoord).xyz;           // ���_�̖@��

  // �A�e
  vec3 v = normalize(p);                              // �����x�N�g��

  // �T���v���_��@�������ɉ�]����ϊ��s��
  vec3 zn = vec3(-n.y, n.x, 0.0);
  float len = length(zn);
  vec3 t = mix(vec3(1.0, 0.0, 0.0), zn / len, step(0.001, len));
  vec3 b = cross(n, t);
  mat3 m = mat3(t, b, n);

  // �����̃^�l
  uint seed = rand(gl_FragCoord.xy);
  //uint seed = 2463534242u;

  // ���ˏƓx
  vec4 idiff = vec4(0.0);

  // �@�����̌X�̃T���v���_�ɂ���
  for (int i = 0; i < diffuseSamples; ++i)
  {
    // �T���v���_�𐶐�����
    vec4 d = sampler(seed, 0.5);

    // �T���v���_��@�����ɉ�]����
    vec3 l = m * d.xyz;

    // �@�����̃T���v���_�����̐F��ݐς���
    idiff += sample(l, diffuseLod);
  }

  // �����˕���
  vec3 r = reflect(v, n);

  // �����˕����̐F
  vec4 s = sample(r, 0);

  // ��f�̉A�e�����߂�
  fresnel.a = 0.0;
  fc = mix(albedo * idiff / float(diffuseSamples), s, fresnel);
}
