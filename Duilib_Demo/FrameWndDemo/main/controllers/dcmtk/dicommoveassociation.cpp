/*
 *  
 *  $Id: dicommoveassociation.cpp $
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
#define LOGGER "C-MOVE"

#include "api/dicom/idicomconformance.h"

//#include <api/controllers/icontroladorlog.h>
#include <main/controllers/controladorlog.h>
//#include <api/dicom/imodelodicom.h>
//#include <main/controllers/controladorpermisos.h>
//#include <api/controllers/ipacscontroller.h>
//#include <main/entorno.h>

//#include <api/internationalization/internationalization.h>

//#include "istorecallback.h"

#include "dicomnetwork.h"
#include "dicommoveassociation.h"

#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/dcmjpeg/djencode.h>
#include <dcmtk/dcmjpeg/djrplol.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>

//#include <wx/string.h>
//#include <wx/intl.h>

//#include "tls/tls.h"
//#include "tls/gtlslayer.h"
#include <dcmtk/dcmtls/tlstrans.h>

MoveAssociation::MoveAssociation(const std::string& _ambitolog/*, IModeloDicom* pModelo*/) 
		: FindAssociation(_ambitolog)
		, m_maxReceivePDULength(ASC_DEFAULTMAXPDU)
		//m_pModelo(pModelo),
		, m_errorMessage("")
{
	m_abstractSyntax = UID_MOVEStudyRootQueryRetrieveInformationModel;

}

MoveAssociation::~MoveAssociation() { }

void MoveAssociation::Create(const std::string& title, const std::string& peer, int port, const std::string& ouraet, /*int ourPort,*/ const char *abstractSyntax) {
	Association::Create(title, peer, port, ouraet, abstractSyntax);
}

//void MoveAssociation::SetModelo(IModeloDicom* pModelo) {
//	m_pModelo = pModelo;
//}

CONDITION MoveAssociation::SendObject(DcmDataset *dataset) {
	return moveSCU(dataset);
}

void MoveAssociation::OnAddPresentationContext(T_ASC_Parameters *params) {
	addAllStoragePresentationContexts(params, true, true);
}

typedef struct _MoveStoreCallbackInfo {
	MoveAssociation* pCaller;
	T_ASC_Association* assoc;
	std::clock_t lastTick;
} MoveStoreCallbackInfo;

CONDITION MoveAssociation::storeSCP(T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID presID) 
{
	CONDITION cond;
	T_DIMSE_C_StoreRQ* req;

	req = &msg->msg.CStoreRQ;

	MoveStoreCallbackInfo callbackData;
	callbackData.pCaller = this;
	callbackData.assoc = assoc;
	callbackData.lastTick = std::clock();

	std::stringstream ss;
	ss << m_pathParam;
	ss << "//";
	ss << m_numeroImagenes;
	m_numeroImagenes++;
	std::string fileName = ss.str();//GNC::Entorno::Instance()->CreateGinkgoTempFile(m_pathParam);

	cond = DIMSE_storeProvider(assoc, presID, req, fileName.c_str(), 1,
		NULL, storeSCPCallback, (void*) & callbackData,
		DIMSE_BLOCKING, 0);

	if (cond.good()) {
		//we are going to read received file dataset
		DcmFileFormat ff;
		ff.loadFile(fileName.c_str(), EXS_Unknown, EGL_noChange, DCM_TagInfoLength);
		DcmDataset* imageDataSet = ff.getDataset();

		if ((imageDataSet)) {
			// do not duplicate the dataset, let the user do this
			// if he wants to
			//OnFileReceived(fileName, imageDataSet);
		}
	}

	return cond;
}

