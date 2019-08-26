#pragma once
#include <UIlib.h>
using namespace DuiLib;

#include "api/dicom/dicomdataset.h"

#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdicent.h>
#include <dcmtk/dcmdata/dcdatset.h>

#include "main/controllers/dcmtk/dicomservers.h"

#include "main/controllers/pacscontroller.h"

//////////////////////////////////////////////////////////////////////////
// ����ʹ��xml�ļ���������
//////////////////////////////////////////////////////////////////////////

typedef struct Series_Info
{
	std::string series_id;
	std::string modality;   // ����C-GET�������dicom�ļ�ʱ����Ҫ��Modality����
	bool is_downloaded;
};
typedef struct PatientInfo
{
	std::string patiend_id;
	std::string study_id;
	//std::vector<std::string> sereis_ids;
	std::vector<Series_Info> sereis_infos;
};

class DcmtkDLDicomDemoFrameWnd : public WindowImplBase
{
public:
	DcmtkDLDicomDemoFrameWnd(void);
	~DcmtkDLDicomDemoFrameWnd(void);

	virtual LPCTSTR GetWindowClassName() const ;
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void    Notify(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnSelChanged(CControlUI* pSender);
	void OnOpenPatientIDListFile();
	void OnOpenDownloadPath();

	void DoSearchStudyTest();
	void DoSearchSeriesTest();

	void DoDownloadTest();	
	void DoDownloadTest2();	

	int GetPatienCount();
	int GetStudyCount();
	int GetSeriesCount();

	void UpdateDownloadStaticsText();
	void OutputResultStaticsToFile(std::string path);

	bool CheckedMatchConditions(GIL::DICOM::DicomDataset& data);
	DcmElement* CrearElementoConValor(const char* s);
	std::vector<std::string> testSplit(std::string srcStr, const std::string& delim);

private:
	std::vector<std::string> m_study_ids;
	std::vector<std::string> m_patient_ids; // ����edit�ؼ�������ʾ��patientid
	
	CEditUI* m_pPatientCsvPathEdit;
	CEditUI* m_pPatientIdEdit;
	CEditUI* m_pBodyPartEdit;
	CEditUI* m_pThicknessEdit;
	CEditUI* m_pMOdalityiesInStudyEdit;
	CEditUI* m_pDownloadPathEdit;

	CLabelUI* m_pResearchResultLabel;
	CLabelUI* m_pStatiscResultLabel;

	std::vector<struct PatientInfo>  m_patient_infos1;  // ���Ը���patienid�ҵ�series������
	std::vector<std::string>  m_patient_infos2;  // ���ܸ���patienid�ҵ�series������

	std::string m_dicom_saved_path;
	std::string m_filter_thickness;
	std::string m_bodyPartExamined;
	std::string m_filter_modality;
	int m_downloading_dicom_index;
};

