/*
 *  
 *  $Id: dicomfindassociation.cpp $
 *  DeepWise Dicom Viewer
 *
 *  Copyright 2017-2030 DeepWise All rights reserved.
 *  http://www.deepwise.com
 *
 *  This file is licensed under LGPL v3 license.
 *  See License.txt for details
 *
 *  Code adapted from Aeskulap
 *
 */
#include <api/controllers/icontroladorlog.h>
//#include <api/controllers/icontroladorpermisos.h>
#include "dicomfindassociation.h"
//#include <wx/string.h>
//#include <wx/intl.h>
//#include <api/internationalization/internationalization.h>
#include <main/controllers/dicommanager.h>
//#include <main/controllers/dcmtk/dicomservers.h>
#include <api/dicom/dicomdataset.h>
#include "api/autoptr.h"

#ifdef verify
#define MACRO_QUE_ESTORBA verify
#undef verify
#endif

#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#ifdef MACRO_QUE_ESTORBA
#define verify MACRO_QUE_ESTORBA
#endif

#define STATUS_FIND_CANCEL_SHOWTHEFIRST500 (STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest + 1)

FindAssociation::FindAssociation(const std::string& ambitolog) : Association(ambitolog), pResultsWrapper(NULL) {
	SetFindRootQueryRetrieveInformationModel(FRQRIM_Study);
	//by jia
	//GNC::GCS::Permisos::EstadoPermiso estado = GNC::GCS::IControladorPermisos::Instance()->Get("core.pacs.limits", "find_rsp_max_results");
	//if (estado) {
	//	maxResults = estado.ObtenerValor<int>();
	//}
	//else {
	//	maxResults = -1;
	//}
	maxResults  = -1;
	bPushResults = true;
}

FindAssociation::~FindAssociation() {
	DeleteResultStack();
}

CONDITION FindAssociation::findSCU(T_ASC_Association *assoc, DcmDataset *query) {
	CONDITION cond;
	DIC_US msgId = assoc->nextMsgID++;
	T_ASC_PresentationContextID presId;
	T_DIMSE_C_FindRQ req;
	T_DIMSE_C_FindRSP rsp;
	DcmDataset *statusDetail = NULL;
	std::string errorMessage;
	
	if (query == NULL) {
		return makeOFCondition(OFM_dcmnet, DIMSEC_BADDATA, OF_error, "NULL DIMSE Query");
	}
	
	DeleteResultStack();
	
	/* which presentation context should be used */
	presId = ASC_findAcceptedPresentationContextID(assoc, m_abstractSyntax.c_str());
	if (presId == 0) {
		LOG_ERROR(ambitolog, "Invalid PresentationContextId");
		return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
	}
	
	req.MessageID = msgId;
	req.DataSetType = DIMSE_DATASET_PRESENT;
	req.Priority = DIMSE_PRIORITY_LOW;
	strcpy(req.AffectedSOPClassUID, m_abstractSyntax.c_str());
	
	FindCallbackInfo callbackData;
	callbackData.pCaller = this;
	callbackData.assoc = assoc;
	
	cond = DIMSE_findUser(assoc, presId, &req, query,
				   findCallback, (void*) &callbackData,
				   (m_timeout == 0) ? DIMSE_BLOCKING : DIMSE_NONBLOCKING,
				   m_timeout,
				   &rsp, &statusDetail);

	if(rsp.DimseStatus != STATUS_Success && rsp.DimseStatus != STATUS_Pending) {
		std::ostringstream os;
		os << DU_cfindStatusString(rsp.DimseStatus);
		if (statusDetail != NULL) {
			OFString errorComment;
			if (statusDetail->findAndGetOFString(DCM_ErrorComment, errorComment).good()) {
				os << ". " << errorComment.c_str();
			}
		}
		errorMessage = os.str();
	}
	
	if (cond == DIMSE_NORMAL) {
		if (rsp.DimseStatus == STATUS_Success) {
		}
	}

    if (rsp.DimseStatus == STATUS_FIND_CANCEL_SHOWTHEFIRST500)
    {
        //errorMessage = _Std("show the first 500");
		errorMessage = "show the first 500";

    }
	
	if (statusDetail != NULL) {
		LOG_DEBUG(ambitolog, "DIMSE_findUser(): Status: " << std::endl << DumpDataset(statusDetail));
		delete statusDetail;
	}

	if (rsp.DimseStatus != STATUS_Success && rsp.DimseStatus != STATUS_Pending) {
		return makeOFCondition(OFM_dcmnet, 18, OF_error, errorMessage.c_str());
	}

	return cond;
}

