#include <filesystem>
#include <sstream>
#include <iomanip>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSizePolicy>
#include <QtGui/QImageReader>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>

#include <InvestigationEntry.hh>

namespace GenshinArtifactSpawnStat {

InvestigationEntry::InvestigationEntry(const QString& map_path, const QString& screenshot_path) {
	setFrameShape(QFrame::Panel);

	load_images(map_path, screenshot_path);
	init_drop_choice_box();

	QHBoxLayout* entry_layout = new QHBoxLayout;
	entry_layout->addWidget(m_map_label);
	entry_layout->addWidget(m_screenshot_label);
	entry_layout->addWidget(m_drop_choice_widget, 0, Qt::AlignRight);

	setLayout(entry_layout);
	setFocusPolicy(Qt::StrongFocus);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	setAutoFillBackground(true);
	adjustSize();
}

void InvestigationEntry::load_images(const QString& map_path, const QString& screenshot_path) {
	auto create_black_img = []() {
		QPixmap img(400, 400);
		QPainter p(&img);
		img.fill(QColor::fromRgb(0, 0, 0));
		return img;
	};

	m_map_label = new QLabel;
	m_map_image = create_black_img();
	if (std::filesystem::is_regular_file(map_path.toStdString())) {
		QImageReader map_reader{ map_path };
		if (map_reader.canRead())
			m_map_image = QPixmap::fromImageReader(&map_reader)
							.scaled(1000, 400, Qt::AspectRatioMode::KeepAspectRatio);
	}
	m_map_label->setPixmap(m_map_image);
	m_map_label->setFixedSize(m_map_image.size());

	m_screenshot_label = new QLabel;
	m_screenshot_image = create_black_img();
	if (std::filesystem::is_regular_file(screenshot_path.toStdString())) {
		QImageReader screenshot_reader{ screenshot_path };
		if (screenshot_reader.canRead())
			m_screenshot_image = QPixmap::fromImageReader(&screenshot_reader)
								   .scaled(1000, 400, Qt::AspectRatioMode::KeepAspectRatio);
	}
	m_screenshot_label->setPixmap(m_screenshot_image);
	m_screenshot_label->setFixedSize(m_screenshot_image.size());
}

void InvestigationEntry::init_drop_choice_box() {
	m_1_one_star_stats_label = new QLabel{ "- %" };
	m_2_one_star_stats_label = new QLabel{ "- %" };
	m_1_two_star_stats_label = new QLabel{ "- %" };
	m_record_num_label = new QLabel{ "Records: -" };
	m_avg_exp_label = new QLabel{ "Avg. exp: -" };

	m_1_one_star_button = new QRadioButton{ "★" };
	m_2_one_star_button = new QRadioButton{ "★ x2" };
	m_1_two_star_button = new QRadioButton{ "★★" };

	auto* drop_choice_layout = new QGridLayout;
	drop_choice_layout->addWidget(m_1_one_star_stats_label, 0, 0);
	drop_choice_layout->addWidget(m_2_one_star_stats_label, 1, 0);
	drop_choice_layout->addWidget(m_1_two_star_stats_label, 2, 0);
	drop_choice_layout->setColumnMinimumWidth(1, 10);
	drop_choice_layout->addWidget(m_1_one_star_button, 0, 2);
	drop_choice_layout->addWidget(m_2_one_star_button, 1, 2);
	drop_choice_layout->addWidget(m_1_two_star_button, 2, 2);
	drop_choice_layout->addWidget(m_record_num_label, 3, 0, 1, 3);
	drop_choice_layout->addWidget(m_avg_exp_label, 4, 0, 1, 3);

	m_drop_choice_widget = new QWidget;
	m_drop_choice_widget->setStyleSheet("background-color: rbga(0,0,0,0)");
	m_drop_choice_widget->setLayout(drop_choice_layout);
}

void InvestigationEntry::keyPressEvent(QKeyEvent* e) {
	if (!e->isAutoRepeat()) {
		if (e->key() == Qt::Key_1) m_1_one_star_button->toggle();
		if (e->key() == Qt::Key_2) m_2_one_star_button->toggle();
		if (e->key() == Qt::Key_3) m_1_two_star_button->toggle();
	}
	QWidget::keyPressEvent(e);
}

InvestigationEntry::Drop InvestigationEntry::drop() const {
	if (m_1_one_star_button->isChecked()) return Drop::SingleOneStar;
	if (m_2_one_star_button->isChecked()) return Drop::DoubleOneStar;
	if (m_1_two_star_button->isChecked()) return Drop::SingleTwoStar;
	return Drop::None;
}

void InvestigationEntry::set_drop(Drop drop) {
	m_1_one_star_button->setAutoExclusive(false);
	m_2_one_star_button->setAutoExclusive(false);
	m_1_two_star_button->setAutoExclusive(false);
	switch (drop) {
		case Drop::SingleOneStar:
			m_1_one_star_button->setChecked(true);
			break;
		case Drop::DoubleOneStar:
			m_2_one_star_button->setChecked(true);
			break;
		case Drop::SingleTwoStar:
			m_1_two_star_button->setChecked(true);
			break;
		case Drop::None:
			m_1_one_star_button->setChecked(false);
			m_2_one_star_button->setChecked(false);
			m_1_two_star_button->setChecked(false);
			break;
	}
	m_1_one_star_button->setAutoExclusive(true);
	m_2_one_star_button->setAutoExclusive(true);
	m_1_two_star_button->setAutoExclusive(true);
}

void InvestigationEntry::zoom(double factor) {
	m_map_image = m_map_image.scaled(m_map_image.size() * factor);
	m_map_label->setPixmap(m_map_image);
	m_map_label->setFixedSize(m_map_image.size());

	m_screenshot_image = m_screenshot_image.scaled(m_screenshot_image.size() * factor);
	m_screenshot_label->setPixmap(m_screenshot_image);
	m_screenshot_label->setFixedSize(m_screenshot_image.size());

	resize(size().width(), 500);
}

void InvestigationEntry::enable_choice(bool enable) {
	m_1_one_star_button->setEnabled(enable);
	m_2_one_star_button->setEnabled(enable);
	m_1_two_star_button->setEnabled(enable);
}

void InvestigationEntry::set_stats(int single_one_star_drops, int double_one_star_drops, int single_two_star_drops) {
	const auto records = single_one_star_drops + double_one_star_drops + single_two_star_drops;
	std::ostringstream records_oss;
	records_oss << "Records: " << records;
	m_record_num_label->setText(records_oss.str().c_str());

	auto avg_exp = 420.0 * (1.0 * single_one_star_drops + 2.0 * double_one_star_drops + 2.0 * single_two_star_drops);
	if (records > 0) avg_exp /= static_cast<float>(records);
	std::ostringstream avg_oss;
	avg_oss << "Avg. exp: " << std::fixed << std::setprecision(2) << avg_exp;
	m_avg_exp_label->setText(avg_oss.str().c_str());

	float s1_perc = single_one_star_drops;
	float d1_perc = double_one_star_drops;
	float s2_perc = single_two_star_drops;
	if (records > 0) {
		s1_perc /= static_cast<float>(records);
		d1_perc /= static_cast<float>(records);
		s2_perc /= static_cast<float>(records);
	}
	std::ostringstream s1_oss, d1_oss, s2_oss;
	s1_oss << std::fixed << std::setprecision(2) << 100.0 * s1_perc << " %";
	d1_oss << std::fixed << std::setprecision(2) << 100.0 * d1_perc << " %";
	s2_oss << std::fixed << std::setprecision(2) << 100.0 * s2_perc << " %";
	m_1_one_star_stats_label->setText(s1_oss.str().c_str());
	m_2_one_star_stats_label->setText(d1_oss.str().c_str());
	m_1_two_star_stats_label->setText(s2_oss.str().c_str());
}

}