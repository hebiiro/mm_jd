//----------------------------------------------------------------------------------
//	�t�B���^�v���O�C�� �w�b�_�[�t�@�C�� for AviUtl ExEdit2
//	By �j�d�m����
//----------------------------------------------------------------------------------

// �g���b�N�o�[���ڍ\����
struct FILTER_ITEM_TRACK {
	FILTER_ITEM_TRACK(LPCWSTR name, double value, double s, double e, double step = 1.0) : name(name), value(value), s(s), e(e), step(step) {}
	LPCWSTR type = L"track";	// �ݒ�̎��
	LPCWSTR name;				// �ݒ薼
	double value;				// �ݒ�l (�t�B���^�����̌Ăяo�����Ɍ��݂̒l�ɍX�V����܂�)
	const double s, e;			// �ݒ�l�̍ŏ��A�ő�
	const double step;			// �ݒ�l�̒P��( 1.0 / 0.1 / 0.01 / 0.001 )
};

// �`�F�b�N�{�b�N�X���ڍ\����
struct FILTER_ITEM_CHECK {
	FILTER_ITEM_CHECK(LPCWSTR name, bool value) : name(name), value(value) {}
	LPCWSTR type = L"check";	// �ݒ�̎��
	LPCWSTR name;				// �ݒ薼
	bool value;					// �ݒ�l (�t�B���^�����̌Ăяo�����Ɍ��݂̒l�ɍX�V����܂�)
};

// �F�I�����ڍ\����
struct FILTER_ITEM_COLOR {
	union VALUE { // �ݒ�l�̐F
		int code;
		struct { unsigned char b, g, r, x; };
	};
	FILTER_ITEM_COLOR(LPCWSTR name, int code) : name(name) { value.code = code; }
	LPCWSTR type = L"color";	// �ݒ�̎��
	LPCWSTR name;				// �ݒ薼
	VALUE value;				// �ݒ�l (�t�B���^�����̌Ăяo�����Ɍ��݂̒l�ɍX�V����܂�)
};

// �I�����X�g���ڍ\����
struct FILTER_ITEM_SELECT {
	struct ITEM {		// �I��������
		LPCWSTR name;	// �I�����̖��O
		int value;		// �I�����̒l
	};
	FILTER_ITEM_SELECT(LPCWSTR name, int value, ITEM* list) : name(name), value(value), list(list) {}
	LPCWSTR type = L"select";	// �ݒ�̎��
	LPCWSTR name;				// �ݒ薼
	int value;					// �ݒ�l (�t�B���^�����̌Ăяo�����Ɍ��݂̒l�ɍX�V����܂�)
	ITEM* list;					// �I�������X�g (ITEM��񋓂��Ė��O��null��ITEM�ŏI�[�������X�g�ւ̃|�C���^)
};

// �t�@�C���I�����ڍ\����
struct FILTER_ITEM_FILE {
	FILTER_ITEM_FILE(LPCWSTR name, LPCWSTR value, LPCWSTR filefilter) : name(name), value(value), filefilter(filefilter) {}
	LPCWSTR type = L"file";		// �ݒ�̎��
	LPCWSTR name;				// �ݒ薼
	LPCWSTR value;				// �ݒ�l (�t�B���^�����̌Ăяo�����Ɍ��݂̒l�ɍX�V����܂�)
	LPCWSTR filefilter;			// �t�@�C���t�B���^
};

//----------------------------------------------------------------------------------

// RGBA32bit�\����
struct PIXEL_RGBA {
	unsigned char r, g, b, a;
};

// �V�[�����\����
struct SCENE_INFO {
	int width, height;		// �V�[���̉𑜓x
	int rate, scale;		// �V�[���̃t���[�����[�g
	int sample_rate;		// �V�[���̃T���v�����O���[�g
};

