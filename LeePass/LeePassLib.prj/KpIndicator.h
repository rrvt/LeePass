// KpIndicator -- Three fields that suggest a match in a search (not a unique name)


#pragma once


class KpIndicator {

public:
String title;
String url;
String userName;

  KpIndicator() { }
 ~KpIndicator() { }

  bool operator=  (KpIndicator& idctr);
  bool operator!= (KpIndicator& idctr);
  };

