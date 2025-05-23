// KP Entry uuid

#pragma once


class KpID {

Byte data[16];

public:

  KpID() {clear();}
 ~KpID() {clear();}

  void  clear() {ZeroMemory(data, 16);}

  KpID& operator= (BYTE* uuid) {memcpy_s(data, 16, uuid, 16);   return *this;}

  bool  operator== (BYTE* uuid) {return !memcmp(data, uuid, 16);}
  };