OFCondition	MoveAssociation::subOpSCP(T_ASC_Association **subAssoc)
{
	T_DIMSE_Message     msg;
	T_ASC_PresentationContextID presID;

	if (!ASC_dataWaiting(*subAssoc, 0)) /* just in case */
		return DIMSE_NODATAAVAILABLE;

	OFCondition cond = DIMSE_receiveCommand(*subAssoc, DIMSE_BLOCKING, 0, &presID,
		&msg, NULL);

	if (cond == EC_Normal) {
		switch (msg.CommandField)
		{
		case DIMSE_C_STORE_RQ:
			cond = storeSCP(*subAssoc, &msg, presID);
			break;
		case DIMSE_C_ECHO_RQ:
			cond = echoSCP(*subAssoc, &msg, presID);
			break;
		default:
			cond = DIMSE_BADCOMMANDTYPE;
			LOG_ERROR("movescuLogger", "cannot handle command: 0x"
				<< STD_NAMESPACE hex << OFstatic_cast(unsigned, msg.CommandField));
			break;
		}
	}
	/* clean up on association termination */
	if (cond == DUL_PEERREQUESTEDRELEASE)
	{
		cond = ASC_acknowledgeRelease(*subAssoc);
		ASC_dropSCPAssociation(*subAssoc);
		ASC_destroyAssociation(subAssoc);
		return cond;
	}
	else if (cond == DUL_PEERABORTEDASSOCIATION)
	{
	}
	else if (cond != EC_Normal)
	{
		OFString temp_str;
		LOG_ERROR("movescuLogger", "DIMSE failure (aborting sub-association): " << DimseCondition::dump(temp_str, cond));
		/* some kind of error so abort the association */
		cond = ASC_abortAssociation(*subAssoc);
	}

	if (cond != EC_Normal)
	{
		ASC_dropAssociation(*subAssoc);
		ASC_destroyAssociation(subAssoc);
	}
	return cond;
}

void MoveAssociation::subOpCallback(void * pCaller,
	T_ASC_Network *aNet, T_ASC_Association **subAssoc)
{
	MoveAssociation* caller = static_cast<MoveAssociation*>(pCaller);
	
	if (aNet == NULL) return;   /* help no net ! */

	if (*subAssoc == NULL) {
		/* negotiate association */
		caller->acceptSubAssoc(aNet, subAssoc);
	} else {
		/* be a service class provider */
		caller->subOpSCP(subAssoc);
	}

	//static int a = 0;
	//a++;
	////printf("haha%d\n", a);
	//OutputDebugString(L"haha----------------------\n");
}

static int
	selectReadable_MY(
	T_ASC_Association *assoc,
	T_ASC_Network *net,
	T_ASC_Association *subAssoc,
	T_DIMSE_BlockingMode blockMode, int timeout)
{
	T_ASC_Association *assocList[2];
	int assocCount = 0;

	if (net != NULL && subAssoc == NULL) {
		if (ASC_associationWaiting(net, 0)) {
			/* association request waiting on network */
			return 2;
		}
	}
	assocList[0] = assoc;
	assocCount = 1;
	assocList[1] = subAssoc;
	if (subAssoc != NULL) assocCount++;
	if (subAssoc == NULL) {
		timeout = 1;            /* poll wait until an assoc req or move rsp */
	} else {
		if (blockMode == DIMSE_BLOCKING) {
			timeout = 10000;    /* a long time */
		}
	}
	if (!ASC_selectReadableAssociation(assocList, assocCount, timeout)) {
		/* none readable */
		return 0;
	}
	if (assocList[0] != NULL) {
		/* main association readable */
		return 1;
	}
	if (assocList[1] != NULL) {
		/* sub association readable */
		return 2;
	}
	/* should not be reached */
	return 0;
}

