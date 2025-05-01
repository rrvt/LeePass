// Password Generator


#pragma once


class Generator {

int frequencies[256];

public:

  Generator() { }
 ~Generator() { }

  bool   get(String& password);

  double quality(TCchar* pswd);

private:

  void   addChars(TCchar* set);
  };

