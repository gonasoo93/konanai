/*K
This file is part of Konantech AI Framework project
It is subject to the license terms in the LICENSE file found in the top-level directory of this distribution
*/
#pragma once

#include "../core/common.h"

struct DicTermInfo {
	int m_nth;
	int m_freq;
	int m_voc_id;
	string m_tag;
	string m_lemma;
};

class NewsReformer {
public:
	NewsReformer();
	virtual ~NewsReformer();

	void exec_all();
	void exec_collect_words();
	void exec_create_dict(bool need_load=true, string fname="");
	void exec_replace_words(bool need_load = true);

protected:
	void m_proc_collect_words(FILE* fin, FILE* fout);
	void m_replace_words(FILE* fin, FILE* fout, int& art_cnt, int& sent_cnt, int& word_cnt);

	/*
	static inline void ms_ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
			}));
	}

	// trim from end (in place)
	static inline void ms_rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void ms_trim(std::string& s) {
		ms_ltrim(s);
		ms_rtrim(s);
	}
	*/

protected:
	int m_seq_wid;			// Phase1 ���� �ܾ ���� ������ �ο��� �Ϸ� ��ȣ
	int* m_pFreqDist;		// ���� �׸� ������ ���� ���� �󵵺� �ܾ��
	int m_nMaxFreqCnt;		// m_pFreqDist �迭�� ũ��, �� �ѵ� �ʰ� �ϰ� ���� ��ġ
	int m_nDictSize;		// ������ ������ �ִ� ��Ʈ���� (�ܿ��׸� ��ġ�� ��Ʈ���� ����)
	int m_nFreqThreshold;	// �� �� ������ �׸��� �ܿ��׸� ��ġ�� ��Ʈ���� ��ȯ
	vector<string> m_fnames;
	map<string, DicTermInfo*> m_dic;
	map<int, int> m_index;

	Dict m_dic_map;

	time_t m_time1, m_time2, m_time3;
};