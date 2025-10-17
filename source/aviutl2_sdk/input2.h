//----------------------------------------------------------------------------------
//	���̓v���O�C�� �w�b�_�[�t�@�C�� for AviUtl ExEdit2
//	By �j�d�m����
//----------------------------------------------------------------------------------

// ���̓t�@�C�����\����
// �摜�t�H�[�}�b�g��RGB24bit,RGBA32bit,PA64,HF64,YUY2,YC48���Ή����Ă��܂�
// �����t�H�[�}�b�g��PCM16bit,PCM(float)32bit���Ή����Ă��܂�
// ��PA64��DXGI_FORMAT_R16G16B16A16_UNORM(��Z�ς݃�)�ł�
// ��HF64��DXGI_FORMAT_R16G16B16A16_FLOAT(��Z�ς݃�)�ł�(�����t�H�[�}�b�g)
// ��YC48�͌݊��Ή��̃t�H�[�}�b�g�ł�
struct INPUT_INFO {
	int	flag;					// �t���O
	static constexpr int FLAG_VIDEO = 1;			// �摜�f�[�^����
	static constexpr int FLAG_AUDIO = 2;			// �����f�[�^����
	static constexpr int FLAG_TIME_TO_FRAME = 16;	// �t���[���ԍ������Ԃ���Z�o���� ��func_time_to_frame()���Ă΂��悤�ɂȂ�
	int	rate, scale;			// �t���[�����[�g�A�X�P�[��
	int	n;						// �t���[����
	BITMAPINFOHEADER* format;	// �摜�t�H�[�}�b�g�ւ̃|�C���^(���Ɋ֐����Ă΂��܂œ��e��L���ɂ��Ă���)
	int	format_size;			// �摜�t�H�[�}�b�g�̃T�C�Y
	int audio_n;				// �����T���v����
	WAVEFORMATEX* audio_format;	// �����t�H�[�}�b�g�ւ̃|�C���^(���Ɋ֐����Ă΂��܂œ��e��L���ɂ��Ă���)
	int audio_format_size;		// �����t�H�[�}�b�g�̃T�C�Y
};

// ���̓t�@�C���n���h��
typedef void* INPUT_HANDLE;

// ���̓v���O�C���\����
struct INPUT_PLUGIN_TABLE {
	int flag;					// �t���O
	static constexpr int FLAG_VIDEO = 1;		// �摜���T�|�[�g����
	static constexpr int FLAG_AUDIO = 2;		// �������T�|�[�g����
	static constexpr int FLAG_CONCURRENT = 16;	// �f�[�^�̓����擾���T�|�[�g����
												// ������n���h���ŉ摜�Ɖ����̎擾�֐��������ɌĂ΂��
												// ���قȂ�n���h���Ŋe�֐��������ɌĂ΂��
	static constexpr int FLAG_MULTI_TRACK = 32;	// �}���`�g���b�N���T�|�[�g���� ��func_set_track()���Ă΂��悤�ɂȂ�
	LPCWSTR name;				// �v���O�C���̖��O
	LPCWSTR filefilter;			// ���̓t�@�C���t�B���^
	LPCWSTR information;		// �v���O�C���̏��

	// ���̓t�@�C�����I�[�v������֐��ւ̃|�C���^
	// file		: �t�@�C����
	// �߂�l	: TRUE�Ȃ���̓t�@�C���n���h��
	INPUT_HANDLE (*func_open)(LPCWSTR file);

	// ���̓t�@�C�����N���[�Y����֐��ւ̃|�C���^
	// ih		: ���̓t�@�C���n���h��
	// �߂�l	: TRUE�Ȃ琬��
	bool (*func_close)(INPUT_HANDLE ih);

	// ���̓t�@�C���̏����擾����֐��ւ̃|�C���^
	// ih		: ���̓t�@�C���n���h��
	// iip		: ���̓t�@�C�����\���̂ւ̃|�C���^
	// �߂�l	: TRUE�Ȃ琬��
	bool (*func_info_get)(INPUT_HANDLE ih, INPUT_INFO* iip);

	// �摜�f�[�^��ǂݍ��ފ֐��ւ̃|�C���^
	// ih		: ���̓t�@�C���n���h��
	// frame	: �ǂݍ��ރt���[���ԍ�
	// buf		: �f�[�^��ǂݍ��ރo�b�t�@�ւ̃|�C���^
	// �߂�l	: �ǂݍ��񂾃f�[�^�T�C�Y
	int (*func_read_video)(INPUT_HANDLE ih, int frame, void* buf);

	// �����f�[�^��ǂݍ��ފ֐��ւ̃|�C���^
	// ih		: ���̓t�@�C���n���h��
	// start	: �ǂݍ��݊J�n�T���v���ԍ�
	// length	: �ǂݍ��ރT���v����
	// buf		: �f�[�^��ǂݍ��ރo�b�t�@�ւ̃|�C���^
	// �߂�l	: �ǂݍ��񂾃T���v����
	int (*func_read_audio)(INPUT_HANDLE ih, int start, int length, void* buf);

	// ���͐ݒ�̃_�C�A���O��v�����ꂽ���ɌĂ΂��֐��ւ̃|�C���^ (nullptr�Ȃ�Ă΂�܂���)
	// hwnd			: �E�B���h�E�n���h��
	// dll_hinst	: �C���X�^���X�n���h��
	// �߂�l		: TRUE�Ȃ琬��
	bool (*func_config)(HWND hwnd, HINSTANCE dll_hinst);

	// ���̓t�@�C���̓ǂݍ��ݑΏۃg���b�N��ݒ肷��֐��ւ̃|�C���^ (FLAG_MULTI_TRACK���L���̎��̂݌Ă΂�܂�)
	// func_open()�̒���Ƀg���b�N���擾�A�g���b�N�ԍ��ݒ肪�Ă΂�܂��B���I�[�v������̐ݒ�ȍ~�͌Ă΂�܂���
	// ih		: ���̓t�@�C���n���h��
	// type		: ���f�B�A��� ( 0 = �f�� / 1 = ���� )
	// index	: �g���b�N�ԍ� ( -1 ���w�肳�ꂽ�ꍇ�̓g���b�N���̎擾 )
	// �߂�l	: �ݒ肵���g���b�N�ԍ� (���s�����ꍇ�� -1 ��ԋp)
	//			  �g���b�N���̎擾�̏ꍇ�͐ݒ�\�ȃg���b�N�̐� (���f�B�A�������ꍇ�� 0 ��ԋp)
	int (*func_set_track)(INPUT_HANDLE ih, int type, int index);
	static constexpr int TRACK_TYPE_VIDEO = 0;
	static constexpr int TRACK_TYPE_AUDIO = 1;

	// �f���̎��Ԃ���Y���t���[���ԍ����Z�o���鎞�ɌĂ΂��֐��ւ̃|�C���^ (FLAG_TIME_TO_FRAME���L���̎��̂݌Ă΂�܂�)
	// �摜�f�[�^��ǂݍ��ޑO�ɌĂяo����A���ʂ̃t���[���ԍ��œǂݍ��ނ悤�ɂȂ�܂��B
	// ��FLAG_TIME_TO_FRAME�𗘗p����ꍇ��INPUT_INFO��rate,scale���͕��σt���[�����[�g��\���l��ݒ肵�Ă�������
	// ih		: ���̓t�@�C���n���h��
	// time		: �f���̎���(�b)
	// �߂�l	: �f���̎��ԂɑΉ�����t���[���ԍ�
	int (*func_time_to_frame)(INPUT_HANDLE ih, double time);

};
