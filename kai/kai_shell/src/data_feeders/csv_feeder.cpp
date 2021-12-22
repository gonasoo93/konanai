/*K
This file is part of Konantech AI Framework project
It is subject to the license terms in the LICENSE file found in the top-level directory of this distribution
*/
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "csv_feeder.h"
#include "../../../kai_engine/src/include/kai_errors.h"
#include "../../../kai_engine/src/nightly/nightly_utils.h"

//hs.cho
#ifdef KAI2021_WINDOWS
#else
#define strcpy_s(a,b,c) !strncpy(a,c,b)
#define strtok_s strtok_r
#endif


using std::ifstream;
using std::string;
using std::vector;

int CsvFeeder::ms_checkCode = 25186118;

typedef struct CsvCache {
	int     m_checkCode;	
	int     m_version;
	int     m_cntData;
	int     m_vecSize;
	int     m_headerSize;
	KaiList m_header;	// A list of KString
	KFloat* m_parData;

	CsvCache() : m_checkCode(0), m_version(0), m_cntData(0), m_vecSize(0), m_headerSize(0), m_parData(NULL) { }

} CsvCache;

CsvFeeder::CsvFeeder() : DataFeeder() {
	m_version       = 1;
	m_cntData       = 0;
	m_vecSize       = 0;
	m_headerSize    = 0;
	m_parData       = NULL;
	m_propDict.clear();
}

CsvFeeder::~CsvFeeder() {
	// Reset properties
	if (m_parData) {
		delete[] m_parData;
		m_parData = NULL;
	}

	m_cntData       = 0;
	m_vecSize       = 0;
	m_headerSize    = 0;

	m_propDict.clear();
}

KaiValue CsvFeeder::m_getProperty(KString sKey, KaiValue def) {
	if (m_propDict.find(sKey) != m_propDict.end())
		return m_propDict[sKey];

	return def;
}

void CsvFeeder::m_setProperty(KString sKey, KaiValue val) {
	m_propDict[sKey] = val;
}

void CsvFeeder::loadData(KString sCsvFilename, KString sCachePath, KaiDict kwArgs, KHSession hSession, KHDataset hDataset) {
	// Set user-defined properties
	m_propDict.clear();
	m_propDict = kwArgs;

	KBool bLoadCache = m_getProperty("load_cache", false);
	KBool bSaveCache = m_getProperty("save_cache", true);

	// Make a directory for using cache
	if (bSaveCache)
		Utils::mkdir(sCachePath);

	// Get a base filename of the CSV file
	KString sBaseFilename = sCsvFilename.substr(sCsvFilename.find_last_of("/\\") + 1);

	// Remove an extension
	KString::size_type posDot = sBaseFilename.find_last_of('.');
	sBaseFilename = sBaseFilename.substr(0, posDot);

	// Generate a cache filename
	KString sCacheFilename = sCachePath + "/" + sBaseFilename + ".dat";

	if (!bLoadCache || !m_loadCache(sCacheFilename)) {
		m_createDataset(sCsvFilename);

		if (bSaveCache)
			m_saveCache(sCacheFilename);
	}

	// Update the properties to dataset
	if (hSession && hDataset)
		KAI_Component_set_property(hSession, hDataset, m_propDict);
	else
		printf("warning: %s(%u): KAI_Component_set_property() failed.\n", __FUNCTION__, __LINE__);
}

KaiList CsvFeeder::getOuputFieldNames() {
	KaiList targetNames;

	KaiList dataHeader = m_propDict["header"];
	KaiList srcColumns = m_propDict["output_columns"];

	for (auto& it : srcColumns) {
		KaiList range = it;

		KInt idxStart = range[0];
		KInt nCount = range[1];

		for (KInt n = 0; n < nCount; n++) targetNames.push_back(dataHeader[idxStart+n]);
	}

	return targetNames;
}

