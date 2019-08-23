/*
 *  
 *  $Id: dicomgetassociation.h $
 *  DeepWise Dicom Viewer
 *
 *  Copyright 2017-2030 DeepWise All rights reserved.
 *  http://www.deepwise.com
 *
 *  This file is licensed under LGPL v3 license.
 *  See License.txt for details
 *
 */
#pragma once

#include <set>
#include <ctime>
#include "dicomfindassociation.h"
//#include "istorecallback.h"
//#include "api/dicom/idicomconformance.h"

class GetAssociation : public FindAssociation {
public:
	typedef std::list<std::string> SOPClassList;

	std::string m_errorMessage;
    GetAssociation(const std::string& ambitolog);//, IModeloDicom* pModelo=NULL);
    virtual ~GetAssociation();
	
	 void SetWellKnownNumResults(unsigned int numResults);
	 
	 //void SetModelo(IModeloDicom* pModelo);

	 //void SetStorageSOPClasses(const GIL::DICOM::SOPClassList& SOPClasses);
	 void SetStorageSOPClasses(std::string modality);//const SOPClassList& SOPClasses);


	 void Create(const std::string& title, const std::string& peer, int port, const std::string& ouraet, /*int ourPort,*/ const char *abstractSyntax = UID_GETStudyRootQueryRetrieveInformationModel);
	
    CONDITION SendObject(DcmDataset *dataset);
	
//protected:
	
    virtual void OnAddPresentationContext(T_ASC_Parameters *params);
	
	 //virtual void OnFileReceived(const std::string& fileName, DcmDataset* dataSet);
	
    int m_maxReceivePDULength;

	 unsigned int m_wellKnownNumResults;
	
//private:
	
    typedef struct _GetCallbackInfo {
		T_ASC_Association *assoc;
		T_ASC_PresentationContextID presId;
		GetAssociation* pCaller;
    } GetCallbackInfo;
	
    typedef struct _StoreCallbackInfo {
		GetAssociation* pCaller;
		T_ASC_Association* assoc;
		std::clock_t lastTick;
    } StoreCallbackInfo;
	
    CONDITION getSCU(DcmDataset *pdset);
    
	 static void getCallback(void *callbackData, T_DIMSE_C_GetRQ *request, int responseCount, T_DIMSE_C_GetRSP *response);
	
    CONDITION acceptSubAssoc(T_ASC_Network *aNet, T_ASC_Association **assoc);
    CONDITION subOpSCP(T_ASC_Association **subAssoc);
    static void subOpCallback(void * pCaller, T_ASC_Network *aNet, T_ASC_Association **subAssoc);
	
    CONDITION storeSCP(T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID presID);
    static void storeSCPCallback(void *callbackData, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *req, char *imageFileName, DcmDataset **imageDataSet, T_DIMSE_C_StoreRSP *rsp, DcmDataset **statusDetail);
	
    CONDITION echoSCP(T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID presID);

	
    //IModeloDicom* m_pModelo;
public:
	 int m_numeroImagenes;
	 std::string m_mensaje;
	 long m_bytesDescargados;
protected:
	typedef struct TMedida{
		time_t m_instante;
		int bytesDescargados;
		float oldTasa;
		TMedida() {
			m_instante = time(NULL);
			bytesDescargados = 0;
			oldTasa = 0.0f;
		}
	} TMedida;
	TMedida m_medida;

    std::string m_pathParam;

	//GIL::DICOM::SOPClassList SOPClasses;
	SOPClassList SOPClasses;

public:
	float TasaTransferencia(int bytesRestantes);
	void ResetearMedida(bool clearTasa = false);	 
    void SetPath(std::string path);
};
