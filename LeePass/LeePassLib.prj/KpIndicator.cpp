// KpIndicator -- Three fields that suggest a match in a search (not a unique name)


#include "pch.h"
#include "KpIndicator.h"


bool KpIndicator::operator=  (KpIndicator& idctr)
                   {return idctr.title == title && idctr.url == url && idctr.userName == userName;}
bool KpIndicator::operator!= (KpIndicator& idctr)
                   {return idctr.title != title || idctr.url != url || idctr.userName != userName;}

