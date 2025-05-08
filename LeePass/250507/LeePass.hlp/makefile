

LeePass.chm : *.htm LeePass.hhp LeePass.hhc App.css
  -1 "C:\Program Files (x86)\HTML Help Workshop\hhc" LeePass


Clean.cln    :
  @echo "Cleaning"
  cd $(MAKEDIR)
  del /F LeePass.chm


Release/LeePass.chm : LeePass.chm
  copy $(MAKEDIR)\LeePass.chm  $(MAKEDIR)\..\Release\LeePass.chm


Release/Clean.cln    : Clean.cln
  cd $(MAKEDIR)\..\Release
  del LeePass.chm
  cd $(MAKEDIR)


Debug/LeePass.chm : LeePass.chm
  copy $(MAKEDIR)\LeePass.chm  $(MAKEDIR)\..\Debug\LeePass.chm


Debug/Clean.cln    : Clean.cln
  cd $(MAKEDIR)\..\Debug
  del LeePass.chm
  cd $(MAKEDIR)


Release/Rebuild.rbd : Release/Clean.cln Release/LeePass.chm


Debug/Rebuild.rbd   : Debug/Clean.cln Debug/LeePass.chm