void CsvFeeder::m_createDataset(KString sCsvFilename) {
	printf("Loading csv file...\n");

	// If this variable is false, then thid method is failed.
	KBool bHeaderExist = m_propDict["header_exist"];

	KaiList rawHeader;
	KaiList dataHeader;
	KaiList* pHeader = bHeaderExist ? &rawHeader : NULL;

	// CSV ������ �а�, pHeader(==&rawHeader)�� attributes ����, rows�� ������ ���� �ε�
	//vector<vector<string>> rows = kutil.load_csv(KArgs::data_root + filepath, pHeader);
	KaiList rows = m_readCsvFile(sCsvFilename, pHeader);

	KInt nRows = rows.size();
	KInt nCols = ((KaiList)rows[0]).size();
	KInt nDataCols = nCols;

	// Check the validation
	if (nRows < 1) {
		printf("error: %s(%u): data not found.\n", __FUNCTION__, __LINE__);
		THROW(KERR_NO_DATASET_FOR_MODEL_EXEC);
	}

	KaiList to_onehot_list;

	// m_propDict["to_onehot"]�� �����ϴ� ���, KList Ÿ������ [0]=0, [1]=3�� ����Ǿ� ����
	// ([0]�� ��� column�� �ε���, [1]�� one-hot ���ͷ� ���� �� ������ ũ�⸦ ����)
	// one-hot vector�� ǥ���ϴ� ���, ù ��° 'Sex' �÷��� 'I(Infant)', 'M(Male)', 'F(Female)'�� �и��ؾ� �ϹǷ� col ���� +2
	// ��������� nDataCols ���� 9->11�� �����
	if (m_propDict.find("to_onehot") != m_propDict.end()) {
		to_onehot_list = (KaiList)m_propDict["to_onehot"];

		for (auto it = to_onehot_list.begin(); it != to_onehot_list.end(); it++) {
			KaiList to_onehot = *it;
			nDataCols += (KInt)to_onehot[1] - 1;
		}
	}

	KBool bInputNorm = m_getProperty("input_normalize", false);
	KInt temp_input_columns = m_getProperty("temp_input_columns", 0);

	if ((KBool)m_getProperty("input_normalize", false)) {
		//printf("input normalizetion here!!!");
		// �Ӽ� ����: ����ȭ ���� ���� �÷���, ����ȭ�� ���� ����� ǥ��������
		// �ο� ������ true�� �����Ѵ�. 
		// �� ���� ���� (!found) ���� ó�� ���� ��-��-�ڿ� ���� ���� �� ����� ���� ����ȭ �ݿ�
		// ����ȭ�� �̿�� �ʵ庰 ���, ǥ�������� �ð�ȭ �� ���� ����, predict �� ����ȭ�� ���� ���� ���� ����
		// ������ dataset - dataloader ����� �����ϰ� datafeeder ���Ҹ� �ϰ� �� �����̹Ƿ� datafeeder�� å���� �κ���
		// ���� �Է� ����ȭ �� ������ datafeeder�� å���� �κ��̹Ƿ� ���, ǥ�������� �������� �ʱ�� �Ѵ�. 
	}

	// Create a blank array
	m_parData = new KFloat[nRows * nDataCols];
	memset(m_parData, 0, sizeof(KFloat) * nRows * nDataCols);

	// one-hot vector ���� ���ο� ���� rawHeader�κ��� dataHeader�� ����� (������ ��� ���ų� �� Ŀ����, �������� ���� ����)
	// rows�� string �����͵��� float���� ��ȯ�Ͽ� csv_data�� ����
	for (KInt nc = 0, nd = 0; nc < nCols; nc++, nd++) {
		bool found = false;

		// one-hot ���ͷ� �����ϴ� �ɼ��� Ȱ��ȭ �Ǿ��ִ� ���
		for (auto& it : to_onehot_list) {
			// [0]=��� column �ε���, [1]=��ȯ�� one-hot ���� ũ�Ⱑ ����� ����Ʈ
			KaiList to_onehot = it;

			// ���� column �ε����� 0�϶� ('Sex' column)
			if (to_onehot[0] == nc) {
				// ��ȯ�� one-hot ���� ũ�� (3)
				KInt nvec = to_onehot[1];
				// �� KaiDict (map<KString, KValue>) ����
				KaiDict value_idx;

				// ��ü rows ����ŭ loop�Ͽ� 0��° 'column'�� � ������ ������� Ȯ��
				for (KInt nr = 0; nr < nRows; nr++) {
					// nr��° ���� �����͸� get
					KaiList row = (KaiList)(rows[nr]);
					// nc��° ���� �����͸� string���� get
					KString sData = (KString)row[nc];

					// �ش� ������(���ڿ�)�� KaiDict�� ������,
					if (value_idx.find(sData) == value_idx.end()) {
						// KaiDict ũ�⸦ �ε����� �Ͽ�, �ε����� nvec(3)���� ũ�ų� ������ Error
						KInt idx = value_idx.size();
						if (idx >= nvec)
							THROW(KERR_ONE_HOT_DATA_EXCEED_SIZE);

						// sData ��(I,M,F �� �ϳ�)�� idx�� ����
						value_idx[sData] = idx;

						// nc��° ���� attribute ���� string���� get
						KString sValue = rawHeader[nc];

						// dataHeader�� 'Sex/I', 'Sex/M', �Ǵ� 'Sex/F' ���� Push
						if (pHeader)
							dataHeader.push_back(sValue + "/" + sData);
					}

					// ���� ����(I,M,F ��)�� ó������ ������ ���� column idx(0~2)�� get
					// ***** �������� ������ ���� dataHeader �� csv_data�� �Է� ������ �ٲ� *****
					KInt idx = value_idx[sData];

					// nr�� nd+idx��(0~2��) ���� 1�� ����
					m_parData[nr*nDataCols + nd + idx] = 1.0f;
				}

				// one-hot vector ũ�⸸ŭ, nd�� ���� 0->2�� ���� (for������ +1 �ϹǷ�)
				nd += nvec - 1;
				found = true;
				break;	// one-hot vector�� �� ���� ����� ���� break
			}
		}

		// one-hot vector ������ �ش���� �ʰų�, �ش� column�� �ƴ� ���
		// (nd �� ������� �ʾ���)
		if (!found) {
			KFloat sum = 0, sqsum = 0;
			// ��ü ��(row)�� ���� loop�� ������
			for (KInt nr = 0; nr < nRows; nr++) {
				KaiList row = KaiList(rows[nr]);
				KFloat element = std::stof(row[nc]);
				m_parData[nr*nDataCols + nd] = element;
				if (bInputNorm) {
					sum += element;
					sqsum += element * element;
				}
			}
			if (bInputNorm && nd < temp_input_columns) {
				KFloat mean = sum / nRows;
				KFloat std = ::sqrt(sqsum / nRows - mean * mean);
				//means.set_at(nd) = mean;
				//stds.set_at(nd) = std;
				for (KInt nr = 0; nr < nRows; nr++) {
					KFloat element = m_parData[nr*nDataCols + nd];
					m_parData[nr*nDataCols + nd] = (element - mean) / std;
				}
			}

			// 'Sex' column�� �ƴ� ��� attribute�� string�� �״�� dataHeader�� input
			if (pHeader)
				dataHeader.push_back(rawHeader[nc]);
		}
	}

	// ������ ���� ���(rawHeader)�� one-hot vector ���� ���ο� ���� ��ȯ�� ���(dataHeader)��
	// m_propDict�� ����
	if (bHeaderExist) {
		// Original 9 attributes(string) :
		// {Sex, Length, Diameter, Height, Whole weight, Shucked weight, Viscera weight, Shell weight, Rings}
		m_propDict["raw_header"] = rawHeader;

		// Renewed 11 attributes(string) :
		// {'Sex/I(Infant)', 'Sex/M(Male)', 'Sex/F(Female)', Length, Diameter, ... , Shell weight, Rings}
		m_propDict["header"] = dataHeader;

		m_headerSize = (int)dataHeader.size();
		m_propDict["header_size"] = m_headerSize;
	}
	else
		m_headerSize = 0;

	//m_data = m_data.to_cuda();
	//KaiDict data = KaiDict{ {"default", csv_data.get_core()} };

	// ������ csv_data�� core(�ڵ�)�� rows/cols ������ ����
	m_propDict["total_count"] = m_cntData = nRows;
	m_propDict["vec_size"] = m_vecSize = nDataCols;
	//m_propDict["data"] = data;
}

