#include <vector>

#include <QtWidgets/QMenu>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QAction>

#include <InvestigationEntry.hh>
#include <DropSelectHandler.hh>

#ifndef INCLUDE_GENSHINARTIFACTSPAWNSTAT_APPWINDOW_HH_
#define INCLUDE_GENSHINARTIFACTSPAWNSTAT_APPWINDOW_HH_

namespace GenshinArtifactSpawnStat {

class AppWindow : public QMainWindow {

	inline static constexpr int BUTTON_WIDTH = 50;
	inline static constexpr int SPACING = 5;

	inline static auto SAVE_FILE = "save.dat";
	inline static auto ROUTE_FILE = "route.dat";

	QMenu* m_file_menu = nullptr;
	QMenu* m_edit_menu = nullptr;
	QAction* m_save_action = nullptr;
	QAction* m_zoom_action = nullptr;
	QAction* m_edit_route_action = nullptr;
	QAction* m_save_route_action = nullptr;
	QScrollArea* m_central = nullptr;
	QGridLayout* m_layout = nullptr;

	std::vector<QPushButton*> m_entry_buttons;
	std::vector<InvestigationEntry*> m_entries;
	std::vector<std::size_t> m_row_order;
	DropSelectHandler* m_selecthandler = nullptr;

	void create_menu();
	void create_entries();
	void add_entry(const QString& map_file, const QString& spot_file);
	void add_entry_button();
	void update_max_width();
	void install_keyboard_navigation();
	void remove_keyboard_navigation();
	void route_mode(bool activate);
	void save_route();
	void load_route();

private slots:
	void save() const;
	void zoom();
	void entry_button_action(bool checked, std::size_t row);

public:
	AppWindow();
};

}

#endif