OFCondition
DIMSE_moveUser_MY(
    /* in */
    T_ASC_Association *assoc,
    T_ASC_PresentationContextID presID,
    T_DIMSE_C_MoveRQ *request,
    DcmDataset *requestIdentifiers,
    DIMSE_MoveUserCallback callback, void *callbackData,
    /* blocking info for response */
    T_DIMSE_BlockingMode blockMode, int timeout,
    /* sub-operation provider callback */
    T_ASC_Network *net,
    DIMSE_SubOpProviderCallback subOpCallback, void *subOpCallbackData,
    /* out */
    T_DIMSE_C_MoveRSP *response, DcmDataset **statusDetail,
    DcmDataset **rspIds,
    OFBool ignorePendingDatasets)
{
    T_DIMSE_Message req, rsp;
    DIC_US msgId;
    int responseCount = 0;
    T_ASC_Association *subAssoc = NULL;
    DIC_US status = STATUS_Pending;
    OFBool firstLoop = OFTrue;

    if (requestIdentifiers == NULL) return DIMSE_NULLKEY;

    bzero((char*)&req, sizeof(req));
    bzero((char*)&rsp, sizeof(rsp));

    req.CommandField = DIMSE_C_MOVE_RQ;
    request->DataSetType = DIMSE_DATASET_PRESENT;
    req.msg.CMoveRQ = *request;

    msgId = request->MessageID;

    OFCondition cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &req, NULL, requestIdentifiers, NULL, NULL);
    if (cond != EC_Normal) {
        return cond;
    }

    /* receive responses */

    while (cond == EC_Normal && status == STATUS_Pending) {

        /* if user wants, multiplex between net/subAssoc
         * and move responses over main assoc.
         */
        switch (selectReadable_MY(assoc, net, subAssoc, blockMode, timeout)) {
        case 0:
            /* none are readable, timeout */
            if ((blockMode == DIMSE_BLOCKING) || firstLoop) {
                firstLoop = OFFalse;
                continue;  /* continue with while loop */
            } else {
                return DIMSE_NODATAAVAILABLE;
            }
            /* break; */   // never reached after continue or return statement
        case 1:
            /* main association readable */
            firstLoop = OFFalse;
            break;
        case 2:
            /* net/subAssoc readable */
            if (subOpCallback) {
                subOpCallback(subOpCallbackData, net, &subAssoc);
            }
            firstLoop = OFFalse;
            continue;    /* continue with main loop */
            /* break; */ // never reached after continue statement
        }

        bzero((char*)&rsp, sizeof(rsp));

        cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &presID, &rsp, statusDetail);
        if (cond != EC_Normal) {
            return cond;
        }
        if (rsp.CommandField != DIMSE_C_MOVE_RSP) {
            char buf1[256];
            sprintf(buf1, "DIMSE: Unexpected Response Command Field: 0x%x", (unsigned)rsp.CommandField);
            return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf1);
        }

        *response = rsp.msg.CMoveRSP;

        if (response->MessageIDBeingRespondedTo != msgId) {
            char buf2[256];
            sprintf(buf2, "DIMSE: Unexpected Response MsgId: %d (expected: %d)", response->MessageIDBeingRespondedTo, msgId);
            return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf2);
        }

        status = response->DimseStatus;
        responseCount++;

        switch (status) {
        case STATUS_Pending:
            if (*statusDetail != NULL) {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "moveUser: Pending with statusDetail, ignoring detail");
                delete *statusDetail;
                *statusDetail = NULL;
            }
            if (response->DataSetType != DIMSE_DATASET_NULL) {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "moveUser: Status Pending, but DataSetType!=NULL");
                if (! ignorePendingDatasets) {
                    // Some systems send an (illegal) dataset following C-MOVE-RSP messages
                    // with pending status, which is a protocol violation, but we need to
                    // handle this nevertheless. The MV300 has been reported to exhibit
                    // this behavior.
                    DCMNET_WARN(DIMSE_warn_str(assoc) << "Reading but ignoring response identifier set");
                    DcmDataset *tempset = NULL;
                    cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presID, &tempset, NULL, NULL);
                    delete tempset;
                    if (cond != EC_Normal) {
                        return cond;
                    }
                } else {
                    // The alternative is to assume that the command set is wrong
                    // and not to read a dataset from the network association.
                    DCMNET_WARN(DIMSE_warn_str(assoc) << "Assuming NO response identifiers are present");
                }
            }

            /* execute callback */
            if (callback) {
                callback(callbackData, request, responseCount, response);
            }
            break;
        default:
            if (response->DataSetType != DIMSE_DATASET_NULL) {
                cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presID, rspIds, NULL, NULL);
                if (cond != EC_Normal) {
                    return cond;
                }
            }
            break;
        }
    }

    /* do remaining sub-association work, we may receive a non-pending
     * status before the sub-association has cleaned up.
     */
    while (subAssoc != NULL) {
        if (subOpCallback) {
            subOpCallback(subOpCallbackData, net, &subAssoc);
        }
    }

    return cond;
}
CONDITION MoveAssociation::moveSCU(DcmDataset *pdset) {
	CONDITION cond;
	T_ASC_PresentationContextID presId;
	T_DIMSE_C_MoveRQ req;
	T_DIMSE_C_MoveRSP rsp;
	DIC_US msgId = assoc->nextMsgID++;
	DcmDataset* rspIds = NULL;
	const char* sopClass;
	DcmDataset* statusDetail = NULL;

	if (pdset == NULL) {
		return makeOFCondition(OFM_dcmnet, DIMSEC_BADDATA, OF_error, "NULL DIMSE Query");
	}

	//sopClass = m_abstractSyntaxMove;
	sopClass = m_abstractSyntax.c_str();

	// which presentation context should be used
	presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);

	if (presId == 0) {
		return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
	}

	req.MessageID = msgId;
	strcpy(req.AffectedSOPClassUID, sopClass);
	req.Priority = DIMSE_PRIORITY_HIGH;
	req.DataSetType = DIMSE_DATASET_PRESENT;
	strcpy(req.MoveDestination, m_ourAET.c_str());

	cond = DIMSE_moveUser_MY(
		assoc,
		presId,
		&req,
		pdset,
		NULL,
		NULL,
		DIMSE_BLOCKING,
		0,
		GetNetwork()->GetDcmtkNet(),
		subOpCallback,//NULL,
		this,
		&rsp, &statusDetail, &rspIds, false);

	if( rsp.DimseStatus != STATUS_Success && rsp.DimseStatus != STATUS_Pending) {
		std::ostringstream os;
		os << DU_cmoveStatusString(rsp.DimseStatus);
		if (statusDetail != NULL) {
			OFString errorComment;
			if (statusDetail->findAndGetOFString(DCM_ErrorComment, errorComment).good()) {
				os << ". " << errorComment.c_str();
			}
		}	
		m_errorMessage = os.str();
	}

	if (statusDetail != NULL) {
		LOG_DEBUG(ambitolog, "DIMSE_moveUser(): Estado: " << std::endl << DumpDataset(statusDetail));
		delete statusDetail;
	}

	if (rspIds != NULL) {
		delete rspIds;
	}

	if(m_errorMessage != "" || rsp.NumberOfFailedSubOperations > 0 ) {
		if (m_errorMessage.size() == 0) {
			m_errorMessage = "Some operations failed";
		}
		return makeOFCondition(OFM_dcmnet, 18, OF_error, m_errorMessage.c_str());
	}

	return cond;
}