void FindAssociation::findCallback(void* callbackData, T_DIMSE_C_FindRQ* /*rq*/, int responseCount, T_DIMSE_C_FindRSP* rsp, DcmDataset *responseIdentifiers) {
	FindCallbackInfo* pCallback = (FindCallbackInfo*) callbackData;
	
	int max_results = pCallback->pCaller->GetMaxResults();
	if (pCallback->pCaller->GetMaxResults() > 0 && responseCount > pCallback->pCaller->GetMaxResults()) {
		LOG_DEBUG(pCallback->pCaller->ambitolog, "findCallback(): Ignoring response num " << responseCount << ". The maximum number of responses was " << pCallback->pCaller->GetMaxResults());
		// by jia
		//rsp->DimseStatus = STATUS_FIND_CANCEL_SHOWTHEFIRST500;
		//pCallback->pCaller->Stop();
		//ASC_releaseAssociation(pCallback->assoc);
		//return;
	}
	// by jia
	//wxString mess = wxString::Format(_("%d results has been found"), (int)(responseCount) );
	//std::string mensaje(mess.ToUTF8());
	//
	//LOG_DEBUG(pCallback->pCaller->ambitolog, "Processing response num " << responseCount);
	//if(!pCallback->pCaller->NotificarProgreso(0.0f, mensaje))
	//{
	//	rsp->DimseStatus = STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest;
	//	pCallback->pCaller->Stop();
	//	ASC_releaseAssociation(pCallback->assoc);
	//	LOG_INFO(pCallback->pCaller->ambitolog, "Operation canceled by user");
	//	return;
	//} else 	{
		DcmDataset* response = new DcmDataset(*responseIdentifiers);
		
		if (pCallback->pCaller->bPushResults) {
			pCallback->pCaller->result.push(response);
			//push into Ginkgo data...
			if (pCallback->pCaller->pResultsWrapper != NULL ) {//by jia ) && pCallback->pCaller->pDicomServer.IsValid()) {
				GIL::DICOM::DICOMManager mgr( response , "") ;//by jia, pCallback->pCaller->pDicomServer->GetDefaultCharset() );
				GNC::GCS::Ptr<GIL::DICOM::DicomDataset> base = new GIL::DICOM::DicomDataset();	

				mgr.CargarJerarquia((*base), DCM_MaxReadLength, NULL, false);
				pCallback->pCaller->pResultsWrapper->push_back(base);
			}
		}
		else {
		    LOG_DEBUG("C-FIND", "No results found");
		}
		pCallback->pCaller->OnResponseReceived(response);
	//}
}

CONDITION FindAssociation::SendObject(DcmDataset *dataset) {
	return findSCU(assoc, dataset);
}

void FindAssociation::OnResponseReceived(DcmDataset* response) {
	LOG_DEBUG(ambitolog, "Response received: " << std::endl << DumpDataset(response));
}

DcmStack* FindAssociation::GetResultStack() {
	return &result;
}

DcmStack FindAssociation::CopyResultStack() {
	DcmStack copy;
	
	for (unsigned int i = 0; i < result.card(); i++) {
		DcmDataset* dset = new DcmDataset( *(static_cast<DcmDataset*>(result.elem(i))) );
		copy.push(dset);
	}
	
	return copy;
}

void FindAssociation::DeleteResultStack() {
	DcmDataset* obj = NULL;
	unsigned int size = result.card();
	
	for (Uint32 i = 0; i < size; i++) {
		obj = static_cast<DcmDataset*>(result.elem(i));
		delete obj;
	}
	
	result.clear();
}

void FindAssociation::SetMaxResults(int max) {
	maxResults = max;
}

//void FindAssociation::SetCallbackInfo(std::list< GNC::GCS::Ptr<GIL::DICOM::DicomDataset> >* pRwrapper, const GNC::GCS::Ptr<DicomServer>& dicomServer)
void FindAssociation::SetCallbackInfo(std::list< GNC::GCS::Ptr<GIL::DICOM::DicomDataset> >* pRwrapper)//, const DicomServer& dicomServer)
{
	pResultsWrapper = pRwrapper;
	//pDicomServer = dicomServer;
}

int FindAssociation::GetMaxResults() {
	return maxResults;
}

void FindAssociation::OnAddPresentationContext(T_ASC_Parameters* params) {
	CONDITION cond = ASC_addPresentationContext(params, 1, m_abstractSyntax.c_str(), AllTransferSyntaxes, 3);
	if (cond.bad()) {
		LOG_ERROR(ambitolog, "Unable to add default presentation context");
	}
}
