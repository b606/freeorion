// -*- C++ -*-
//MapWnd.h
#ifndef _MapWnd_h_
#define _MapWnd_h_

#include <vector>

#include "CUIWnd.h"
#include "../util/XMLDoc.h"

class CUIButton;
class CUIEdit;
class CUITurnButton;
class Fleet;
class FleetButton;
class MapWndPopup;
class ProductionWnd;
class ResearchWnd;
class SidePanel;
class SitRepPanel;
class System;
class SystemIcon;
class StatisticIcon;
class StatisticIconDualValue;
class CUIToolBar;
class UniverseObject;

namespace GG {
    class Texture;
    class MultiEdit;
}

/** This class is a window that graphically displays everything in the universe */
class MapWnd : public GG::Wnd
{
public:
    //! \name Signal Types //!@{
    typedef boost::signal<void (int)> SystemLeftClickedSignalType;  //!< emitted when the user left-clicks a star system
    typedef boost::signal<void (int)> SystemRightClickedSignalType; //!< emitted when the user right-clicks a star system
    typedef boost::signal<void (int)> SystemBrowsedSignalType;      //!< emitted when the user moves the mouse over a star system
    //!@}

    //! \name Slot Types //!@{
    typedef SystemLeftClickedSignalType::slot_type  SystemLeftClickedSlotType;  //!< type of functor invoked when the user left-clicks a star system
    typedef SystemRightClickedSignalType::slot_type SystemRightClickedSlotType; //!< type of functor invoked when the user right-clicks a star system
    typedef SystemBrowsedSignalType::slot_type      SystemBrowsedSlotType;      //!< type of functor invoked when the user moves the mouse over a star system
    //!@}

    //! \name Structors //!@{
    MapWnd();    //!< default ctor
    ~MapWnd();   //!< default dtor
    //!@}

    //! \name Accessors //!@{
    virtual GG::Pt ClientUpperLeft() const;

    double         ZoomFactor() const    {return m_zoom_factor;}
    SidePanel*     GetSidePanel() const  {return m_side_panel;}
    XMLElement     SaveGameData() const;         //!< returns the relevant data that should be restored after a save-and-load cycle
    bool           InProductionViewMode() const; //!< returns tru iff this MapWnd is visible and usable for interaction, but the allowed interactions are restricted to those appropriate to the production screen
    //!@}

    //! \name Mutators //!@{
    virtual void   Render();
    virtual void   KeyPress (GG::Key key, Uint32 key_mods);
    virtual void   LButtonDown(const GG::Pt& pt, Uint32 keys);
    virtual void   LDrag(const GG::Pt& pt, const GG::Pt& move, Uint32 keys);
    virtual void   LButtonUp(const GG::Pt& pt, Uint32 keys);
    virtual void   LClick(const GG::Pt& pt, Uint32 keys);
    virtual void   RClick(const GG::Pt& pt, Uint32 keys);
    virtual void   MouseWheel(const GG::Pt& pt, int move, Uint32 keys);

    void           InitTurn( int turn_number );                     //!< called at the start of each turn
    void           RestoreFromSaveData(const XMLElement& elem); //!< restores the UI state that was saved in an earlier call to SaveGameData().
    void           ShowSystemNames();                               //!< enables the system name text
    void           HideSystemNames();                               //!< disables the system name text
    void           HandlePlayerChatMessage(const std::string& msg); //!< displays incoming player chat text

    mutable SystemLeftClickedSignalType  SystemLeftClickedSignal;
    mutable SystemRightClickedSignalType SystemRightClickedSignal;
    mutable SystemBrowsedSignalType      SystemBrowsedSignal;

