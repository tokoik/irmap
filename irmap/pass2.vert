#version 150 core

//
// pass2.vert
//
//   �t���[���o�b�t�@�I�u�W�F�N�g�̓��e��`�悷��V�F�[�_
//

// ���X�^���C�U�ɑ��钸�_����
out vec2 texcoord;                                    // �e�N�X�`�����W

void main()
{
  // �e�N�X�`�����W�����߂�
  texcoord = vec2(gl_VertexID & 1, gl_VertexID >> 1);

  // �e�N�X�`�����W���璸�_���W�����߂ďo��
  gl_Position = vec4(texcoord * 2.0 - 1.0, 0.0, 1.0);
}
