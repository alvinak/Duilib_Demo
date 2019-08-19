/*
 *  
 *  $Id: dicomnetwork.cpp $
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
#include <sstream>
//#include <wx/string.h>
#include "dicomnetwork.h"
#include "dicomechoassociation.h"
//#include <api/controllers/ipacscontroller.h>
//#include <api/controllers/icontroladorlog.h>

#ifdef verify
#define MACRO_QUE_ESTORBA verify
#undef verify
#endif

#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmimage/diregist.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmjpeg/djencode.h>
#include <dcmtk/dcmdata/dcrledrg.h>
#include <dcmtk/dcmdata/dcrleerg.h>

//#include "tls/tls.h"
//#include "tls/gtlslayer.h"
//#include <dcmtk/dcmtls/tlstrans.h>


//#include <main/controllers/configurationcontroller.h>
//#include <wx/thread.h>

#ifdef MACRO_QUE_ESTORBA
#define verify MACRO_QUE_ESTORBA
#endif

GIL::DICOM::DCMTK::Network::IntanceMapType GIL::DICOM::DCMTK::Network::m_pInstancesMap;

//GNC::GCS::ILockable GIL::DICOM::DCMTK::Network::m_sLock;

GIL::DICOM::DCMTK::Network* GIL::DICOM::DCMTK::Network::Instance(void* key) {
	//GNC::GCS::ILocker lock(m_sLock);

	GIL::DICOM::DCMTK::Network* pInstance;

	GIL::DICOM::DCMTK::Network::IntanceMapType::iterator it = m_pInstancesMap.find(key);

	if (it == m_pInstancesMap.end()) {
		pInstance = new GIL::DICOM::DCMTK::Network();
		m_pInstancesMap[key] = pInstance;
		//by jia LOG_TRACE("DICOMNetwork", "Network instance created. key = " << key);
	}
	else {
		//by jia LOG_TRACE("DICOMNetwork", "Network instance reused. key = " << key);
		pInstance = (*it).second;
	}

	return pInstance;
}

void GIL::DICOM::DCMTK::Network::FreeInstance(void* key)
{
	//GNC::GCS::ILocker lock(m_sLock);

	GIL::DICOM::DCMTK::Network::IntanceMapType::iterator it = m_pInstancesMap.find(key);
	
	if (it == m_pInstancesMap.end()) {
		//by jia LOG_WARN("DICOMNetwork", "Unable to free Network instance. Instance was not registered.");
	}
	else {
		GIL::DICOM::DCMTK::Network* pInstance = (*it).second;
		delete pInstance;
		m_pInstancesMap.erase(it);
		//by jia LOG_TRACE("DICOMNetwork", "Network instance released. key = " << key);
	}

}

GIL::DICOM::DCMTK::Network::Network() {
	m_pDCMTKNetwork = NULL;
	m_Initiallized = false;
}

GIL::DICOM::DCMTK::Network::~Network() {
	DropNetwork();
}

CONDITION GIL::DICOM::DCMTK::Network::ConnectAssociation(Association* assoc, int pdu) {
	//GNC::GCS::ILocker lock(m_sLock);
	CONDITION cond;
	
	cond = ASC_ConnectAssociation(
								  assoc,
								  assoc->m_calledAET,
								  assoc->m_calledPeer,
								  assoc->m_calledPort,
								  assoc->m_ourAET,
								  pdu);
	if (!SUCCESS(cond)) {
		assoc->Drop(cond);
		return cond;
	}
	
	assoc->Net = this;
	assoc->msgId = assoc->assoc->nextMsgID;
	
	return cond;
}

bool GIL::DICOM::DCMTK::Network::Initiallized()
{
	return m_Initiallized;
}

CONDITION GIL::DICOM::DCMTK::Network::InitializeNetwork(int timeout, Association::RoleType role, int port) {

	if (m_pDCMTKNetwork != NULL) {
		//LOG_WARN("DICOMNetwork", "Shutting down previous initiallized network");
		DropNetwork();
	}

	CONDITION cond;
	
	switch (role) {
		case Association::RT_Acceptor:
			cond = ASC_initializeNetwork(NET_ACCEPTOR, 0, timeout, &m_pDCMTKNetwork);
			break;
		case Association::RT_Requestor:
			cond = ASC_initializeNetwork(NET_REQUESTOR, port, timeout, &m_pDCMTKNetwork);
			break;
		case Association::RT_AcceptorRequestor:
			cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, port, timeout, &m_pDCMTKNetwork);
			break;
		default:
			cond = EC_IllegalParameter;
			break;
	}
	m_Initiallized = cond.good();
	return cond;
}

CONDITION GIL::DICOM::DCMTK::Network::DropNetwork() {
	
	CONDITION cond = ASC_dropNetwork(&m_pDCMTKNetwork);
	m_pDCMTKNetwork = NULL;
	
	return cond;
}

CONDITION GIL::DICOM::DCMTK::Network::ASC_ConnectAssociation(Association* assoc, const std::string& peerTitle, const std::string& peer, int port, const std::string& ouraet, int pdu) {
	CONDITION cond;
	//by jia
	//GTLSTransportLayer *tLayer = NULL;
	//
 //   if (assoc->IsSecure())
 //   {
	//	tLayer = new GTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, NULL);
	//	if (tLayer == NULL)
	//	{
	//		return makeOFCondition(OFM_dcmtls, DULC_TLSERROR, OF_error, "TLS Error: Unable to create TLS transport layer");
	//	}
	//	tLayer->setCertificateFromString(assoc->GetCliCert());
	//	tLayer->setPrivateKeyFromString(assoc->GetCliKey());
	//	
	//	if (! tLayer->checkPrivateKeyMatchesCertificate())
	//	{
	//		return makeOFCondition(OFM_dcmtls, DULC_TLSERROR, OF_error, "TLS Error: Private key and certificate do not match");
	//	}
	//	
	//	tLayer->addSystemTrustedCertificates();
	//	
	//	if (assoc->GetValidate()) {
	//		tLayer->setCertificateVerification(DCV_requireCertificate);
	//	}
	//	else {
	//		tLayer->setCertificateVerification(DCV_ignoreCertificate);
	//	}
	//	/*
	//	 if (opt_dhparam && ! (tLayer->setTempDHParameters(opt_dhparam)))
	//	 {
	//	 LOG_WARN(assoc->ambitolog, "unable to load temporary DH parameters. Ignoring");
	//	 }
	//	 */
	//	
	//	cond = ASC_setTransportLayer(m_pDCMTKNetwork, tLayer, 0);
	//	if (cond.bad())
	//	{
	//		return cond;
	//	}
 //   }	
	
    T_ASC_Parameters *params;
	 
	 std::string peerHost;
    char localHost[129];
	
    cond = ASC_createAssociationParameters(&params, pdu);
    if (!SUCCESS(cond)) {
		return cond;
    }

	 //user identity
	 if (assoc->UseUserPass()) {
		 cond = ASC_setIdentRQUserPassword(params, assoc->GetUser().c_str(), assoc->GetPass().c_str());
		 if (!SUCCESS(cond)) {
			return cond;
		 }
	 }

    ASC_setAPTitles(params, ouraet.c_str(), peerTitle.c_str(), NULL);
	 
	 cond = ASC_setTransportLayerType(params, assoc->IsSecure());
	 if (cond.bad()) {
		 return cond;
	}
	
	gethostname(localHost, sizeof (localHost) - 1);
	{
		std::ostringstream os;
		os << peer.c_str() << ":" << port;
		peerHost = os.str();
	}
	ASC_setPresentationAddresses(params, localHost, peerHost.c_str());

	assoc->OnAddPresentationContext(params);

	{
		OFString str;
		ASC_dumpParameters(str, params, ASC_ASSOC_RQ);
		//by jia LOG_DEBUG(assoc->ambitolog, "Requesting association" << std::endl << str.c_str());

	}
	
	/* create association */
	cond = ASC_requestAssociation(m_pDCMTKNetwork, params, &(assoc->assoc));
	if (cond != ASC_NORMAL) {
		if (cond == ASC_ASSOCIATIONREJECTED) {
			T_ASC_RejectParameters rej;
			
			ASC_getRejectParameters(params, &rej);
			OFString str;
			ASC_printRejectParameters(str, &rej);
			return makeOFCondition(OFM_dcmnet, DULC_ASSOCIATIONREJECTED, OF_error, str.c_str());
		}
		else {
			return cond;
		}
    }

	{
		OFString str;
		ASC_dumpParameters(str, params, ASC_ASSOC_AC);
		//by jia LOG_DEBUG(assoc->ambitolog, "Association accepted" << std::endl << str.c_str());
	}
	
    /* what has been accepted/refused ? */
	
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
		return cond;
    }
	
    return ASC_NORMAL;
}

CONDITION GIL::DICOM::DCMTK::Network::SendEchoRequest(const std::string& title, const std::string& peer, int port, const std::string& ouraet, int pdu) {
	EchoAssociation dcmEcho("C-ECHO");
	dcmEcho.Create(title, peer, port, ouraet, UID_VerificationSOPClass);
	
	ConnectAssociation(&dcmEcho, pdu);
	
	return dcmEcho.SendEchoRequest();
}

T_ASC_Network* GIL::DICOM::DCMTK::Network::GetDcmtkNet() {
    return m_pDCMTKNetwork;
}

void GIL::DICOM::DCMTK::Network::SetDcmtkNet(T_ASC_Network* n) {
    m_pDCMTKNetwork = n;
}