KaiList CsvFeeder::m_readCsvFile(KString sCsvFilename, KaiList* pHead) {
#ifdef KAI2021_WINDOWS
	std::replace(sCsvFilename.begin(), sCsvFilename.end(), '/', '\\');
#endif

	ifstream infile(sCsvFilename);

	if (infile.fail()) {
		//hs.cho
		//THROW(KERR_FILE_OPEN_FAILURE, sCsvFilename);
		THROW(KERR_FILE_OPEN_FAILURE);
	}

	vector<vector<string>> rows;

	string line;
	char buffer[1024];
	char* context = NULL;

	getline(infile, line);
	if (pHead) {
		if (strcpy_s(buffer, 1024, line.c_str()))
			THROW(KERR_ASSERT);
		char* token = strtok_s(buffer, ",", &context);
		while (token) {
			(*pHead).push_back(token);
			token = strtok_s(NULL, ",", &context);
		}
	}

	while (std::getline(infile, line)) {
		if (line[0] == ',') {
			line = "0" + line;
		}
		if (line[line.length() - 1] == ',') {
			line = line + "0";;
		}

		std::size_t pos = line.find(",,");
		while (pos != std::string::npos) {
			line = line.substr(0, pos + 1) + "0" + line.substr(pos + 1);
			pos = line.find(",,");
		}

		if (strcpy_s(buffer, 1024, line.c_str()))
			THROW(KERR_ASSERT);
		char* token = strtok_s(buffer, ",", &context);
		vector<string> row;
		while (token) {
			row.push_back(token);
			token = strtok_s(NULL, ",", &context);
		}
		rows.push_back(row);
	}

	infile.close();

	// Conversion
	KaiList csv_data;
	for (vector<vector<string>>::iterator it=rows.begin(); it!=rows.end(); ++it) {
		KaiList row_data;
		for (vector<string>::iterator it_row=it->begin(); it_row!=it->end(); ++it_row) {
			row_data.push_back((KString)*it_row);
		}
		csv_data.push_back(row_data);
	}

	return csv_data;
}

