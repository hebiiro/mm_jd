//----------------------------------------------------------------------------------
//	�T���v��AVI(vfw�o�R)���̓v���O�C�� for AviUtl ExEdit2
//----------------------------------------------------------------------------------
#include <windows.h>
#include <vfw.h>
#pragma comment(lib, "vfw32.lib")

#include "input2.h"

INPUT_HANDLE func_open(LPCWSTR file);
bool func_close(INPUT_HANDLE ih);
bool func_info_get(INPUT_HANDLE ih, INPUT_INFO* iip);
int func_read_video(INPUT_HANDLE ih, int frame, void* buf);
int func_read_audio(INPUT_HANDLE ih, int start, int length, void* buf);
bool func_config(HWND hwnd, HINSTANCE dll_hinst);

//---------------------------------------------------------------------
//	���̓v���O�C���\���̒�`
//---------------------------------------------------------------------
INPUT_PLUGIN_TABLE input_plugin_table = {
	INPUT_PLUGIN_TABLE::FLAG_VIDEO | INPUT_PLUGIN_TABLE::FLAG_AUDIO, //	�t���O
	L"AVI File Reader2 (sample)",						// �v���O�C���̖��O
	L"AviFile (*.avi)\0*.avi\0",						// ���̓t�@�C���t�B���^
	L"Sample AVI File Reader2 version 2.01 By �j�d�m����",		// �v���O�C���̏��
	func_open,											// ���̓t�@�C�����I�[�v������֐��ւ̃|�C���^
	func_close,											// ���̓t�@�C�����N���[�Y����֐��ւ̃|�C���^
	func_info_get,										// ���̓t�@�C���̏����擾����֐��ւ̃|�C���^
	func_read_video,									// �摜�f�[�^��ǂݍ��ފ֐��ւ̃|�C���^
	func_read_audio,									// �����f�[�^��ǂݍ��ފ֐��ւ̃|�C���^
	func_config,										// ���͐ݒ�̃_�C�A���O��v�����ꂽ���ɌĂ΂��֐��ւ̃|�C���^ (nullptr�Ȃ�Ă΂�܂���)
};

//---------------------------------------------------------------------
//	�t�@�C���n���h���\����
//---------------------------------------------------------------------
struct FILE_HANDLE {
	int				flag;
	static constexpr int FLAG_VIDEO = 1;
	static constexpr int FLAG_AUDIO = 2;
	PAVIFILE		pfile;
	PAVISTREAM		pvideo, paudio;
	AVIFILEINFO		fileinfo;
	AVISTREAMINFO	videoinfo, audioinfo;
	void*			videoformat;
	LONG			videoformatsize;
	void*			audioformat;
	LONG			audioformatsize;
};

//---------------------------------------------------------------------
//	�v���O�C��DLL�������֐� (����`�Ȃ�Ă΂�܂���)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) bool InitializePlugin(DWORD version) { // version�͖{�̂̃o�[�W�����ԍ�
	return true;
}

//---------------------------------------------------------------------
//	�v���O�C��DLL�I���֐� (����`�Ȃ�Ă΂�܂���)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void UninitializePlugin() {
}

//---------------------------------------------------------------------
//	���̓v���O�C���\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) INPUT_PLUGIN_TABLE* GetInputPluginTable(void) {
	return &input_plugin_table;
}

//---------------------------------------------------------------------
//	�t�@�C���I�[�v��
//---------------------------------------------------------------------
INPUT_HANDLE func_open(LPCWSTR file) {
	FILE_HANDLE* fp = (FILE_HANDLE*)GlobalAlloc(GMEM_FIXED, sizeof(FILE_HANDLE));
	if (fp == NULL) return NULL;
	ZeroMemory(fp, sizeof(FILE_HANDLE));

	if (AVIFileOpen(&fp->pfile, file, OF_READ, nullptr) != S_OK) {
		GlobalFree(fp);
		return nullptr;
	}

	if (AVIFileInfo(fp->pfile, &fp->fileinfo, sizeof(fp->fileinfo)) == S_OK) {
		for (DWORD i = 0; i < fp->fileinfo.dwStreams; i++) {
			PAVISTREAM pas;
			if (AVIFileGetStream(fp->pfile, &pas, 0, i) == S_OK) {
				AVISTREAMINFO asi;
				AVIStreamInfo(pas, &asi, sizeof(asi));
				if (asi.fccType == streamtypeVIDEO) {
					//	�r�f�I�X�g���[���̐ݒ�
					fp->pvideo = pas;
					fp->videoinfo = asi;
					AVIStreamFormatSize(fp->pvideo, 0, &fp->videoformatsize);
					fp->videoformat = (FILE_HANDLE*)GlobalAlloc(GMEM_FIXED, fp->videoformatsize);
					if (fp->videoformat) {
						AVIStreamReadFormat(fp->pvideo, 0, fp->videoformat, &fp->videoformatsize);
						fp->flag |= FILE_HANDLE::FLAG_VIDEO;
					} else {
						AVIStreamRelease(pas);
					}
				} else if (asi.fccType == streamtypeAUDIO) {
					//	�I�[�f�B�I�X�g���[���̐ݒ�
					fp->paudio = pas;
					fp->audioinfo = asi;
					AVIStreamFormatSize(fp->paudio, 0, &fp->audioformatsize);
					fp->audioformat = (FILE_HANDLE*)GlobalAlloc(GMEM_FIXED, fp->audioformatsize);
					if (fp->videoformat) {
						AVIStreamReadFormat(fp->paudio, 0, fp->audioformat, &fp->audioformatsize);
						fp->flag |= FILE_HANDLE::FLAG_AUDIO;
					} else {
						AVIStreamRelease(pas);
					}
				} else {
					AVIStreamRelease(pas);
				}
			}
		}
	}

	return fp;
}

