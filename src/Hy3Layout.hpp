#pragma once
#include <list>
#include <map>
#include <set>

#include <hyprland/src/layout/IHyprLayout.hpp>

#include "BitFlag.hpp"

class Hy3Layout;

enum class GroupEphemeralityOption {
	Ephemeral,
	Standard,
	ForceEphemeral,
};

enum class ShiftDirection {
	Left,
	Up,
	Down,
	Right,
};

enum class SearchDirection { None, Forwards, Backwards };

enum class Axis { None, Horizontal, Vertical };

enum class Layer { None = 0, Tiled = 1 << 0, Floating = 1 << 1 };

inline Layer operator|(Layer a, Layer b) { return static_cast<Layer>((int) a | (int) b); }

inline Layer operator&(Layer a, Layer b) { return static_cast<Layer>((int) a & (int) b); }

#include "Hy3Node.hpp"
#include "TabGroup.hpp"
#include "conversions.hpp"

enum class FocusShift {
	Top,
	Bottom,
	Raise,
	Lower,
	Tab,
	TabNode,
};

enum class TabFocus {
	MouseLocation,
	Left,
	Right,
	Index,
};

enum class TabFocusMousePriority {
	Ignore,
	Prioritize,
	Require,
};

enum class SetSwallowOption {
	NoSwallow,
	Swallow,
	Toggle,
};

enum class ExpandOption {
	Expand,
	Shrink,
	Base,
	Maximize,
	Fullscreen,
};

enum class ExpandFullscreenOption {
	MaximizeOnly,
	MaximizeIntermediate,
	MaximizeAsFullscreen,
};

struct FocusOverride {
	Hy3Node* left = nullptr;
	Hy3Node* up = nullptr;
	Hy3Node* right = nullptr;
	Hy3Node* down = nullptr;

	Hy3Node** forDirection(ShiftDirection direction) {
		switch (direction) {
		case ShiftDirection::Left: return &left;
		case ShiftDirection::Up: return &up;
		case ShiftDirection::Right: return &right;
		case ShiftDirection::Down: return &down;
		default: UNREACHABLE();
		}
	}

	bool isEmpty() { return !(left || right || up || down); }
};

class Hy3Layout: public IHyprLayout {
public:
	virtual void onWindowCreated(CWindow*, eDirection = DIRECTION_DEFAULT);
	virtual void onWindowCreatedTiling(CWindow*, eDirection = DIRECTION_DEFAULT);
	virtual void onWindowRemovedTiling(CWindow*);
	virtual void onWindowRemovedFloating(CWindow*);
	virtual void onWindowFocusChange(CWindow*);
	virtual bool isWindowTiled(CWindow*);
	virtual void recalculateMonitor(const int& monitor_id);
	virtual void recalculateWindow(CWindow*);
	virtual void
	resizeActiveWindow(const Vector2D& delta, eRectCorner corner, CWindow* pWindow = nullptr);
	virtual void fullscreenRequestForWindow(CWindow*, eFullscreenMode, bool enable_fullscreen);
	virtual std::any layoutMessage(SLayoutMessageHeader header, std::string content);
	virtual SWindowRenderLayoutHints requestRenderHints(CWindow*);
	virtual void switchWindows(CWindow*, CWindow*);
	virtual void moveWindowTo(CWindow*, const std::string& direction);
	virtual void alterSplitRatio(CWindow*, float, bool);
	virtual std::string getLayoutName();
	virtual CWindow* getNextWindowCandidate(CWindow*);
	virtual void replaceWindowDataWith(CWindow* from, CWindow* to);
	virtual bool isWindowReachable(CWindow*);
	virtual void bringWindowToTop(CWindow*);

	virtual void onEnable();
	virtual void onDisable();

	void insertNode(Hy3Node& node);
	void makeGroupOnWorkspace(int workspace, Hy3GroupLayout, GroupEphemeralityOption);
	void makeOppositeGroupOnWorkspace(int workspace, GroupEphemeralityOption);
	void changeGroupOnWorkspace(int workspace, Hy3GroupLayout);
	void untabGroupOnWorkspace(int workspace);
	void toggleTabGroupOnWorkspace(int workspace);
	void changeGroupToOppositeOnWorkspace(int workspace);
	void changeGroupEphemeralityOnWorkspace(int workspace, bool ephemeral);
	void makeGroupOn(Hy3Node*, Hy3GroupLayout, GroupEphemeralityOption);
	void makeOppositeGroupOn(Hy3Node*, GroupEphemeralityOption);
	void changeGroupOn(Hy3Node&, Hy3GroupLayout);
	void untabGroupOn(Hy3Node&);
	void toggleTabGroupOn(Hy3Node&);
	void changeGroupToOppositeOn(Hy3Node&);
	void changeGroupEphemeralityOn(Hy3Node&, bool ephemeral);
	void shiftNode(Hy3Node&, ShiftDirection, bool once, bool visible);
	void shiftWindow(int workspace, ShiftDirection, bool once, bool visible);
	void shiftFocus(int workspace, ShiftDirection, bool visible, BitFlag<Layer> = Layer::None);
	void moveNodeToWorkspace(int origin, std::string wsname, bool follow);
	void changeFocus(int workspace, FocusShift);
	void focusTab(int workspace, TabFocus target, TabFocusMousePriority, bool wrap_scroll, int index);
	void setNodeSwallow(int workspace, SetSwallowOption);
	void killFocusedNode(int workspace);
	void expand(int workspace, ExpandOption, ExpandFullscreenOption);
	void resizeNode(const Vector2D& delta, eRectCorner corner, Hy3Node* node);

	bool shouldRenderSelected(CWindow*);

	Hy3Node* getWorkspaceRootGroup(const int& workspace);
	Hy3Node* getWorkspaceFocusedNode(
	    const int& workspace,
	    bool ignore_group_focus = false,
	    bool stop_at_expanded = false
	);

	static void renderHook(void*, SCallbackInfo&, std::any);
	static void windowGroupUrgentHook(void*, SCallbackInfo&, std::any);
	static void windowGroupUpdateRecursiveHook(void*, SCallbackInfo&, std::any);
	static void tickHook(void*, SCallbackInfo&, std::any);

	std::list<Hy3Node> nodes;
	std::list<Hy3TabGroup> tab_groups;

private:
	Hy3Node* getNodeFromWindow(CWindow*);
	void applyNodeDataToWindow(Hy3Node*, bool no_animation = false);
	void shiftFocusToMonitor(ShiftDirection direction);
	std::unordered_map<CWindow*, FocusOverride> m_focusIntercepts;
	Hy3Node* getFocusOverride(CWindow* src, ShiftDirection direction);
	void setFocusOverride(CWindow* src, ShiftDirection direction, Hy3Node* dest);

	// if shift is true, shift the window in the given direction, returning
	// nullptr, if shift is false, return the window in the given direction or
	// nullptr. if once is true, only one group will be broken out of / into
	Hy3Node* shiftOrGetFocus(Hy3Node&, ShiftDirection, bool shift, bool once, bool visible);

	void updateAutotileWorkspaces();
	bool shouldAutotileWorkspace(int);
	void resizeNode(Hy3Node*, Vector2D, ShiftDirection resize_edge_x, ShiftDirection resize_edge_y);
	void focusMonitor(CMonitor*);

	struct {
		std::string raw_workspaces;
		bool workspace_blacklist;
		std::set<int> workspaces;
	} autotile;

	friend struct Hy3Node;
};
