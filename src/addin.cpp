
#include "addin.h"





#ifdef _ADDIN

#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include "server.h"

FxRESULT AddinWrapper::Initialize(FxIFramework *fw, FxIAddinConf *conf){

	if(NULL == fw)
		return FxE_FAIL;
	if(NULL == conf)
		return FxE_FAIL;

	const char *confile = NULL;
	if( FxS_OK != conf->GetConfigString(&confile) || confile == NULL){
		return FxE_FAIL;
	}

	TiXmlDocument doc;
	//std::string xml;
	doc.Parse( confile ) ;
	if(doc.Error()){
		return FxE_FAIL;
	}


	TiXmlElement* e = doc.RootElement();
	std::string file;
	while(e){
		e = e->FirstChildElement();
		std::string name,value;
		name = e->Value();
		value = e->GetText();
		if( name == "config"){
			file = value;

		}
		e = e->NextSiblingElement();
	}
	if(file == ""){
		std::cout<<"Error: [config] element not be found in XML tree! "<<std::endl;
		return FxE_FAIL ;
	}

	if(!LocServer::instance().init( file)){
		return FxE_FAIL;
	}
	return FxS_OK;

}

FxRESULT AddinWrapper::Uninitialize(){
	return FxS_OK;
}

FxRESULT AddinWrapper::Run(){
	LocServer::instance().exec();
	return FxS_OK;
}

FxRESULT AddinWrapper::Quit(){
	LocServer::instance().shutdown();
	return FxS_OK;
}


FxRESULT CreateAddin(int uid, FxIAddin **ppVal){

	if (ppVal == NULL)
        return FxE_INVALIDARG;

    FxIAddinPtr sp;
    if (uid == ADDIN_ID_LOCATION)
    {
    	AddinWrapper *addin = new AddinWrapper();
    	sp.Attach(addin);
    }
    else
        return FxE_INVALIDARG;

    *ppVal = sp.Detach();
    return FxS_OK;

}





#endif
