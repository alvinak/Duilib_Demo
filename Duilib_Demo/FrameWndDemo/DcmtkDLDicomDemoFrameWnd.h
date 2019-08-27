#pragma once
#include <UIlib.h>
using namespace DuiLib;

#include "api/dicom/dicomdataset.h"

#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdicent.h>
#include <dcmtk/dcmdata/dcdatset.h>

#include "main/controllers/dcmtk/dicomservers.h"
#include "main/controllers/pacscontroller.h"

#include "extended_control/list_pro_ui.h"

//////////////////////////////////////////////////////////////////////////
// ����ʹ��xml�ļ���������
//////////////////////////////////////////////////////////////////////////

#define DOWNLOAD_STATUS_HAS_FOUND		"found"
#define DOWNLOAD_STATUS_NOT_FOUND		"not found"
#define DOWNLOAD_STATUS_DOWNLOADING		"downloading"
#define DOWNLOAD_STATUS_STOPPED			"stopped"
#define DOWNLOAD_STATUS_SUCCESS			"success"
#define DOWNLOAD_STATUS_FAILURE			"failure"



typedef struct Series_Info
{
	std::string series_id;
	std::string modality;   // ����C-GET�������dicom�ļ�ʱ����Ҫ��Modality����
	//bool is_downloaded;
	std::string download_status;
};
typedef struct PatientInfo
{
	std::string patiend_id;
	std::string study_id;
	//std::vector<std::string> sereis_ids;
	std::vector<Series_Info> sereis_infos;
};

#define WM_USER_UPDATE_DOWNLOAD_DICOM_FILE   (WM_USER + 101)
#define WM_USER_UPDATE_DOWNLOAD_STATUS		 (WM_USER + 102)

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
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//void thread_func(DcmtkDLDicomDemoFrameWnd* ptr);
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

	void UpdateDownloadStaticsText(int index);
	void UpdateDownloadListProAll();
	void UpdateDownloadListProItem(int index, std::string status_text);
	void OutputResultStaticsToFile(std::string path);

	bool CheckedMatchConditions(GIL::DICOM::DicomDataset& data);
	DcmElement* CrearElementoConValor(const char* s);
	std::vector<std::string> testSplit(std::string srcStr, const std::string& delim);
	void SetDownloadStop(bool is_stopped);

private:
	std::vector<std::string> m_study_ids;
	std::vector<std::string> m_patient_ids; // ����edit�ؼ�������ʾ��patientid
	
	CEditUI* m_pPatientCsvPathEdit;
	CEditUI* m_pPatientIdEdit;
	CEditUI* m_pBodyPartEdit;
	CEditUI* m_pThicknessEdit;
	CEditUI* m_pMOdalityiesInStudyEdit;
	CEditUI* m_pSexEdit;
	CEditUI* m_pDownloadPathEdit;

	CLabelUI* m_pStatiscResultLabel;

	COptionUI* m_pFilterBodyPart;
	COptionUI* m_pFilterThickness;
	COptionUI* m_pFilterModality;
	COptionUI* m_pFilterSex;

	COptionUI* m_pFilterRANGE;
	CDateTimeUI* m_pFilterFROM;
	CDateTimeUI* m_pFilterTO;

	ListPro* m_listPro;
	std::vector<struct PatientInfo>  m_patient_infos1;  // ���Ը���patienid�ҵ�series������
	std::vector<std::string>  m_patient_infos2;  // ���ܸ���patienid�ҵ�series������

	std::string m_dicom_saved_path;
	std::string m_filter_thickness;
	std::string m_bodyPartExamined;
	std::string m_filter_modality;
	int m_downloading_dicom_index;
	bool m_is_stoped;
};