    void CenterOnMapCoord(double x, double y); //!< centers the map on map position (x, y)
    void CenterOnSystem(int systemID);         //!< centers the map on system \a systemID
    void CenterOnFleet(int fleetID);           //!< centers the map on fleet \a fleetID
    void CenterOnSystem(System* system);       //!< centers the map on system \a system
    void CenterOnFleet(Fleet* fleet);          //!< centers the map on fleet \a fleet
    void ShowTech(const std::string& tech_name);//!< brings up the research screen and centers the tech tree on \a tech_name
    void SelectSystem(int systemID);           //!< catches emitted signals from the system icons, and allows programmatic selection of planets
    void SelectFleet(int fleetID);             //!< allows programmatic selection of fleets
    void SelectSystem(System* system);         //!< allows programmatic selection of planets
    void SelectFleet(Fleet* fleet);            //!< allows programmatic selection of fleets

    void SetFleetMovement(FleetButton* fleet_button); //!< creates fleet movement lines for all fleets in the given FleetButton to indicate where (and whether) they are moving
    void SetFleetMovement(Fleet* fleet);       //!< creates fleet movement lines for a single fleet that are already moving
    void SetProjectedFleetMovement(Fleet* fleet, const std::list<System*>& travel_route); //!< creates projected fleet movement lines for a fleet that the user has selected

    void RegisterPopup( MapWndPopup* popup );  //!< registers a MapWndPopup, which can be cleaned up with a call to DeleteAllPopups( )
    void RemovePopup( MapWndPopup* popup );    //!< removes a MapWndPopup from the list cleaned up on a call to DeleteAllPopups( )
    void Cleanup();                            //!< cleans up the MapWnd at the end of a turn (ie, closes all windows and disables all keyboard accelerators)
    void Sanitize();                           //!< sanitizes the MapWnd after a game
    //!@}
        
    static const int    SIDE_PANEL_WIDTH;

protected:
    virtual bool   EventFilter(GG::Wnd* w, const GG::Wnd::Event& event);

private:
    void FoodResourcePoolChanged();
    void MineralResourcePoolChanged();
    void TradeResourcePoolChanged();
    void ResearchResourcePoolChanged();
    void PopulationResourcePoolChanged();
    void IndustryResourcePoolChanged();

    void TurnBtnClicked() {EndTurn();}
    void MenuBtnClicked() {ShowMenu();}
    void ProductionBtnClicked() {ToggleProduction();}
    void ResearchBtnClicked() {ToggleResearch();}
    void SitRepBtnClicked() {ToggleSitRep();}

    struct StarlaneData;     ///< contains all the information necessary to render a single fleet movement line on the main map

    /** contains all the information necessary to render a single fleet movement line on the main map */
    struct MovementLineData
    {
        MovementLineData() {}
        MovementLineData(double x_, double y_, const std::list<System*>& dest, GG::Clr clr = GG::CLR_WHITE) :
            x(x_), y(y_), destinations(dest), color(clr) {}
        double x;
        double y;
        std::list<System*> destinations;
        GG::Clr color;
        // TODO : other properties, based on moving empire and destination, etc. (?)
    };

    void Zoom(int delta);                        //!< changes the zoomlevel of the main map
    void RenderBackgrounds();                    //!< renders the backgrounds onto the screen
    void RenderStarlanes();                      //!< renders the starlanes between the systems
    void RenderFleetMovementLines();             //!< renders the dashed lines indicating where each fleet is going
    void MoveBackgrounds(const GG::Pt& move);    //!< scrolls the backgrounds at their respective rates
    void CorrectMapPosition(GG::Pt &move_to_pt); //!< ensures that the map data are positioned sensibly
    void SystemDoubleClicked(int system_id);
    void SystemRightClicked(int system_id);
    void MouseEnteringSystem(int system_id);
    void MouseLeavingSystem(int system_id);
    void UniverseObjectDeleted(const UniverseObject *obj);
    bool ReturnToMap();
    bool OpenChatWindow();
    bool OpenConsoleWindow();
    bool EndTurn();
    bool ToggleSitRep();
    bool ToggleResearch();
    bool ToggleProduction();
    bool ShowMenu();
    bool CloseSystemView();                      //!< closes off the current system view
    bool KeyboardZoomIn();
    bool KeyboardZoomOut();
    bool ZoomToHomeSystem();
    bool ZoomToPrevOwnedSystem();
    bool ZoomToNextOwnedSystem();
    bool ZoomToPrevIdleFleet();
    bool ZoomToNextIdleFleet();
    bool ZoomToPrevFleet();
    bool ZoomToNextFleet();