void MoveAssociation::SetPath(std::string path) {
	m_pathParam = path;
}
void MoveAssociation::SetNumeroImagene(int index) {
	m_numeroImagenes = index;
}
struct StoreCallbackData
{
	char *imageFileName;
	DcmFileFormat *dcmff;
	T_ASC_Association *assoc;
};

void MoveAssociation::storeSCPCallback(
	/* in */
	void *callbackData,
	T_DIMSE_StoreProgress *progress,    /* progress state */
	T_DIMSE_C_StoreRQ *req,             /* original store request */
	char *imageFileName, DcmDataset **imageDataSet, /* being received into */
	/* out */
	T_DIMSE_C_StoreRSP *rsp,            /* final store response */
	DcmDataset **statusDetail) 
{
    DIC_UI sopClass;
    DIC_UI sopInstance;
	OFCmdUnsignedInt  opt_sleepDuring = 0;//by jia
	OFBool            opt_abortDuringStore = OFFalse;//by jia
	OFBool            opt_abortAfterStore = OFFalse;//by jia

    if ((opt_abortDuringStore && progress->state != DIMSE_StoreBegin) ||
        (opt_abortAfterStore && progress->state == DIMSE_StoreEnd)) {
        LOG_INFO("movescuLogger", "ABORT initiated (due to command line options)");
        ASC_abortAssociation(OFstatic_cast(StoreCallbackData*, callbackData)->assoc);
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        return;
    }

    if (opt_sleepDuring > 0)
    {
      OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepDuring));
    }

    // dump some information if required (depending on the progress state)
    // We can't use oflog for the pdu output, but we use a special logger for
    // generating this output. If it is set to level "INFO" we generate the
    // output, if it's set to "DEBUG" then we'll assume that there is debug output
    // generated for each PDU elsewhere.
    //OFLogger progressLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION ".progress");//by jia
    //if (progressLogger.getChainedLogLevel() == OFLogger::INFO_LOG_LEVEL)
    //{
    //  switch (progress->state)
    //  {
    //    case DIMSE_StoreBegin:
    //      COUT << "RECV: ";
    //      break;
    //    case DIMSE_StoreEnd:
    //      COUT << OFendl;
    //      break;
    //    default:
    //      COUT << '.';
    //      break;
    //  }
    //  COUT.flush();
    //}

    if (progress->state == DIMSE_StoreEnd)
    {
       *statusDetail = NULL;    /* no status detail */

       /* could save the image somewhere else, put it in database, etc */
       /*
        * An appropriate status code is already set in the resp structure, it need not be success.
        * For example, if the caller has already detected an out of resources problem then the
        * status will reflect this.  The callback function is still called to allow cleanup.
        */
       // rsp->DimseStatus = STATUS_Success;

       if ((imageDataSet != NULL) && (*imageDataSet != NULL))//by jia && !opt_bitPreserving && !opt_ignore)
       {
         StoreCallbackData *cbdata = OFstatic_cast(StoreCallbackData*, callbackData);
         /* create full path name for the output file */
         OFString ofname;
		 OFString          opt_outputDirectory = "G:\\temp1";//m_pathParam;//by jia
         OFStandard::combineDirAndFilename(ofname, opt_outputDirectory, cbdata->imageFileName, OFTrue /* allowEmptyDirName */);
         if (OFStandard::fileExists(ofname))
         {
           LOG_WARN("movescuLogger", "DICOM file already exists, overwriting: " << ofname);
         }
		 E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;//by jia
         E_TransferSyntax xfer = opt_writeTransferSyntax;
         if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

		 E_EncodingType    opt_sequenceType = EET_ExplicitLength;//by jia
		 E_GrpLenEncoding  opt_groupLength = EGL_recalcGL;		 //by jia
		 E_PaddingEncoding opt_paddingType = EPD_withoutPadding; //by jia
		 OFCmdUnsignedInt  opt_filepad = 0;						 //by jia
		 OFCmdUnsignedInt  opt_itempad = 0;						 //by jia
		 OFBool            opt_useMetaheader = OFTrue;			 //by jia

         OFCondition cond = cbdata->dcmff->saveFile(ofname.c_str(), xfer, opt_sequenceType, opt_groupLength,
           opt_paddingType, OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad),
           (opt_useMetaheader) ? EWM_fileformat : EWM_dataset);
         if (cond.bad())
         {
           LOG_ERROR("movescuLogger", "cannot write DICOM file: " << ofname);
           rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;

           // delete incomplete file
           OFStandard::deleteFile(ofname);
         }

        /* should really check the image to make sure it is consistent,
         * that its sopClass and sopInstance correspond with those in
         * the request.
         */
        if ((rsp->DimseStatus == STATUS_Success))//by jia && !opt_ignore)
        {
          /* which SOP class and SOP instance ? */
			OFBool            opt_correctUIDPadding = OFFalse;//by jia
          if (!DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, opt_correctUIDPadding))
          {
             LOG_FATAL("movescuLogger", "bad DICOM file: " << imageFileName);
             rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
          }
          else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
          {
            rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
          }
          else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
          {
            rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
          }
        }
      }
    }
}

