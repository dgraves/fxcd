#ifndef _CDPREFERENCES_H_
#define _CDPREFERENCES_H_

class CDWindow;

class CDPreferences : public FXDialogBox
{
  FXDECLARE(CDPreferences)
protected:
  CDWindow* cdwindow;
  CDPreferences(){}
public:
  CDPreferences(CDWindow* owner);
  //virtual void create();

  enum
  {
    ID_SERVERLIST=FXDialogBox::ID_LAST,
    ID_LAST
  };
};

#endif
