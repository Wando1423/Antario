#pragma once
#include <vector>
#include "..\Utils\DrawManager.h"

enum class MenuSelectableType;

struct MenuStyle
{
    int   iPaddingX       = 20;                     /*- Padding between sections                            -*/
    int   iPaddingY       = 6;                      /*- Padding between selectables                         -*/
    Color colCursor       = { 0, 150, 255, 100 };   /*- Color of the mouse cursor                           -*/
    Color colHover        = { 100, 100, 100, 50 };  /*- Color applied on the obj when mouse hovers above it -*/
    Color colSectionOutl  = { 15, 15, 15, 200 };    /*- Color of the section outline                        -*/
    Color colSectionFill  = { 0, 0, 0, 25 };        /*- Color filling the section bounds                    -*/
    Color colCheckbox1    = { 50, 50, 50, 255 };    /*- Color of the first gradient color of the checkbox   -*/
    Color colCheckbox2    = { 35, 35, 35, 255 };    /*- Color of the second gradient color of the checkbox  -*/
    Color colText         = { 160, 160, 160, 255 }; /*- Color of the text inside the main window            -*/
    Color colHeader       = { 50, 50, 50, 230 };    /*- Color of the header background                      -*/
    Color colHeaderText   = { 200, 200, 215 };      /*- Color of the text inside the header strip           -*/
    Color colComboBoxRect = { 50, 50, 50, 180 };    /*- Color of the combobox rectangle                     -*/
};



class MouseCursor
{
public:
    MouseCursor() { this->vecPointPos = g_Render.GetScreenCenter(); };
    virtual      ~MouseCursor() = default;
    virtual void Render();
    virtual void RunThink(UINT uMsg, LPARAM lParam);

    SPoint     GetPosition() const { return vecPointPos; }; /* Set&get actual mouse position */
    virtual void SetPosition(SPoint ptPos) { this->vecPointPos = ptPos; };

    /* Checks if the mouse is in specified bounds */
    virtual bool IsInBounds(const SPoint& vecDst1, const SPoint& vecDst2);
    virtual bool IsInBounds(const SRect& rcRect);

    bool     bLMBPressed = false; /* Actual state of Left Mouse Button (pressed or not)   */
    bool     bRMBPressed = false; /* Actual state of Right Mouse Button (pressed or not)  */
    bool     bLMBHeld = false;
    bool     bRMBHeld = false;
    SPoint   vecPointPos;         /* Current mouse cursor position                        */
};



class MenuMain
{
protected:
    using SSize = SPoint;
public:
    MenuMain() : type(), pParent(nullptr), bIsHovered(false), iMaxChildWidth(0) {}
    virtual      ~MenuMain() = default;
    virtual void RunThink(UINT uMsg, LPARAM lParam);
    virtual void Initialize();
    virtual bool UpdateData();      /* Updates all the needed data like size, position, etc. */
    virtual void Render();          /* Renders the object                                    */

                                    /* Size and position get/set     */

    virtual SPoint GetPos()                     { return this->rcBoundingBox.Pos(); }
    virtual void   SetPos(SPoint ptNewPosition) { this->rcBoundingBox.left = ptNewPosition.x; rcBoundingBox.top = ptNewPosition.y; }
    virtual SSize  GetSize()                    { return this->szSizeObject;     }
    virtual void   SetSize(SSize sNewSize)      { this->szSizeObject = sNewSize; }
    virtual SRect  GetBBox()                    { return this->rcBoundingBox;  }
    virtual int    GetMaxChildWidth()           { return this->iMaxChildWidth; }

    virtual void   SetupSize();

    /* Parent/child setting functions */

    virtual void SetParent(MenuMain* parent);
    virtual void AddChild(const std::shared_ptr<MenuMain>& child);

    static std::unique_ptr<MouseCursor> mouseCursor; /* Pointer to our mouse cursor                             */
    static MenuStyle style;                          /* Struct containing all colors / paddings in our menu.    */
    static CD3DFont* pFont;                          /* Pointer to the font used in the menu.                   */

    std::vector<std::shared_ptr<MenuMain>> vecChildren;
    MenuSelectableType                     type;     /* Type of an object.                                      */
    std::string                            strLabel; /* Label / Name of the window.                             */

protected:
    virtual void AddDummy();
    virtual void AddCheckBox(std::string strSelectableLabel, bool *bValue);
    virtual void AddButton(std::string strSelectableLabel, void(&fnPointer)(), SPoint ptButtonSize = SPoint(0, 0));
    virtual void AddCombo(std::string strSelectableLabel, std::vector<std::string> vecBoxOptions, int* iVecIndex);
    virtual void AddSlider(std::string strLabel, float* flValue, float flMinValue, float flMaxValue);
    virtual void AddSlider(std::string strLabel, int* iValue, int iMinValue, int iMaxValue);

protected:
    MenuMain* pParent;
    bool      bIsHovered;     /* Defines if the selectable is hovered with the mouse cursor.  */
    int       iMaxChildWidth; /* Maximum child width. Set mainly for buttons and selectables. */
    SSize     szSizeObject;   /* Size of the drawed object                                    */   
    SRect     rcBoundingBox;  /* Bounding box of the drawed ent.                              */
};



