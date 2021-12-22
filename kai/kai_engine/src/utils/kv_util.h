/*K
This file is part of Konantech AI Framework project
It is subject to the license terms in the LICENSE file found in the top-level directory of this distribution
*/
#pragma once

#ifdef XXX
#include"../session/kcommon.h"
#include"../include/kai_api.h"
#include"../math/karray.h"
#include"../math/kmath.h"

class KVUtil {
public:
	void save_value(FILE* fid, KaiValue value);
	KaiValue read_value(FILE* fid);

	void save_int(FILE* fid, KInt dat);
	void save_str(FILE* fid, KString dat);
	void save_float(FILE* fid, KFloat dat);

	void save_list(FILE* fid, KaiList value);
	void save_dict(FILE* fid, KaiDict value);

	void save_shape(FILE* fid, KaiShape shape);

	KInt read_int(FILE* fid);
	KString read_str(FILE* fid);
	KFloat read_float(FILE* fid);

	KaiList read_list(FILE* fid);
	KaiDict read_dict(FILE* fid);

	void read_shape(FILE* fid, KaiShape& shape);

	KaiArray<KInt> read_narray(FILE* fid);
	KaiArray<KFloat> read_farray(FILE* fid);

	template <class T> void save_array(FILE* fid, KaiArray<T> arr);
	template <class T> void get_array_data(KHArray hArray, KInt nStart, KInt nCount, T* pBuffer);

	KString to_decs_str(KaiValue value);


	KaiValue seek_dict(KaiDict dict, KString sKey, KaiValue kDefaultValue);		// dict�� sKey �׸� ���� ��� ���ڷ� ������ ����Ʈ �� ��ȯ
	KaiValue seek_set_dict(KaiDict& dict, KString sKey, KaiValue kDefaultValue);	// seek_dict()ó�� �����ϴ� ���ÿ� ����Ʈ ���� dict�� ����

	KString type_name(KInt n) { return "KInt"; }
	KString type_name(KFloat n) { return "KFloat"; }

	template <class T> KString arrDescription(KaiArray<T> arr);

	KaiDict& dict_ref(KaiValue value);

	KFloat mean(KaiList list);
	KFloat sum(KaiList list);

	KaiDict to_host(KaiDict dict, KaiMath* pMath);
	KaiList to_host(KaiList dict, KaiMath* pMath);

	KaiList to_list(KStrList rawHeader);

protected:
	KString m_encode_esc(KString str);
	
	KString m_listDescription(KaiList list);
	KString m_dictDescription(KaiDict dict);
	KString m_shapeDescription(KaiShape shape);

	KaiValue m_to_host(KaiValue value, KaiMath* pMath);

};

extern KVUtil kutil;
#endif