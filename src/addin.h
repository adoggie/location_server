#ifndef _ADDIN_LOCATION_H
#define _ADDIN_LOCATION_H

#include <framework.h>

#ifdef _ADDIN


#define ADDIN_ID_LOCATION 8

//#include "../../../include/framework.h"


extern "C" FxRESULT CreateAddin(int uid, FxIAddin **ppVal);


class AddinWrapper: public FxObject, public FxIAddin{

public:

	FxRESULT Initialize(FxIFramework *fw, FxIAddinConf *conf);
	FxRESULT Uninitialize();

	FxRESULT Run();
    virtual FxRESULT Quit();

};



#endif  //  _ADDIN

#endif