CONDITION MoveAssociation::echoSCP(T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID presID) {
    CONDITION cond;

    // the echo succeeded !!
    cond = DIMSE_sendEchoResponse(assoc, presID, &msg->msg.CEchoRQ, STATUS_Success, NULL);

    return cond;
}
OFCondition MoveAssociation::acceptSubAssoc(T_ASC_Network *aNet, T_ASC_Association **assoc)
{
    const char *knownAbstractSyntaxes[] = {
        UID_VerificationSOPClass
    };
    const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  // 10
                                       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  // 20
                                       NULL };                                                      // +1
    int numTransferSyntaxes;
    OFString temp_str;

    OFCondition cond = ASC_receiveAssociation(aNet, assoc, 16384);//by jia opt_maxPDU);
	E_TransferSyntax  opt_in_networkTransferSyntax = EXS_Unknown;

	if (cond.good()) {//by jia
		LOG_INFO("movescuLogger", "Sub-Association Received");
		LOG_DEBUG("movescuLogger", "Parameters:" << OFendl << ASC_dumpParameters(temp_str, (*assoc)->params, ASC_ASSOC_RQ));

		//dicomservice.cpp 文件中GetSupportedTransferSyntaxUIDs的调用，随后需要添加进来。
		const GIL::DICOM::SOPClassMap& scps = GIL::DICOM::Conformance::GetScpSOPClasses();
		for (GIL::DICOM::SOPClassMap::const_iterator it = scps.begin(); cond.good() && it != scps.end(); ++it) {
			GIL::DICOM::ArrayHelper transfersyntaxes(scps.GetSupportedTransferSyntaxUIDs((*it).first));
			const char* scuid = (*it).second.scUID.c_str();
			cond = ASC_acceptContextsWithPreferredTransferSyntaxes((*assoc)->params, &scuid, 1, transfersyntaxes.array, transfersyntaxes.size); 
			transfersyntaxes.free();
		}
	}
    if (cond.good()) {
        cond = ASC_acknowledgeAssociation(*assoc);
	}
    if (cond.good())    {
        LOG_INFO("movescuLogger", "Sub-Association Acknowledged (Max Send PDV: " << (*assoc)->sendPDVLength << ")");
        if (ASC_countAcceptedPresentationContexts((*assoc)->params) == 0) {
            LOG_INFO("movescuLogger", "    (but no valid presentation contexts)");
		}
        /* dump the presentation contexts which have been accepted/refused */
        LOG_DEBUG("movescuLogger", ASC_dumpParameters(temp_str, (*assoc)->params, ASC_ASSOC_AC));
    } else {
        LOG_ERROR("movescuLogger", DimseCondition::dump(temp_str, cond));
        ASC_dropAssociation(*assoc);
        ASC_destroyAssociation(assoc);
    }
    return cond;
}