    void SetAccelerators();
    void RemoveAccelerators();
    void DisableAlphaNumAccels();
    void EnableAlphaNumAccels();
    void CloseAllPopups();
    void HideAllPopups();
    void ShowAllPopups();

    std::set<GG::Key> m_disabled_accels_list;     //!< the list of Accelerators disabled by \a DisableAlphaNumAccels

    std::vector<boost::shared_ptr<GG::Texture> > m_backgrounds; //!< starfield backgrounds
    std::vector<boost::shared_ptr<GG::Texture> > m_nebulae;     //!< decorative nebula textures
    std::vector<GG::Pt>   m_nebula_centers; //!< the centerpoints of each of the nebula textures
    std::vector<double>   m_bg_scroll_rate; //!< array, the rates at which each background scrolls
    std::vector<double>   m_bg_position_X;  //!< array, the X position of the first full background image
    std::vector<double>   m_bg_position_Y;  //!< array, the Y positions of the backgrounds

    double                          m_zoom_factor;   //! the current zoom level; clamped to [MIN_SCALE_FACTOR, MAX_SCALE_FACTOR]
    SidePanel*                      m_side_panel;    //! the planet view panel on the side of the main map
    std::map<int, SystemIcon*>      m_system_icons;  //! the system icons in the main map, indexed by system id
    SitRepPanel*                    m_sitrep_panel;  //! the sitrep panel
    ResearchWnd*                    m_research_wnd;  //! the research screen
    ProductionWnd*                      m_production_wnd;  //! the production screen
    GG::MultiEdit*                  m_chat_display;  //! the (read-only) MP-chat output multi-line edit box
    CUIEdit*                        m_chat_edit;     //! the MP-chat input edit box
    std::vector<FleetButton*>       m_moving_fleet_buttons; //! the moving fleets in the main map
    std::set<StarlaneData>          m_starlanes;     //! the starlanes between systems
    std::map<Fleet*, 
             MovementLineData>      m_fleet_lines;   //! the lines used for moving fleets in the main map
    MovementLineData                m_projected_fleet_lines; //! the lines that show the projected path of the active fleet in the FleetWnd
    GG::Pt                          m_drag_offset;   //! the distance the cursor is from the upper-left corner of the window during a drag ((-1, -1) if no drag is occurring)
    bool                            m_dragged;       //! tracks whether or not a drag occurs during a left button down sequence of events
    CUITurnButton*                  m_turn_update;   //!< button that updates player's turn
    std::list<MapWndPopup*>         m_popups;        //!< list of currently active popup windows
    bool                            m_menu_showing; //!< set during ShowMenu() to prevent reentrency
    int                             m_current_owned_system;
    int                             m_current_fleet;
    bool                            m_in_production_view_mode;

    CUIToolBar                      *m_toolbar;
    StatisticIconDualValue          *m_food,*m_mineral,*m_trade,*m_population;
    StatisticIcon                   *m_research,*m_industry;

    CUIButton                       *m_btn_siterep,*m_btn_research,*m_btn_production,*m_btn_menu;

    static const int NUM_BACKGROUNDS;
    static double s_min_scale_factor;
    static double s_max_scale_factor;
};


/** Derive any window from this class to have it managed by MapWnd. For example, MapWnd will delete all open popups
   when the end turn button is hit. */
class MapWndPopup : public CUIWnd
{
public:
    MapWndPopup( const std::string& t, int x, int y, int h, int w, Uint32 flags );
    MapWndPopup(const XMLElement& elem);     
    virtual ~MapWndPopup();

    void CloseClicked();

    void Close();      //!< closes the MapWndPopup   
};

#endif // _MapWnd_h_