//---------------------------------------------------------------------
//	�t�@�C���N���[�Y
//---------------------------------------------------------------------
bool func_close(INPUT_HANDLE ih) {
	FILE_HANDLE* fp = (FILE_HANDLE*)ih;

	if (fp) {
		if (fp->flag & FILE_HANDLE::FLAG_AUDIO) {
			AVIStreamRelease(fp->paudio);
			GlobalFree(fp->audioformat);
		}
		if (fp->flag & FILE_HANDLE::FLAG_VIDEO) {
			AVIStreamRelease(fp->pvideo);
			GlobalFree(fp->videoformat);
		}
		AVIFileRelease(fp->pfile);
		GlobalFree(fp);
	}

	return true;
}

//---------------------------------------------------------------------
//	�t�@�C���̏��
//---------------------------------------------------------------------
bool func_info_get(INPUT_HANDLE ih, INPUT_INFO* iip) {
	FILE_HANDLE	*fp = (FILE_HANDLE *)ih;

	iip->flag = 0;
	if (fp->flag & FILE_HANDLE::FLAG_VIDEO) {
		iip->flag |= INPUT_INFO::FLAG_VIDEO;
		iip->rate = fp->videoinfo.dwRate;
		iip->scale = fp->videoinfo.dwScale;
		iip->n = fp->videoinfo.dwLength;
		iip->format = (BITMAPINFOHEADER*)fp->videoformat;
		iip->format_size = fp->videoformatsize;
	}

	if (fp->flag & FILE_HANDLE::FLAG_AUDIO) {
		iip->flag |= INPUT_INFO::FLAG_AUDIO;
		iip->audio_n = fp->audioinfo.dwLength;
		iip->audio_format = (WAVEFORMATEX*)fp->audioformat;
		iip->audio_format_size = fp->audioformatsize;
	}

	return true;
}

//---------------------------------------------------------------------
//	�摜�ǂݍ���
//---------------------------------------------------------------------
int func_read_video(INPUT_HANDLE ih, int frame, void* buf) {
	FILE_HANDLE* fp = (FILE_HANDLE*)ih;

	LONG videosize, size;
	if (AVIStreamRead(fp->pvideo, frame, 1, NULL, NULL, &videosize, NULL) != S_OK) return 0;
	if (AVIStreamRead(fp->pvideo, frame, 1, buf, videosize, &size, NULL) != S_OK) return 0;
	return size;
}

//---------------------------------------------------------------------
//	�����ǂݍ���
//---------------------------------------------------------------------
int func_read_audio(INPUT_HANDLE ih, int start, int length, void* buf) {
	FILE_HANDLE* fp = (FILE_HANDLE*)ih;
	LONG size;
	int samplesize;

	samplesize = ((WAVEFORMATEX*)fp->audioformat)->nBlockAlign;
	if (AVIStreamRead(fp->paudio, start, length, buf, samplesize * length, NULL, &size) != S_OK) return 0;
	return size;
}

//---------------------------------------------------------------------
//	�ݒ�_�C�A���O
//---------------------------------------------------------------------
bool func_config(HWND hwnd, HINSTANCE dll_hinst) {
	MessageBox(hwnd, L"�T���v���_�C�A���O", L"���͐ݒ�", MB_OK);

	// DLL���J������Ă��ݒ肪�c��悤�ɕۑ����Ă����Ă�������

	return true;
}