class BaseWindow : public MenuMain
{
public:
    BaseWindow() : bIsDragged(false), bIsInitialized(false), ptOldMousePos(), pHeaderFont(nullptr), iHeaderHeight(0) { }
    BaseWindow(SPoint ptPosition, SPoint szSize, CD3DFont* font, CD3DFont* headerFont, std::string strLabel = "");
    virtual void Render();
    virtual bool UpdateData();

    bool      bIsDragged;    /* Says if the window is currently dragged. */
private:
    virtual int  GetHeaderHeight();

    bool      bIsInitialized;
    SPoint    ptOldMousePos; /* Old mouse position used for dragging     */
    CD3DFont* pHeaderFont;   /* Header only font                         */
    int       iHeaderHeight; /* Header height in pixels                  */
};



class BaseSection : public BaseWindow
{
public:
    BaseSection(SPoint szSize, int iNumRows, std::string strLabel);
    virtual void Render();
    virtual bool UpdateData();

private:
    void SetupPositions();          /* Calculates proper positions for all child selectables.   */
    bool bIsInitialized = false;    /* Local boolean for initialization purposes.               */
    int iNumRows;                   /* Number of vertical rows we will split section to.        */

};



class Checkbox : public MenuMain
{
public:
    Checkbox(std::string strLabel, bool *bValue, MenuMain* pParent);
    virtual void Render();
    virtual bool UpdateData();

    bool*  bCheckboxValue;         /* The value we are changing with the checkbox                  */
private:
    SSize  szSelectableSize;
    SPoint ptSelectablePosition;
};



class Button : public MenuMain
{
public:
    Button(std::string strLabel, void(&fnPointer)(), MenuMain* pParent, SPoint ptButtonSize = SPoint(0, 0));
    virtual void Render();
    virtual bool UpdateData();
private:
    bool    bIsActivated;           /* Defines if we activated the button                            */
    void(*fnActionPlay)();      /* Pointer to the function that will be run at the button press. */
};



class ComboBox : public MenuMain
{
public:
    ComboBox(std::string strLabel, std::vector<std::string> vecBoxOptions, int* iCurrentValue, MenuMain* pParent);
    virtual void Render();
    virtual bool UpdateData();

    virtual SPoint GetSelectableSize();
private:
    bool   bIsActive;            /* Boolean defining if we are supposed to draw our list or not.     */
    bool   bIsButtonHeld;
    int    idHovered;
    int*   iCurrentValue;        /* Current selected option. Defined as a vector index.              */
    SPoint szSelectableSize;     /* Size of the internal selectable size of the combo                */
    SPoint ptSelectablePosition; /* Position of the selectable                                       */
    std::vector<std::string> vecSelectables;  /* Vector of strings that will appear as diff settings. */
};


template <typename T>
class Slider : public MenuMain
{
public:
    Slider(const std::string& strLabel, T* tValue, T tMinValue, T tMaxValue, MenuMain* pParent);
    virtual void Render();
    virtual bool UpdateData();

    float GetValuePerPixel() const;
    void  SetValue(T flValue);
private:
    T*   nValue;      /* Slider current value                    */
    T    nMin;        /* Minimal slider value                    */
    T    nMax;        /* Maximal slider value                    */
    int  iDragX{};      /* Mouse position at the start of the drag */
    int  iDragOffset{}; /* Offset of the mouse position            */
    int  iButtonPosX{}; /* Slider button representing value        */
    bool bPressed{};             
    SPoint szSelectableSize;     /* Size of the internal selectable size of the combo */
    SPoint ptSelectablePosition; /* Position of the selectable                        */
};


class DummySpace : public MenuMain
{
public:
    DummySpace(SPoint size) { this->szSizeObject = size; };
    void Render()     override { };
    bool UpdateData() override { return false; };
};

enum class MenuSelectableType
{
    TYPE_MAIN,
    TYPE_WINDOW,
    TYPE_SECTION,
    TYPE_CHECKBOX,
    TYPE_BUTTON,
    TYPE_COMBO,
    TYPE_SLIDER,
};