bool CsvFeeder::m_loadCache(KString sCacheFilename) {
	FILE* fid = Utils::fopen(sCacheFilename, "rb", false);

	if (fid == NULL)
		return false;

	printf("Loading cache data...\n");

	CsvCache cache;

	if (fread(&cache.m_checkCode, sizeof(int), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_READ);
	if (fread(&cache.m_version, sizeof(int), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_READ);
	if (fread(&cache.m_cntData, sizeof(int), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_READ);
	if (fread(&cache.m_vecSize, sizeof(int), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_READ);
	if (fread(&cache.m_headerSize, sizeof(int), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_READ);

	if (cache.m_checkCode != ms_checkCode) { fclose(fid); return false; }
	if (cache.m_version != m_version) { fclose(fid); return false; }

	for (int i=0; i<cache.m_headerSize; ++i) {
		size_t strlen = 0;
		if (fread(&strlen, sizeof(size_t), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_READ);

		char szFieldName[256] = {0, };

		if (strlen > sizeof(szFieldName) / sizeof(char))
			THROW(KERR_FAILURE_ON_FILE_READ);

		if (fread(szFieldName, sizeof(char), strlen, fid) != strlen) THROW(KERR_FAILURE_ON_FILE_READ);

		cache.m_header.push_back( KString(szFieldName) );
	}

	int payload_size = cache.m_cntData * cache.m_vecSize;

	cache.m_parData = new KFloat[payload_size];
	std::memset(cache.m_parData, 0, sizeof(KFloat) * payload_size);

	if (fread(cache.m_parData, sizeof(KFloat), payload_size, fid) != payload_size) THROW(KERR_FAILURE_ON_FILE_READ);

	// Copy the cache data to this class
	m_propDict["total_count"] = m_cntData = cache.m_cntData;
	m_propDict["vec_size"] = m_vecSize = cache.m_vecSize;
	m_propDict["header_size"] = m_headerSize = cache.m_headerSize;
	if (m_headerSize > 0)
		m_propDict["header"] = cache.m_header;

	if (m_parData) {
		delete [] m_parData;
		m_parData = NULL;
	}

	m_parData = cache.m_parData;

	return true;
}

void CsvFeeder::m_saveCache(KString sCacheFilename) {
	printf("Saving cache data...\n");

	FILE* fid = Utils::fopen(sCacheFilename, "wb");

	if (fid == NULL)
		THROW(KERR_FAILURE_ON_FILE_SAVE);

	// Write headers
	if (fwrite(&ms_checkCode, sizeof(int), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_SAVE);
	if (fwrite(&m_version, sizeof(int), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_SAVE);
	if (fwrite(&m_cntData, sizeof(int), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_SAVE);
	if (fwrite(&m_vecSize, sizeof(int), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_SAVE);
	if (fwrite(&m_headerSize, sizeof(int), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_SAVE);

	KaiList header = m_propDict["header"];
	for (int i=0; i<m_headerSize; ++i) {
		KString sFieldName = (KString)header[i];
		size_t  strlen     = sFieldName.length();

		if (fwrite(&strlen, sizeof(size_t), 1, fid) != 1) THROW(KERR_FAILURE_ON_FILE_SAVE);
		if (fwrite(sFieldName.c_str(), sizeof(char), strlen, fid) != strlen) THROW(KERR_FAILURE_ON_FILE_SAVE);
	}

	int payload_size = m_cntData * m_vecSize;
	if (fwrite(m_parData, sizeof(KFloat), payload_size, fid) != payload_size) THROW(KERR_FAILURE_ON_FILE_SAVE);

	fclose(fid);
}

KBool CsvFeeder::m_getDataCount(void* pAux, KInt* pnDataCount) {
	if (!pnDataCount)
		return false;

	*pnDataCount = m_cntData;
	return true;
}

KBool CsvFeeder::m_getFieldSpec(void* pAux, KBool bInput, KString sFieldName, KCbFieldInfo* pFieldInfo, KCbFieldInfo* pMatchInfo) {
	////////////////////////////////////////////////////////////////

	// User-defined code (To set shapes of feeding data)
	
	// feed_shape = is_seq ? { data_step, data_shape } : { data_shape }

	KaiList srcColumns      = m_propDict[bInput ? "input_columns" : "output_columns"];
	KInt    nSrcVecSize     = m_propDict["vec_size"];
	KInt    nTargetDataSize = 0;

	for (auto& it: srcColumns) {
		KaiList range = it;
		if (range.size() != 2)
			THROW(KERR_BAD_INPUT_COLUMN_SPECIFICATION);
		KInt idxStart = range[0];
		KInt nCount   = range[1];
		if (idxStart < 0 || nCount <= 0 || idxStart + nCount > nSrcVecSize)
			THROW(KERR_BAD_INPUT_COLUMN_SPECIFICATION);

		nTargetDataSize += nCount;
	}

	if (nTargetDataSize > nSrcVecSize)
		THROW(KERR_BAD_INPUT_COLUMN_SPECIFICATION);

	KBool    is_float_type = true;
	KBool    is_seq        = false;
	KInt     data_step     = 1;
	KaiShape data_shape    = KaiShape{ nTargetDataSize };

	////////////////////////////////////////////////////////////////

	// For interworking with callbacks (do not modify)
	pFieldInfo->m_bIsFloat   = is_float_type;
	pFieldInfo->m_shape      = data_shape;
	pFieldInfo->m_bIsSeq     = is_seq;
	pFieldInfo->m_nTimesteps = data_step;

	////////////////////////////////////////////////////////////////

	return true;
}

KBool CsvFeeder::m_feedIntData(void* pAux, KBool bInput, KString sFieldName, KaiShape shape, KIntList nDatIndexs, KInt* pnBuffer) {
	THROW(KERR_INTERNAL_LOGIC_ERROR);
	return false;
}

KBool CsvFeeder::m_feedFloatData(void* pAux, KBool bInput, KString sFieldName, KaiShape shape, KIntList nDatIndexs, KFloat* pfBuffer) {
	/*
	* Description :
	*  - shape :
	*      the array size of one data
	*      (e.g. If it is Abalone data, then the shape value is <10> or <1>)
	*  - nDatIndexs :
	*      A list of batch indexes
	*      (e.g. If nDatIndexs is {0,1,2}, then this method returns <3,10>or<3,1>-sized array)
	*/

	// Get the sizes of vectors
	KInt nSrcVecSize = m_propDict["vec_size"];
	KInt nDstVecSize = shape.total_size();
	
	// Get the range of the source data
	KaiList srcColumns = m_propDict[bInput ? "input_columns" : "output_columns"];

	for (KInt idxDstData=0; idxDstData<(KInt)nDatIndexs.size(); ++idxDstData) {
		KInt idxSrcData = nDatIndexs[idxDstData];

		KInt nCntCopy = 0;

		for (auto& it: srcColumns) {
			KaiList range = it;
			if (range.size() != 2)
				THROW(KERR_BAD_INPUT_COLUMN_SPECIFICATION);
			KInt idxStart = range[0];
			KInt nCount   = range[1];
			if (idxStart < 0 || nCount <= 0 || idxStart + nCount > nSrcVecSize)
				THROW(KERR_BAD_INPUT_COLUMN_SPECIFICATION);

			memcpy(
				pfBuffer + idxDstData*nDstVecSize + nCntCopy,
				m_parData + idxSrcData*nSrcVecSize + idxStart,
				sizeof(KFloat)*nCount);

			nCntCopy += nCount;
		}

		// Check the validation
		if (nCntCopy != nDstVecSize)
			THROW(KERR_BAD_INPUT_COLUMN_SPECIFICATION);
	}

	return true;
}