// �I�u�W�F�N�g���\����
struct OBJECT_INFO {
	int64_t id;				// �I�u�W�F�N�g��ID (�A�v���N�����̌ŗLID)
	int frame;				// �I�u�W�F�N�g�̌��݂̃t���[���ԍ�
	int frame_total;		// �I�u�W�F�N�g�̑��t���[����
	double time;			// �I�u�W�F�N�g�̌��݂̎���(�b)
	double time_total;		// �I�u�W�F�N�g�̑�����(�b)
	int width, height;		// �I�u�W�F�N�g�̌��݂̉摜�T�C�Y (�摜�t�B���^�̂�)
	int64_t sample_index;	// �I�u�W�F�N�g�̌��݂̉����T���v���ʒu (�����t�B���^�̂�)
	int64_t sample_total;	// �I�u�W�F�N�g�̑��T���v���� (�����t�B���^�̂�)
	int sample_num;			// �I�u�W�F�N�g�̌��݂̉����T���v���� (�����t�B���^�̂�)
	int channel_num;		// �I�u�W�F�N�g�̌��݂̉����`�����l���� (�����t�B���^�̂�) ���ʏ�2�ɂȂ�܂�
};

//----------------------------------------------------------------------------------

// �摜�t�B���^�����p�\����
struct FILTER_PROC_VIDEO {
	// �V�[�����
	const SCENE_INFO* scene;

	// �I�u�W�F�N�g���
	const OBJECT_INFO* object;

	// ���݂̉摜�̃f�[�^���擾����
	// buffer		: �摜�f�[�^�̊i�[��ւ̃|�C���^
	void (*get_image_data)(PIXEL_RGBA* buffer);

	// ���݂̉摜�̃f�[�^��ݒ肵�܂�
	// buffer		: �摜�f�[�^�ւ̃|�C���^
	// width,height	: �摜�T�C�Y
	void (*set_image_data)(PIXEL_RGBA* buffer, int width, int height);

};

// �����t�B���^�����p�\����
struct FILTER_PROC_AUDIO {
	// �V�[�����
	const SCENE_INFO* scene;

	// �I�u�W�F�N�g���
	const OBJECT_INFO* object;

	// ���݂̉����̃f�[�^���擾����
	// buffer		: �����f�[�^�̊i�[��ւ̃|�C���^ �������f�[�^��PCM(float)32bit
	// channel		: �����f�[�^�̃`�����l�� ( 0 = ���`�����l�� / 1 = �E�`�����l�� )
	void (*get_sample_data)(float* buffer, int channel);

	// ���݂̉����̃f�[�^��ݒ肷��
	// buffer		: �����f�[�^�ւ̃|�C���^ �������f�[�^��PCM(float)32bit
	// channel		: �����f�[�^�̃`�����l�� ( 0 = ���`�����l�� / 1 = �E�`�����l�� )
	void (*set_sample_data)(float* buffer, int channel);

};

//----------------------------------------------------------------------------------

// �t�B���^�v���O�C���\����
struct FILTER_PLUGIN_TABLE {
	int flag;								// �t���O
	static constexpr int FLAG_VIDEO = 1;	// �摜�t�B���^���T�|�[�g����
	static constexpr int FLAG_AUDIO = 2;	// �����t�B���^���T�|�[�g����
											// �摜�Ɖ����̃t�B���^�����͕ʁX�̃X���b�h�ŏ�������܂�
	static constexpr int FLAG_INPUT = 4;	// �I�u�W�F�N�g�̏������͂����� (���f�B�A�I�u�W�F�N�g�ɂ���ꍇ)
	LPCWSTR name;				// �v���O�C���̖��O
	LPCWSTR label;				// ���x���̏����l (nullptr�Ȃ�f�t�H���g�̃��x���ɂȂ�܂�)
	LPCWSTR information;		// �v���O�C���̏��
	void** items;				// �ݒ荀�ڂ̒�` (FILTER_ITEM_XXX�|�C���^��񋓂���null�I�[�������X�g�ւ̃|�C���^)

	// �摜�t�B���^�����֐��ւ̃|�C���^ (FLAG_VIDEO���L���̎��̂݌Ă΂�܂�)
	bool (*func_proc_video)(FILTER_PROC_VIDEO* video);

	// �����t�B���^�����֐��ւ̃|�C���^ (FLAG_AUDIO���L���̎��̂݌Ă΂�܂�)
	bool (*func_proc_audio)(FILTER_PROC_AUDIO* audio);

};
