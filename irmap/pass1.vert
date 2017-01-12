#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// pass1.vert
//
//   �}���`�v�������_�[�^�[�Q�b�g�ɕ`�悷��V�F�[�_
//

// �ϊ��s��
uniform mat4 mw;                                      // ���_���W�n�ւ̕ϊ��s��
uniform mat4 mc;                                      // �N���b�s���O���W�n�ւ̕ϊ��s��
uniform mat4 mg;                                      // �@���x�N�g���̕ϊ��s��

// ���_����
layout (location = 0) in vec4 pv;                     // ���[�J�����W�n�ł̒��_�̈ʒu
layout (location = 1) in vec4 nv;                     // ���[�J�����W�n�ł̒��_�̖@��

// ���X�^���C�U�ɑ��钸�_����
out vec4 p;                                           // ���_�̈ʒu
out vec3 n;                                           // ���_�̖@��

void main(void)
{
  gl_Position = mc * pv;

  p = mw * pv;                                        // ���_���W�n�ł̒��_�̈ʒu
  n = normalize((mg * nv).xyz);                       // ���_���W�n�ł̒��_�̖@��
}
