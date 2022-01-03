#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <cmath>
#include <iterator>
#include <algorithm>
#include <array>

#include <QtCore/QSignalBlocker>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLayoutItem>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <cpr/cpr.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include <AppWindow.hh>

namespace GenshinArtifactSpawnStat {

AppWindow::AppWindow() {
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	create_menu();
	create_entries();

	m_central = new QScrollArea{};
	auto* main = new QWidget{};
	m_layout = new QGridLayout{};
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setColumnMinimumWidth(1, SPACING);

	for (std::size_t i = 0; i < m_entries.size(); i++) {
		m_layout->addWidget(m_entry_buttons[i], i, 0);
		m_layout->addWidget(m_entries[i], i, 2);
	}
	main->setLayout(m_layout);
	m_central->setWidgetResizable(true);
	m_central->setWidget(main);
	setCentralWidget(m_central);

	install_keyboard_navigation();

	update_max_width();
	resize(maximumWidth(), 1000);
	show();

	receive();
	load_route();
}

void AppWindow::create_menu() {
	m_load_action = new QAction{ "&Load" };
	connect(m_load_action, &QAction::triggered, this, &AppWindow::load);

	m_save_action = new QAction{ "&Save" };
	m_save_action->setShortcut(QKeySequence::Save);
	connect(m_save_action, &QAction::triggered, this, &AppWindow::save);

	m_send_action = new QAction("Send");
	connect(m_send_action, &QAction::triggered, this, &AppWindow::send);

	m_save_route_action = new QAction{ "&Confirm route" };
	connect(m_save_route_action, &QAction::triggered, this, [this]() {
		if (std::filesystem::exists(ROUTE_FILE)) {
			auto ans = QMessageBox::question(this, "Overwrite route?", "Route file 'route.dat' already exists. Overwrite?");
			if (ans == QMessageBox::Yes) {
				route_mode(false);
				save_route();
			}
		}
	});
	m_save_route_action->setEnabled(false);

	m_edit_route_action = new QAction{ "&Edit route" };
	connect(m_edit_route_action, &QAction::triggered, this, [this]() {
		route_mode(true);
	});

	m_zoom_action = new QAction{ "&Zoom images" };
	connect(m_zoom_action, &QAction::triggered, this, &AppWindow::zoom);

	m_file_menu = menuBar()->addMenu("&File");
	m_file_menu->addAction(m_load_action);
	m_file_menu->addAction(m_save_action);
	m_file_menu->addAction(m_send_action);
	m_edit_menu = menuBar()->addMenu("&Edit");
	m_edit_menu->addAction(m_edit_route_action);
	m_edit_menu->addAction(m_save_route_action);
	m_edit_menu->addSeparator();
	m_edit_menu->addAction(m_zoom_action);
}

void AppWindow::create_entries() {
	using namespace std;
	namespace fs = std::filesystem;

	auto str_3digit = [](unsigned i) {
		ostringstream oss;
		oss << setfill('0') << setw(3) << i;
		return oss.str();
	};

	const string resource_dir{ "resource/" };
	const unsigned file_count = std::distance(fs::directory_iterator{ resource_dir }, fs::directory_iterator{});

	QProgressDialog progress{ "Loading resources ...", QString{}, 0, static_cast<int>(file_count) };
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumWidth(500);
	int loaded_files = 0;

	for (unsigned i = 1; i <= file_count; ++i) { // file_count includes map & screenshot - i.e. loop will stop much earlier than i == file_count
		const auto stem = str_3digit(i);
		const auto map_file = resource_dir + stem + ".png";
		if (!fs::is_regular_file(map_file)) break;
		loaded_files++;

		for (char spot = 'a'; spot <= 'z'; ++spot) {
			const auto spot_file = resource_dir + stem + spot + ".png";
			if (!fs::is_regular_file(spot_file)) break;
			loaded_files++;

			add_entry(QString::fromStdString(map_file), QString::fromStdString(spot_file));
			add_entry_button();
			m_row_order.push_back(m_row_order.size());

			progress.setValue(loaded_files);
		}
	}
}

void AppWindow::add_entry(const QString& map_file, const QString& spot_file) {
	auto* entry = new InvestigationEntry{ map_file, spot_file };
	m_entries.push_back(entry);
}

void AppWindow::add_entry_button() {
	auto* entry_button = new QPushButton{};
	entry_button->setCheckable(true);
	entry_button->setEnabled(false);
	entry_button->setFixedWidth(BUTTON_WIDTH);
	entry_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	std::size_t i = m_entry_buttons.size();
	connect(entry_button, QPushButton::toggled, this, [this, i](bool checked) {
		entry_button_action(checked, i);
	});
	m_entry_buttons.push_back(entry_button);
}

std::string AppWindow::drops_as_json() const {
	namespace rj = rapidjson;
	rj::Document json{ rj::kArrayType };

	for (auto row : m_row_order) {
		rj::Value row_json{ rj::kArrayType };
		row_json.PushBack(row, json.GetAllocator());
		int drop_id = -1;

		switch (m_entries.at(row)->drop()) {
			case InvestigationEntry::Drop::SingleOneStar:
				drop_id = 0;
				break;
			case InvestigationEntry::Drop::DoubleOneStar:
				drop_id = 1;
				break;
			case InvestigationEntry::Drop::SingleTwoStar:
				drop_id = 2;
				break;
			case InvestigationEntry::Drop::None:
				drop_id = -1;
				break;
		}
		if (drop_id < 0) continue;

		row_json.PushBack(drop_id, json.GetAllocator());
		json.PushBack(row_json, json.GetAllocator());
	}
	std::ostringstream oss;
	rj::OStreamWrapper rj_oss{ oss };
	rj::Writer<rj::OStreamWrapper> json_writer{ rj_oss };
	json.Accept(json_writer);
	return oss.str();
}

void AppWindow::save() {
	auto save_file = QFileDialog::getSaveFileName(this, "Save", "", "*.dat");
	if (!save_file.isNull())
		std::ofstream{ save_file.toStdString() } << drops_as_json();
}

void AppWindow::load() {
	static const std::array<InvestigationEntry::Drop, 3> drop_dict{
		InvestigationEntry::Drop::SingleOneStar,
		InvestigationEntry::Drop::DoubleOneStar,
		InvestigationEntry::Drop::SingleTwoStar
	};

	auto save_file = QFileDialog::getOpenFileName(this, "Load", "", "*.dat");
	if (save_file.isNull()) return;
	std::ifstream ifs{ save_file.toStdString() };

	rapidjson::IStreamWrapper rj_ifs{ ifs };
	rapidjson::Document json;
	json.ParseStream(rj_ifs);

	// check input first
	if (!json.IsArray()) return;
	for (const auto& drop_arr : json.GetArray()) {
		if (!drop_arr.IsArray() || drop_arr.Size() != 2) return;
		const auto& row_val = drop_arr[0];
		const auto& drop_val = drop_arr[1];
		if (!row_val.IsInt() || !drop_val.IsInt() || drop_val.GetInt() < 0 || drop_val.GetInt() > 2) return;
	}

	// input ok -> can modify without errors
	m_row_order.clear();
	route_mode(true);
	for (const auto& drop_arr : json.GetArray()) {
		const auto& row = drop_arr[0].GetInt();
		const auto& drop = drop_arr[1].GetInt();
		m_row_order.push_back(row);
		m_entries.at(row)->set_drop(drop_dict[drop]);
	}
	route_mode(false);
}

void AppWindow::send() {
	cpr::Response res = cpr::Post(
	  cpr::Url{ HOST },
	  cpr::Body{ "{\"drops\":" + drops_as_json() + "}" },
	  cpr::Header{ { "Content-Type", "application/json" } });

	rapidjson::Document json;
	json.Parse(res.text.c_str());
	if (
	  res.status_code == 200 &&
	  json.HasMember("status") &&
	  json["status"].IsString() &&
	  json["status"].GetString() == std::string{ "success" }) {
		QMessageBox::information(this, "Upload successful",
		  "Your drops have been uploaded. Your selection will be reset.");

		for (auto* entry : m_entries)
			entry->set_drop(InvestigationEntry::Drop::None);
	} else {
		QMessageBox::warning(this, "Upload failed",
		  "Failed to upload your drops. Go to 'File > Save' or 'File > Load' to save/load your selection and try again later.");
	}
}

void AppWindow::zoom() {
	bool confirm = false;
	auto factor = QInputDialog::getDouble(this, "Zoom images", "Factor", 1.0, 0.1, 2.0, 2, &confirm, Qt::WindowFlags{}, 0.05);
	if (!confirm) return;

	for (auto* e : m_entries)
		e->zoom(factor);

	update_max_width();
	resize(maximumWidth(), size().height());
}

void AppWindow::update_max_width() {
	int max_entry_width = 0;
	for (const auto* e : m_entries)
		max_entry_width = std::max(max_entry_width, e->sizeHint().width());

	setMaximumWidth(max_entry_width + m_central->verticalScrollBar()->width() + BUTTON_WIDTH + SPACING + 10);
}

void AppWindow::route_mode(bool activate) {
	if (activate) {
		m_row_order.clear();
		remove_keyboard_navigation();
	}

	m_edit_route_action->setEnabled(!activate);
	m_save_route_action->setEnabled(activate);
	for (auto* e : m_entries)
		e->enable_choice(!activate);

	for (std::size_t i = 0; i < m_entries.size(); ++i) {
		m_entry_buttons[i]->setEnabled(activate);
		const QSignalBlocker ignore_check(m_entry_buttons[i]);
		m_entry_buttons[i]->setChecked(false);
	}

	if (!activate) {
		while (auto* item = m_layout->takeAt(0))
			item->widget()->hide();

		for (std::size_t i = 0; i < m_row_order.size(); i++) {
			auto row = m_row_order[i];

			m_layout->addWidget(m_entry_buttons[row], i, 0);
			m_layout->addWidget(m_entries[row], i, 2);
			m_entry_buttons[row]->show();
			m_entries[row]->show();
		}

		install_keyboard_navigation();
	}
}

void AppWindow::entry_button_action(bool checked, std::size_t row) {
	auto pos = std::find(begin(m_row_order), end(m_row_order), row);
	if (checked && pos == m_row_order.end())
		m_row_order.push_back(row);
	if (!checked && pos != m_row_order.end())
		m_row_order.erase(pos);
}

void AppWindow::install_keyboard_navigation() {
	if (m_selecthandler != nullptr) return;

	std::vector<QWidget*> navigation_order;
	for (std::size_t row : m_row_order)
		navigation_order.push_back(m_entries[row]);

	m_selecthandler = new DropSelectHandler{ navigation_order, m_central };
	m_central->installEventFilter(m_selecthandler);
}

void AppWindow::remove_keyboard_navigation() {
	if (m_selecthandler == nullptr) return;
	m_central->removeEventFilter(m_selecthandler);
	delete m_selecthandler;
	m_selecthandler = nullptr;
}

void AppWindow::save_route() const {
	std::ofstream route_ofs{ ROUTE_FILE };
	for (auto row : m_row_order)
		route_ofs << row << " ";
}

void AppWindow::load_route() {
	if (!std::filesystem::is_regular_file(ROUTE_FILE)) return;
	route_mode(true);

	std::ifstream route_ifs{ ROUTE_FILE };
	std::size_t row = 0;
	while (route_ifs >> row)
		if (row < m_entries.size())
			m_row_order.push_back(row);

	route_mode(false);
}

bool AppWindow::receive() {
	cpr::Response res = cpr::Get(cpr::Url{ HOST });
	if (res.status_code != 200) return false;

	rapidjson::Document json;
	json.Parse(res.text.c_str());
	if (!(
		  json.HasMember("error") &&
		  json["error"].IsBool() &&
		  !json["error"].GetBool() &&
		  json.HasMember("drops") &&
		  json["drops"].IsArray()))
		return false;

	const auto& drops = json["drops"];
	// validate data
	for (const auto& drop_arr : drops.GetArray()) {
		if (!drop_arr.IsArray() || drop_arr.Size() != 3) return false;
		for (const auto& num : drop_arr.GetArray())
			if (!num.IsInt()) return false;
	}

	for (std::size_t i = 0; i < drops.Size() && i < m_entries.size(); ++i) {
		const auto& drop_arr = drops[i].GetArray();
		m_entries[i]->set_stats(
		  drop_arr[0].GetInt(),
		  drop_arr[1].GetInt(),
		  drop_arr[2].GetInt());
	}
	return true;
}

}